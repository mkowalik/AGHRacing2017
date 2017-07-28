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

can_frame_t *can_frames[CAN_FRAMES_MAX_NUMB];
uint8_t		can_frame_number;

CAN_FRAME_DEF(gear_up, PERIOD_ON_EVENT, 0x000, 1);
CAN_RX_DATA_DEF(gear_up, gear_up, 0, 1, 1, 0, DEFAULT_CALC_FUN, DEFAULT_EXTRACT_FUNC);

void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan){

	// Check if received real data
	if ((hcan->pRxMsg->IDE == CAN_ID_STD) && (hcan->pRxMsg->RTR == CAN_RTR_DATA)) {

		rx_mes[rx_buf_number].rec_time = HAL_GetTick();

		CRITICAL_REGION_ENTER();

		// Increase buffer number
		rx_buf_number = (++ rx_buf_number == CAN_RX_SOFTWARE_BUFFER_SIZE) ? 0 : rx_buf_number;
		hcan->pRxMsg = &(rx_mes[rx_buf_number].can_rx);

		CRITICAL_REGION_EXIT();
	}

	HAL_CAN_Receive_IT(hcan, CAN_FIFO0);
}

void HAL_CAN_TxCpltCallback(CAN_HandleTypeDef* hcan){

	CRITICAL_REGION_ENTER();

	empty_tx_mailboxes ++;

	CRITICAL_REGION_EXIT();

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

void can_task_manager_init(void){
	tx_mes.can_tx.RTR 	= CAN_RTR_DATA;
	tx_mes.can_tx.IDE	= CAN_ID_STD;
}

void can_task_manager(void){

	can_frame_t 	*can_frame;
	static uint8_t	tx_frame_number;
	static uint8_t	rx_frame_number;
	uint32_t		time = HAL_GetTick();

	// Frames TX

	for(;(tx_frame_number < can_frame_number) && (empty_tx_mailboxes > 0); tx_frame_number ++){
		can_frame = can_frames[tx_frame_number];

		if(can_frame->rx_update_time + can_frame->m_can_frame->period < time){

			tx_mes.can_tx.DLC 	= can_frame->m_can_frame->dlc;
			tx_mes.can_tx.StdId	= can_frame->m_can_frame->id;

			for(uint8_t byte_ctr = 0; byte_ctr < tx_mes.can_tx.DLC; byte_ctr++){
				tx_mes.can_tx.Data[byte_ctr] = can_frame->tx_data[byte_ctr];
			}

			HAL_CAN_Transmit_IT(&hcan);

			CRITICAL_REGION_ENTER();

			empty_tx_mailboxes --;

			CRITICAL_REGION_EXIT();
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
		can_frames[frame]->frame_valid = can_frames[frame]->rx_update_time + can_frames[frame]->m_can_frame->period > time;
	}

}

#ifdef __cplusplus
}
#endif
