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
#include "can.h"
#include "can_txrx_config.h"

#define PERIOD_ON_EVENT	((uint32_t) 0)

typedef struct{
	CanRxMsgTypeDef		can_rx;
	uint32_t			rec_time;
}rx_mes_t;

typedef struct{
	CanTxMsgTypeDef		can_tx;
}tx_mes_t;

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
	uint8_t			rx_data[8];
	uint8_t			tx_data[8];
	bool			rx_on;
	bool			tx_on;
}can_frame_t;

typedef struct{
	can_frame_t * const	can_frame;
	const uint16_t		multiplier;
	const uint16_t		divider;
	const uint16_t		offset;
	bool 				*data_valid;
	uint8_t	 			*data;
	uint8_t				data_size;
	float				( *data_calc)(const uint16_t mult, const uint16_t div, const uint16_t offs, uint8_t * data_ptr, uint8_t size);
	void				( *data_extract)(const uint16_t mult, const uint16_t div, const uint16_t offs, uint8_t * data_ptr, uint8_t size, void * value);
}can_data_t;

//extern can_frame_t *can_frames;
//extern uint8_t		can_frame_number;

void can_frame_init(can_frame_t *frame);
void can_data_init(can_data_t *data, bool rx);

float can_data_calc_default(const uint16_t mult, const uint16_t div, const uint16_t offs, uint8_t * data_ptr, uint8_t size);
void can_data_extract(const uint16_t mult, const uint16_t div, const uint16_t offs, uint8_t * data_ptr, uint8_t size, void * value);

void can_task_manager(void);
void can_task_manager_init(void);

#define DEFAULT_CALC_FUNC		(&can_data_calc_default)
#define DEFAULT_EXTRACT_FUNC	(&can_data_extract)

#define CAN_FRAME_DEF(name, 							\
					  period_ms, 						\
					  _id,								\
					  _dlc)								\
	   static m_can_frame_t CONCAT2(name, _m_frame) = {	\
			.period	= period_ms,						\
			.id		= _id,								\
			.dlc 	= _dlc								\
		};												\
		static can_frame_t CONCAT2(name, _frame) = {	\
			.m_can_frame = &CONCAT2(name, _m_frame),	\
			.rx_update_time = 0,						\
			.tx_update_time = 0,						\
			.frame_valid = false,						\
			.rx_on = false,								\
			.tx_on = false								\
		}

#define CAN_TX_DATA_DEF(name, 							\
						frame_name, 					\
						byte_offset, 					\
						_multiplier, 					\
						_divider, 						\
						_offset,						\
						_data_calc_func,				\
						_data_extract_func)				\
	static can_data_t CONCAT2(name, _data) = {			\
		.can_frame 	= &CONCAT2(frame_name, _frame),		\
		.multiplier	= _multiplier,						\
		.divider	= _divider,							\
		.offset		= _offset,							\
		.data_valid = false,							\
		.data 		= 									\
				(uint8_t *)&(CONCAT2(frame_name, _frame)\
				.tx_data[byte_offset]),					\
		.data_calc = _data_calc_func,					\
		.data_extract = _data_extract_func				\
		}

#define CAN_RX_DATA_DEF(name, 							\
						frame_name, 					\
						byte_offset, 					\
						_multiplier, 					\
						_divider, 						\
						_offset,						\
						_data_calc_func,				\
						_data_extract_func)				\
	static can_data_t CONCAT2(name, _data) = {			\
		.can_frame 	= &CONCAT2(frame_name, _frame),		\
		.multiplier	= _multiplier,						\
		.divider	= _divider,							\
		.offset		= _offset,							\
		.data_valid = false,							\
		.data 		= 									\
				(uint8_t *)&(CONCAT2(frame_name, _frame)\
				.rx_data[byte_offset]),					\
		.data_calc = _data_calc_func,					\
		.data_extract = _data_extract_func				\
	}

#define CAN_FRAME_INIT(name)		can_frame_init(&CONCAT2(name, _frame))
#define CAN_RX_DATA_INIT(name)		can_data_init(&CONCAT2(name, _data), true)
#define CAN_TX_DATA_INIT(name)		can_data_init(&CONCAT2(name, _data), false)
#define CAN_GET_DATA(name)			CONCAT2(name, _data).data_calc(					\
												CONCAT2(name, _data).multiplier,	\
												CONCAT2(name, _data).divider,		\
												CONCAT2(name, _data).offset,		\
												CONCAT2(name, _data).data,			\
												CONCAT2(name, _data).data_size)
#define CAN_SET_DATA(name, value)	CONCAT2(name, _data).data_extract(				\
												CONCAT2(name, _data).multiplier,	\
												CONCAT2(name, _data).divider,		\
												CONCAT2(name, _data).offset,		\
												CONCAT2(name, _data).data,			\
												CONCAT2(name, _data).data_size,		\
												value)

#define CAN_CHECK_VALIDATION(name)	CONCAT2(name, _data).can_frame->frame_valid

#define CAN_TASK_MANAGER()			can_task_manager()
#define CAN_TASK_MANAGER_INIT()		can_task_manager_init()

#ifdef __cplusplus
}
#endif

#endif /* CAN_TXRX_H_ */
