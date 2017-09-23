/*
 * m_can.c
 *
 *  Created on: Aug 2, 2017
 *      Author: Wojciech
 */

#include "app_config.h"
#if CONFIG_CAN_LIB_ENABLED
#include "drv_config.h"
#include "drv_irq.h"
#include "fifo.h"
#include <string.h>
#include <stdlib.h>

#include "lib_can.h"
#include "can.h"

#define MAX_CAN_FRAMES_COUNT    6
#define MAX_CAN_DATA_COUNT      6
#define SOFTWARE_TX_FIFO        10
#define SOFTWARE_RX_FIFO        10
#define VALIDATION_MULT         2

typedef struct
{
    CanRxMsgTypeDef can_rx;
    uint32_t        rec_time;
}lib_can_rx_message_t;

typedef struct
{
    lib_can_rx_message_t rx1;
    lib_can_rx_message_t rx;
}lib_can_rx_fifo_message_t;

typedef struct
{
    CanTxMsgTypeDef can_tx;
}lib_can_tx_message_t;

static lib_can_frame_t         *s_lib_can_tx_frame_list[MAX_CAN_FRAMES_COUNT];
static lib_can_frame_t         *s_lib_can_rx_frame_list[MAX_CAN_FRAMES_COUNT];
static uint8_t                 s_lib_can_tx_frame_count;
static uint8_t                 s_lib_can_rx_frame_count;

static lib_can_data_t          *s_lib_can_tx_data_list[MAX_CAN_DATA_COUNT];
static lib_can_data_t          *s_lib_can_rx_data_list[MAX_CAN_DATA_COUNT];
static uint8_t                 s_lib_can_tx_data_count;
static uint8_t                 s_lib_can_rx_data_count;

static lib_can_data_t          *s_lib_can_tx_evt_data_list[MAX_CAN_DATA_COUNT];
static lib_can_data_t          *s_lib_can_rx_evt_data_list[MAX_CAN_DATA_COUNT];
static uint8_t                 s_lib_can_tx_evt_data_count;
static uint8_t                 s_lib_can_rx_evt_data_count;

static bool                    s_registering_frame;
static bool                    s_registering_data;

static volatile uint8_t        sv_tx_ongoing_count;

static Fifo_Handle_t           s_rx_fifo;
static Fifo_Handle_t           s_tx_fifo;

static lib_can_rx_fifo_message_t rx_message;

lib_can_frame_t * lib_can_register_frame(uint16_t __period, uint16_t __std_id, uint8_t __dlc, bool __transmit_frame)
{
    void                 *mem;
    bool                 may_register;
    lib_can_frame_t      *can_frame_ptr;

    CRITICAL_REGION_ENTER();
    if(!s_registering_frame)
    {
        s_registering_frame = true;
        may_register = true;
    }
    else
    {
        may_register = false;
    }
    CRITICAL_REGION_EXIT();

    if(!may_register)
    {
        return (void *) 0;
    }

    mem = malloc(sizeof(lib_can_frame_t) + __dlc);
    can_frame_ptr = (lib_can_frame_t *)mem;

    can_frame_ptr->dlc              = __dlc;
    can_frame_ptr->period           = __period;
    can_frame_ptr->std_id           = __std_id;
    can_frame_ptr->transmit_frame   = __transmit_frame;
    can_frame_ptr->frame_up_to_date = false;
    can_frame_ptr->update_time      = 0;
    can_frame_ptr->p_fdata           = (uint8_t *)(can_frame_ptr + 1);

    if (__transmit_frame)
    {
        can_frame_ptr->local_id = s_lib_can_tx_frame_count;
        s_lib_can_tx_frame_count++;
        s_lib_can_tx_frame_list[s_lib_can_tx_frame_count - 1] = can_frame_ptr;
    }
    else
    {
        can_frame_ptr->local_id = s_lib_can_rx_frame_count;
        s_lib_can_rx_frame_count++;
        s_lib_can_rx_frame_list[s_lib_can_rx_frame_count - 1] = can_frame_ptr;
    }

    CRITICAL_REGION_ENTER();
    s_registering_frame = false;
    CRITICAL_REGION_EXIT();

    return can_frame_ptr;
}


