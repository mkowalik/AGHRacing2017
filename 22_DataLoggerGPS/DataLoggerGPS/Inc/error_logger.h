/*
 * error_logger.h
 *
 *  Created on: 07.01.2018
 *      Author: Kowalik
 */

#ifndef ERROR_LOGGER_H_
#define ERROR_LOGGER_H_

#define	ERROR_SYSTEM_CRASH				0x00000

#define	ERROR_CAN_BUS					0x10000
#define	ERROR_CAN_BUS_FOV0				0x10001
#define	ERROR_CAN_BUS_FOV1				0x10002
#define	ERROR_CAN_BUS_EPV				0x10003	// CAN Error passive state BUS
#define	ERROR_CAN_BUS_BOF				0x10004	// CAN Error BUS OFF state
#define	ERROR_CAN_BUS_ACK				0x10005
#define	ERROR_CAN_BUS_RECEIVE_FIFO0		0x10006
#define	ERROR_CAN_BUS_RECEIVE_FIFO1		0x10007

#define	ERROR_FILE_SYSTEM_MIDDLEWARE	0x20000

#define	ERROR_RTC_MIDDLEWARE			0x30000

#define	ERROR_MS_TIMER_MIDDLEWARE		0x40000

#define ERROR_CONFIG_MANAGER			0x50000

#define	ERROR_DATA_SAVER_BUFFER			0x60000

#define	ERROR_CAN_RECEIVER				0x70000

#define	ERROR_CAN_ACTION_SCHEDULER		0x80000

void logError(uint32_t errorNumber, char* comment, uint8_t reset);

#endif /* ERROR_LOGGER_H_ */
