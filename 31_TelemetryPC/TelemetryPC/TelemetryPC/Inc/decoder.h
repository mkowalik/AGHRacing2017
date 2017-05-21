#ifndef DECODER_H
#define DECODER_H

#include "stm32f0xx_hal.h"

#define NULL_SIGN			(0)
#define MAX_TOKEN_NUMBER 	1

typedef enum {CONNECT} keywordCode_t;
typedef enum {KEYWORD, NUMBER, STRING} tokenType_t;
typedef enum {OK, ERROR_} result_t;
typedef union {
	keywordCode_t keyword;
	uint32_t number;
	uint8_t *string;
} tokenValue_t;

typedef struct{
	tokenType_t type;
	tokenValue_t value;
} token_t;

void ReplaceCharactersInString(uint8_t string[], uint8_t oldChar, uint8_t newChar);
void DecodeMsg(uint8_t * string);
void CopyString(uint8_t source[], uint8_t destination[]);
void AppendString(uint8_t sourceString[], uint8_t destinationString[]);

extern token_t token[MAX_TOKEN_NUMBER];
extern uint8_t tokenNumber;

#endif