drv_status_t lib_can_unregister_frame(lib_can_frame_t *frame)
{
    bool            may_unregister;

    CRITICAL_REGION_ENTER();
    if(!s_registering_frame)
    {
        s_registering_frame = true;
        may_unregister = true;
    }
    else
    {
        may_unregister = false;
    }
    CRITICAL_REGION_EXIT();

    if (!may_unregister)
    {
        return DRV_ERROR;
    }

    if (frame->transmit_frame)
    {
        for(uint8_t frame_ctr = frame->local_id; frame_ctr < (s_lib_can_tx_frame_count - 1); frame_ctr++)
        {
            s_lib_can_tx_frame_list[frame_ctr] = s_lib_can_tx_frame_list[frame_ctr + 1];
        }
        s_lib_can_tx_frame_list[--s_lib_can_tx_frame_count] = (void *)0;
    }
    else
    {
        for(uint8_t frame_ctr = frame->local_id; frame_ctr < (s_lib_can_rx_frame_count - 1); frame_ctr++)
        {
            s_lib_can_rx_frame_list[frame_ctr] = s_lib_can_rx_frame_list[frame_ctr + 1];
        }
        s_lib_can_rx_frame_list[--s_lib_can_rx_frame_count] = (void *)0;
    }
    free((void *)frame);

    CRITICAL_REGION_ENTER();
    s_registering_frame = false;
    CRITICAL_REGION_EXIT();

    return DRV_OK;
}


lib_can_data_t * lib_can_register_data(lib_can_frame_t *frame,
                                       uint8_t __byte_offset,
                                       uint16_t __multiplier,
                                       uint16_t __divider,
                                       int16_t __offset,
                                       uint8_t __data_size,
                                       lib_can_data_e __data_type,
                                       bool __event_enable,
                                       void (*__event_handler_ptr)(lib_can_frame_t * frame),
                                       void (*__frame_2_real_data_ptr)(uint16_t mult, uint16_t div, int16_t offs, uint8_t data_siz, lib_can_data_e data_typ, uint8_t *data, void *real_data),
                                       void (*__real_data_2_frame_ptr)(uint16_t mult, uint16_t div, int16_t offs, uint8_t data_siz, lib_can_data_e data_typ, uint8_t *data, void *real_data))
{
    void    * mem;
    bool    may_register;
    uint8_t size_of_real_data;
    lib_can_data_t * can_data_ptr;

    CRITICAL_REGION_ENTER();
    if(!s_registering_data)
    {
        s_registering_data = true;
        may_register = true;
    }
    else
    {
        may_register = false;
    }
    CRITICAL_REGION_EXIT();

    if(!may_register)
    {
        return (void *) 0;
    }

    switch(__data_type)
    {
    case _boolean:
        size_of_real_data = sizeof(bool);
        break;
    case _uint8_t:
        size_of_real_data = 1;
        break;
    case _float:
        size_of_real_data = sizeof(float);
        break;
    default:
        size_of_real_data = 1;
    }

    mem = malloc(sizeof(lib_can_data_t) + size_of_real_data);
    can_data_ptr = (lib_can_data_t *)mem;

    can_data_ptr->multiplier          = __multiplier;
    can_data_ptr->divider             = __divider;
    can_data_ptr->offset              = __offset;
    can_data_ptr->p_data_up_to_date   = &frame->frame_up_to_date;
    can_data_ptr->p_data              = &frame->p_fdata[__byte_offset];
    can_data_ptr->data_size           = __data_size;
    can_data_ptr->data_type           = __data_type;
    can_data_ptr->p_real_data         = (uint8_t *)(can_data_ptr + 1);
    can_data_ptr->enable_evt_handler  = __event_enable;
    can_data_ptr->evt_handler         = __event_handler_ptr;
    can_data_ptr->p_frame_2_real_data = __frame_2_real_data_ptr;
    can_data_ptr->p_real_data_2_frame = __real_data_2_frame_ptr;
    can_data_ptr->p_can_frame         = frame;

    if (frame->transmit_frame)
    {
        if (__event_enable)
        {
            s_lib_can_tx_evt_data_count++;
            s_lib_can_tx_evt_data_list[s_lib_can_tx_evt_data_count - 1] = can_data_ptr;
        }
        else
        {
            s_lib_can_tx_data_count++;
            s_lib_can_tx_data_list[s_lib_can_tx_data_count - 1] = can_data_ptr;
        }
    }
    else
    {
        if (__event_enable)
        {
            s_lib_can_rx_evt_data_count++;
            s_lib_can_rx_evt_data_list[s_lib_can_rx_evt_data_count - 1] = can_data_ptr;
        }
        else
        {
            s_lib_can_rx_data_count++;
            s_lib_can_rx_data_list[s_lib_can_rx_data_count - 1] = can_data_ptr;
        }
    }

    CRITICAL_REGION_ENTER();
    s_registering_data = false;
    CRITICAL_REGION_EXIT();

    return can_data_ptr;
}


