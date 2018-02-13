/*
 * error_logger.h
 *
 *  Created on: 07.01.2018
 *      Author: Kowalik
 */

#ifndef ERROR_LOGGER_H_
#define ERROR_LOGGER_H_

#define	ERROR_SYSTEM_CRASH	0x00000

#define	ERROR_CAN					0x10000
#define	ERROR_CAN_FOV0				0x10001
#define	ERROR_CAN_FOV1				0x10002
#define	ERROR_CAN_EPV				0x10003	// CAN Error passive state BUS
#define	ERROR_CAN_BOF				0x10004	// CAN Error BUS OFF state
#define	ERROR_CAN_ACK				0x10005
#define	ERROR_CAN_RECEIVE_FIFO0		0x10006
#define	ERROR_CAN_RECEIVE_FIFO1		0x10007

void logError(uint32_t errorNumber, char* comment);

#endif /* ERROR_LOGGER_H_ */
