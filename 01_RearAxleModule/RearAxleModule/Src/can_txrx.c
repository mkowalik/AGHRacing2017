/*
 * can_txrx.c
 *
 *  Created on: Jul 25, 2017
 *      Author: Wojciech
 */

#include "can_txrx.h"

#ifdef __cplusplus
extern "C" {
#endif

static volatile uint8_t		empty_tx_mailboxes;
static volatile uint8_t		rx_buf_number;
static volatile rx_mes_t 	rx_mes[CAN_RX_SOFTWARE_BUFFER_SIZE];
static tx_mes_t				tx_mes;

#define CAN_VALIDATION_MULT	2

can_frame_t *can_frames[CAN_FRAMES_MAX_NUMB];
uint8_t		can_frame_number;

float can_data_calc_default(const uint16_t mult, const uint16_t div, const uint16_t offs, uint8_t * data_ptr, uint8_t size){
	float 		value;
	uint32_t	raw_data;

	raw_data = 0;
	for(uint8_t byte_ctr = 0; byte_ctr < size; byte_ctr++){
		raw_data	= (raw_data<<8) & 0xFFFFFFF0;
		raw_data   |= *(data_ptr + byte_ctr);
	}
	value = ((raw_data + offs) * mult) / div;

	return value;
}

void can_data_extract(const uint16_t mult, const uint16_t div, const uint16_t offs, uint8_t * data_ptr, uint8_t size, void * value){
	uint32_t raw_data;

	raw_data = (*((float *)value) * div) / mult - offs;

	for(uint8_t byte_ctr = 0; byte_ctr < size; byte_ctr++){
		*(data_ptr + byte_ctr) = raw_data & (0xFF << 8*byte_ctr);
	}
}

void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan){

	// Check if received real data
	if ((hcan->pRxMsg->IDE == CAN_ID_STD) && (hcan->pRxMsg->RTR == CAN_RTR_DATA)) {

		rx_mes[rx_buf_number].rec_time = HAL_GetTick();

		//CRITICAL_REGION_ENTER();

		// Increase buffer number
		rx_buf_number = (++ rx_buf_number == CAN_RX_SOFTWARE_BUFFER_SIZE) ? 0 : rx_buf_number;
		hcan->pRxMsg = ( CanRxMsgTypeDef *) &(rx_mes[rx_buf_number].can_rx);

		//CRITICAL_REGION_EXIT();
	}

	while(HAL_CAN_Receive_IT(hcan, CAN_FIFO0)){}
}

void HAL_CAN_TxCpltCallback(CAN_HandleTypeDef* hcan){

	//CRITICAL_REGION_ENTER();

	empty_tx_mailboxes ++;

	//CRITICAL_REGION_EXIT();

}

void can_frame_init(can_frame_t *frame){
	can_frames[can_frame_number] = 	frame;
	can_frame_number ++;
}

void can_data_init(can_data_t *data, bool rx){
	if(rx){
		data->can_frame->rx_on = true;
	}
	else{
		data->can_frame->tx_on = true;
	}
}

void can_filter_setter(CAN_HandleTypeDef * hcan){
	CAN_FilterConfTypeDef filterConfig;

	filterConfig.BankNumber = 			0;	// Must be kept 0, only 1 bank in uc
	filterConfig.FilterActivation = 	ENABLE;
	filterConfig.FilterScale = 			CAN_FILTERSCALE_16BIT;
	filterConfig.FilterMode = 			CAN_FILTERMODE_IDLIST;
	filterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;

	filterConfig.FilterNumber = 		0;

	filterConfig.FilterIdLow = 			0x100<<5;
	filterConfig.FilterIdHigh = 		0;
	filterConfig.FilterMaskIdLow = 		0;
	filterConfig.FilterMaskIdHigh = 	0;

	HAL_CAN_ConfigFilter(hcan, &filterConfig);
}

void can_task_manager_init(void){

	tx_mes.can_tx.RTR 	= CAN_RTR_DATA;
	tx_mes.can_tx.IDE	= CAN_ID_STD;
	empty_tx_mailboxes 	= 3;
	rx_buf_number 		= 0;

	can_filter_setter(&hcan);

	hcan.pRxMsg = (CanRxMsgTypeDef*) &(rx_mes[rx_buf_number].can_rx);
	hcan.pTxMsg = (CanTxMsgTypeDef*) &tx_mes.can_tx;

	for(uint8_t can_ctr = 0; can_ctr < can_frame_number; can_ctr ++){
		if(can_frames[can_ctr]->rx_on){
			while(HAL_CAN_Receive_IT(&hcan, CAN_FIFO0)){}
			return;
		}
	}
}

void can_task_manager(void){

	can_frame_t 	*can_frame;
	static uint8_t	tx_frame_number;
	static uint8_t	rx_frame_number;
	uint32_t		time;

	// Frames TX

	if(tx_frame_number == can_frame_number){
		tx_frame_number = 0;
	}
	for(;(tx_frame_number < can_frame_number) && (empty_tx_mailboxes > 0); tx_frame_number ++){
		can_frame = can_frames[tx_frame_number];
		if(can_frame->tx_on){
			time = HAL_GetTick();
			if(can_frame->rx_update_time + can_frame->m_can_frame->period < time){

				can_frame->rx_update_time = time;
				tx_mes.can_tx.DLC 	= can_frame->m_can_frame->dlc;
				tx_mes.can_tx.StdId	= can_frame->m_can_frame->id;

				for(uint8_t byte_ctr = 0; byte_ctr < tx_mes.can_tx.DLC; byte_ctr++){
					tx_mes.can_tx.Data[byte_ctr] = can_frame->tx_data[byte_ctr];
				}

				HAL_CAN_Transmit_IT(&hcan);

				//CRITICAL_REGION_ENTER();

				empty_tx_mailboxes --;

				//CRITICAL_REGION_EXIT();
			}
		}
	}

	// Frames RX

	// Save received data
	while(rx_frame_number != rx_buf_number){
		uint32_t id = rx_mes[rx_frame_number].can_rx.StdId;

		for(uint8_t frame = 0;frame < can_frame_number; frame ++){
			if(id == can_frames[frame]->m_can_frame->id){
				can_frame = can_frames[frame];

				can_frame->rx_update_time = rx_mes[rx_frame_number].rec_time;

				// copy data
				for(uint8_t byte_ctr = 0; byte_ctr < rx_mes[rx_frame_number].can_rx.DLC; byte_ctr++){
					can_frame->rx_data[byte_ctr] = rx_mes[rx_frame_number].can_rx.Data[byte_ctr];
				}

				break;
			}
		}

		rx_frame_number = (++ rx_frame_number == CAN_RX_SOFTWARE_BUFFER_SIZE) ? 0 : rx_frame_number;
	}

	// Validate frames
	for(uint8_t frame = 0;frame < can_frame_number; frame ++){
		can_frames[frame]->frame_valid = can_frames[frame]->rx_update_time + (can_frames[frame]->m_can_frame->period * CAN_VALIDATION_MULT) > time;
	}

}

#ifdef __cplusplus
}
#endif