drv_status_t lib_can_unregister_data(lib_can_data_t *data)
{
    bool may_unregister;

    CRITICAL_REGION_ENTER();
    if(!s_registering_data)
    {
        s_registering_data = true;
        may_unregister = true;
    }
    else
    {
        may_unregister = false;
    }
    CRITICAL_REGION_EXIT();

    if(!may_unregister)
    {
        return DRV_ERROR;
    }

    if (data->p_can_frame->transmit_frame)
    {
        if (data->enable_evt_handler)
        {
            for(uint8_t data_ctr = data->local_id; data_ctr < (s_lib_can_tx_evt_data_count - 1); data_ctr++)
            {
                s_lib_can_tx_evt_data_list[data_ctr] = s_lib_can_tx_evt_data_list[data_ctr + 1];
            }
            s_lib_can_tx_evt_data_list[--s_lib_can_tx_evt_data_count] = (void *)0;
        }
        else
        {
            for(uint8_t data_ctr = data->local_id; data_ctr < (s_lib_can_tx_data_count - 1); data_ctr++)
            {
                s_lib_can_tx_data_list[data_ctr] = s_lib_can_tx_data_list[data_ctr + 1];
            }
            s_lib_can_tx_data_list[--s_lib_can_tx_data_count] = (void *)0;
        }
    }
    else
    {
        if (data->enable_evt_handler)
        {
            for(uint8_t data_ctr = data->local_id; data_ctr < (s_lib_can_rx_evt_data_count - 1); data_ctr++)
            {
                s_lib_can_rx_evt_data_list[data_ctr] = s_lib_can_rx_evt_data_list[data_ctr + 1];
            }
            s_lib_can_rx_evt_data_list[--s_lib_can_rx_evt_data_count] = (void *)0;
        }
        else
        {
            for(uint8_t data_ctr = data->local_id; data_ctr < (s_lib_can_rx_data_count - 1); data_ctr++)
            {
                s_lib_can_rx_data_list[data_ctr] = s_lib_can_rx_data_list[data_ctr + 1];
            }
            s_lib_can_rx_data_list[--s_lib_can_rx_data_count] = (void *)0;
        }
    }
    free((void *)data);

    CRITICAL_REGION_ENTER();
    s_registering_data = false;
    CRITICAL_REGION_EXIT();

    return DRV_OK;
}


