/*
 * can_txrx.h
 *
 *  Created on: Jul 25, 2017
 *      Author: Wojciech
 */

#ifndef CAN_TXRX_H_
#define CAN_TXRX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "common.h"

#define PERIOD_ON_EVENT	((uint32_t) 0)

typedef struct{
	uint32_t 	period;
	uint16_t	id;
	uint8_t		dlc;
}m_can_frame_t;

typedef struct{
	m_can_frame_t *	m_can_frame;
	uint32_t		rx_update_time;
	uint32_t		tx_update_time;
	bool 			frame_valid;
	uint8_t			data[8];
}can_frame_t;

typedef struct{
	can_frame_t * const	can_frame;
	const uint16_t		multiplier;
	const uint16_t		divider;
	const uint16_t		offset;
	bool *				data_valid;
	uint16_t *			data;
	bool				receiver;
}can_data_t;

#define CAN_FRAME_DEF(name, 							\
					  period_ms, 						\
					  _id,								\
					  _dlc)								\
		static m_can_frame_t CONCAT2(name, _m_frame) = {\
			.period	= period_ms,						\
			.id		= _id,								\
			.dlc 	= _dlc								\
		};												\
		static can_frame_t CONCAT2(name, _frame) = {	\
			.m_can_frame = &CONCAT2(name, _m_frame),	\
			.rx_update_time = 0,						\
			.tx_update_time = 0,						\
			.frame_valid = false,						\
		}

#define CAN_TX_DATA_DEF(name, 							\
						frame_name, 					\
						byte_offset, 					\
						multiplier, 					\
						divider, 						\
						offset)							\
	static can_data_t CONCAT2(name, _data) = {			\
		.can_frame 	= &CONCAT2(frame_name, _frame),		\
		.multiplier	= multiplier,						\
		.divider	= divider,							\
		.offset		= offset,							\
		.data_valid = false,							\
		.data 		= 									\
				&((void)CONCAT2(frame_name, _frame)		\
				.data[byte_offset]),					\
		.receiver = false								\
	}

#define CAN_RX_DATA_DEF(name, 							\
						frame_name, 					\
						byte_offset, 					\
						_multiplier, 					\
						_divider, 						\
						_offset)						\
	static can_data_t CONCAT2(name, _data) = {			\
		.can_frame 	= &CONCAT2(frame_name, _frame),		\
		.multiplier	= _multiplier,						\
		.divider	= _divider,							\
		.offset		= _offset,							\
		.data_valid = false,							\
		.data 		= 									\
			(uint16_t *) &(CONCAT2(frame_name, _frame)	\
			.data[byte_offset]),						\
		.receiver = true								\
	}

#endif /* CAN_TXRX_H_ */
