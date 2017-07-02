/*
 * LCD_UC1608.h
 *
 *  Created on: 16.04.2017
 *      Author: Marek
 */

#ifndef LCD_UC1608_H_
#define LCD_UC1608_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define LCD_CMD_ALL_PIX		0b10100100
#define LCD_CMD_ALL_PIX_m	0b00000001
#define LCD_CMD_DISP_EN		0b10101110
#define LCD_CMD_DISP_EN_m	0b00000001
#define LCD_CMD_SET_BR		0b11101000
#define LCD_CMD_SET_BR_m	0b00000011
#define LCD_CMD_SET_GP		0b10000001
#define LCD_CMD_SET_GP_m	0b11111111
#define LCD_CMD_SET_PC		0b00101000
#define LCD_CMD_SET_PC_m	0b00000111
#define LCD_CMD_SET_MC		0b11000000
#define LCD_CMD_SET_MC_m	0b00001101

void LCD_Init();
void LCD_SendCmd(uint8_t cmd, uint8_t mask, uint8_t val);
void LCD_SendData(uint8_t data);

#ifdef __cplusplus
}
#endif

#endif /* LCD_UC1608_H_ */