void lib_can_handler(void)
{
    lib_can_rx_message_t     rx_message;
    lib_can_tx_message_t     tx_message;
    lib_can_frame_t          *frame;

    // receive messages
    while (FIFO_OK == Fifo_PullElement(&s_rx_fifo, (void *)&rx_message))
    {
        uint16_t id;

        id = rx_message.can_rx.StdId;
        for (uint8_t frame_ctr = 0; frame_ctr < s_lib_can_rx_frame_count; frame_ctr++)
        {
            if (s_lib_can_rx_frame_list[frame_ctr]->std_id == id)
            {
                s_lib_can_rx_frame_list[frame_ctr]->update_time = rx_message.rec_time;
                memcpy((uint8_t *)s_lib_can_rx_frame_list[frame_ctr]->p_fdata, rx_message.can_rx.Data, s_lib_can_rx_frame_list[frame_ctr]->dlc);
                break;
            }
        }
    }

    for (uint8_t frame_ctr = 0; frame_ctr < s_lib_can_tx_frame_count; frame_ctr++)
    {
        frame = s_lib_can_tx_frame_list[frame_ctr];
        if (frame->period == 0)
        {
            continue;
        }
        else if (frame->update_time + frame->period < HAL_GetTick())
        {
            tx_message.can_tx.DLC   = frame->dlc;
            tx_message.can_tx.StdId = frame->std_id;
            tx_message.can_tx.RTR   = CAN_RTR_DATA;
            tx_message.can_tx.IDE   = CAN_ID_STD;
            memcpy(tx_message.can_tx.Data, (uint8_t *)frame->p_fdata, frame->dlc);
            frame->update_time = HAL_GetTick();
            Fifo_PushElement(&s_tx_fifo, (void *)&tx_message);
        }
    }

    // send if no tx is ongoing
    if (sv_tx_ongoing_count == 0)
    {
        if (FIFO_OK == Fifo_PullElement(&s_tx_fifo, (void *)&tx_message))
        {
            hcan.pTxMsg = &tx_message.can_tx;
            sv_tx_ongoing_count++;
            HAL_CAN_Transmit_IT(&hcan);
        }
    }


    // update rx validation
    for (uint8_t frame_ctr = 0; frame_ctr < s_lib_can_rx_frame_count; frame_ctr++)
    {
        if(s_lib_can_rx_frame_list[frame_ctr]->update_time + VALIDATION_MULT*s_lib_can_rx_frame_list[frame_ctr]->period > HAL_GetTick())
        {
            s_lib_can_rx_frame_list[frame_ctr]->frame_up_to_date = true;
        }
        else
        {
            s_lib_can_rx_frame_list[frame_ctr]->frame_up_to_date = false;
        }
    }
}

void lib_can_empty_evt_handler(lib_can_frame_t * frame);

void lib_can_evt_trigger(lib_can_data_t * data);


void lib_can_default_frame_2_real_data(uint16_t mult, uint16_t div, int16_t offs, uint8_t data_siz, lib_can_data_e data_typ, uint8_t *data, void *real_data)
{
    float tmp_data;

    switch (data_typ)
    {
    case _boolean:
        if (*data > 0)
        {
            *(bool *)real_data = true;
        }
        else
        {
            *(bool *)real_data = false;
        }
        break;

    case _uint8_t:
        memcpy(real_data, data, 1);
        break;

    case _float:
        tmp_data = (float) (*data) - (float)(offs)
                                    / (float)(mult)
                                    * (float)(div);
        memcpy(real_data, &tmp_data, sizeof(float));
        break;
    }
}


void lib_can_default_real_data_2_frame(uint16_t mult, uint16_t div, int16_t offs, uint8_t data_siz, lib_can_data_e data_typ, uint8_t *data, void *real_data)
{
    float   tmp_data;
    int16_t tmp_data_int16;

    switch (data_typ)
    {
    case _boolean:
        if (*(bool *)real_data)
        {
            *data = 0xFF;
        }
        else
        {
            *data = 0x00;
        }
        break;

    case _uint8_t:
        memcpy(data, real_data, 1);
        break;

    case _float:
        tmp_data = *(float *)(real_data) * (float)(mult)
                                         / (float)(div)
                                         + (float)(offs);
        tmp_data_int16 = (int16_t) tmp_data;
        // to do: depend on data_size
        memcpy(data, &tmp_data_int16, 2);
        break;
    }
}


drv_status_t lib_can_data_set(lib_can_data_t *data_hdlr, void *data)
{
    lib_can_tx_message_t tx_message;

    data_hdlr->p_real_data_2_frame(data_hdlr->multiplier,
                                   data_hdlr->divider,
                                   data_hdlr->offset,
                                   data_hdlr->data_size,
                                   data_hdlr->data_type,
                                   (uint8_t *)data_hdlr->p_data,
                                   data);
    if (data_hdlr->enable_evt_handler)
    {
        tx_message.can_tx.DLC   = data_hdlr->p_can_frame->dlc;
        tx_message.can_tx.StdId = data_hdlr->p_can_frame->std_id;
        tx_message.can_tx.RTR   = CAN_RTR_DATA;
        tx_message.can_tx.IDE   = CAN_ID_STD;
        memcpy(tx_message.can_tx.Data, (uint8_t *)data_hdlr->p_can_frame->p_fdata, data_hdlr->p_can_frame->dlc);
        data_hdlr->p_can_frame->update_time = HAL_GetTick();

        if (sv_tx_ongoing_count == 0)
        {
            hcan.pTxMsg = &tx_message.can_tx;
            HAL_CAN_Transmit_IT(&hcan);
            sv_tx_ongoing_count++;
            return DRV_OK;
        }
        if (FIFO_OK == Fifo_PushElementToFront(&s_tx_fifo, &tx_message))
        {
            return DRV_OK;
        }
        else
        {
            return DRV_ERROR;
        }
    }
    return DRV_OK;
}


bool lib_can_data_get(lib_can_data_t *data_hdlr, void *data)
{
    data_hdlr->p_frame_2_real_data(data_hdlr->multiplier,
                                   data_hdlr->divider,
                                   data_hdlr->offset,
                                   data_hdlr->data_size,
                                   data_hdlr->data_type,
                                   (uint8_t *)data_hdlr->p_data,
                                   data);
    return *data_hdlr->p_data_up_to_date;
}

void lib_can_filter_set(CAN_HandleTypeDef * hcan){
    CAN_FilterConfTypeDef filterConfig;

    filterConfig.BankNumber =           0;  // Must be kept 0, only 1 bank in uc
    filterConfig.FilterActivation =     ENABLE;
    filterConfig.FilterScale =          CAN_FILTERSCALE_16BIT;
    filterConfig.FilterMode =           CAN_FILTERMODE_IDLIST;
    filterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;

    filterConfig.FilterNumber =         0;

    filterConfig.FilterIdLow =          0x100<<5;
    filterConfig.FilterIdHigh =         0x101<<5;
    filterConfig.FilterMaskIdLow =      0x102<<5;
    filterConfig.FilterMaskIdHigh =     0x103<<5;

    HAL_CAN_ConfigFilter(hcan, &filterConfig);
}


void (*lib_can_init(void))(void)
{
    s_rx_fifo = Fifo_Init(sizeof(lib_can_rx_message_t), SOFTWARE_RX_FIFO);
    s_tx_fifo = Fifo_Init(sizeof(lib_can_tx_message_t), SOFTWARE_TX_FIFO);
    sv_tx_ongoing_count = 0;

    lib_can_filter_set(&hcan);
    hcan.pRx1Msg = &rx_message.rx1.can_rx;
    hcan.pRxMsg  = &rx_message.rx.can_rx;
    HAL_CAN_Receive_IT(&hcan, CAN_FIFO0);
    HAL_CAN_Receive_IT(&hcan, CAN_FIFO1);

    return &lib_can_handler;
}

bool lib_can_find_and_handle_event(volatile CanRxMsgTypeDef * rx_msg)
{
    uint16_t id;

    id = rx_msg->StdId;
    for (uint8_t data_ctr = 0; data_ctr < s_lib_can_rx_evt_data_count; data_ctr++)
    {
        if (s_lib_can_rx_evt_data_list[data_ctr]->p_can_frame->std_id == id)
        {
            memcpy((uint8_t *)s_lib_can_rx_evt_data_list[data_ctr]->p_data, (uint8_t *)rx_msg->Data, s_lib_can_rx_evt_data_list[data_ctr]->p_can_frame->dlc);
            s_lib_can_rx_evt_data_list[data_ctr]->evt_handler(s_lib_can_rx_evt_data_list[data_ctr]->p_can_frame);
            return true;
        }
    }
    return false;
}


void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{
    // Check if received real data
    if ((hcan->pRxMsg->IDE == CAN_ID_STD) && (hcan->pRxMsg->RTR == CAN_RTR_DATA))
    {
        switch(hcan->State)
        {
        case(HAL_CAN_STATE_READY):
        case(HAL_CAN_STATE_BUSY_TX):
            if (!lib_can_find_and_handle_event(&rx_message.rx.can_rx))
            {
                rx_message.rx.rec_time = HAL_GetTick();
                Fifo_PushElement(&s_rx_fifo, (void *)&rx_message.rx);
            }
            HAL_CAN_Receive_IT(hcan, CAN_FIFO0);

            if (!lib_can_find_and_handle_event(&rx_message.rx1.can_rx))
            {
                rx_message.rx1.rec_time = HAL_GetTick();
                Fifo_PushElement(&s_rx_fifo, (void *)&rx_message.rx1);
            }
            HAL_CAN_Receive_IT(hcan, CAN_FIFO1);
            break;

        case(HAL_CAN_STATE_BUSY_RX0):
        case(HAL_CAN_STATE_BUSY_TX_RX0):
            if (!lib_can_find_and_handle_event(&rx_message.rx1.can_rx))
            {
                rx_message.rx1.rec_time = HAL_GetTick();
                Fifo_PushElement(&s_rx_fifo, (void *)&rx_message.rx1);
            }
            HAL_CAN_Receive_IT(hcan, CAN_FIFO1);
            break;

        case(HAL_CAN_STATE_BUSY_RX1):
        case(HAL_CAN_STATE_BUSY_TX_RX1):
            if (!lib_can_find_and_handle_event(&rx_message.rx.can_rx))
            {
                rx_message.rx.rec_time = HAL_GetTick();
                Fifo_PushElement(&s_rx_fifo, (void *)&rx_message.rx);
            }
            HAL_CAN_Receive_IT(hcan, CAN_FIFO0);
            break;

        default:
            break;
        }
    }
    else
    {
        switch(hcan->State)
        {
        case(HAL_CAN_STATE_READY):
        case(HAL_CAN_STATE_BUSY_TX):
            HAL_CAN_Receive_IT(hcan, CAN_FIFO0);
            HAL_CAN_Receive_IT(hcan, CAN_FIFO1);
            break;

        case(HAL_CAN_STATE_BUSY_RX0):
        case(HAL_CAN_STATE_BUSY_TX_RX0):
            HAL_CAN_Receive_IT(hcan, CAN_FIFO1);
            break;

        case(HAL_CAN_STATE_BUSY_RX1):
        case(HAL_CAN_STATE_BUSY_TX_RX1):
            HAL_CAN_Receive_IT(hcan, CAN_FIFO0);
            break;

        default:
            break;
        }
    }
}


void HAL_CAN_TxCpltCallback(CAN_HandleTypeDef* hcan)
{
    lib_can_tx_message_t tx_message;

    sv_tx_ongoing_count = 0;
    while (sv_tx_ongoing_count < 3)
    {
        if (FIFO_OK == Fifo_PullElement(&s_tx_fifo, (void *)&tx_message))
        {
            hcan->pTxMsg = &tx_message.can_tx;
            sv_tx_ongoing_count++;
            HAL_CAN_Transmit_IT(hcan);
        }
        else
        {
            break;
        }
    }
}

void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
    // tx failure
    if (hcan->ErrorCode == HAL_CAN_ERROR_TXFAIL)
    {
        sv_tx_ongoing_count = 0;
    }
}

#endif /* CONFIG_CAN_LIB_ENABLED */
