#include <string.h>
#include "decoder.h"

#define MAX_KEYWORD_LENGHT 10
#define MAX_KEYWORD_NUMBER 2

typedef struct{
	keywordCode_t code;
	uint8_t string[MAX_KEYWORD_LENGHT + 1];
} keyword_t;

keyword_t keywordList[MAX_KEYWORD_NUMBER] ={
	{ID, "ID"},
	{CONNECT, "CONNECT"}
};

token_t token[MAX_TOKEN_NUMBER];
uint8_t tokenNumber;

result_t eHexStringToUInt(uint8_t string[], uint32_t * value);
void UIntToHexStr(uint32_t value, uint8_t string[]);

uint8_t ucFindTokensInString(uint8_t * string){
	enum AutomatState {TOKEN, DELIMITER};
	enum AutomatState eAutomateState = DELIMITER;
	uint8_t characterCounter, tempValue;
	
	tokenNumber = 0;
	
	for(characterCounter = 0; ; characterCounter++)
	{
		tempValue = string[characterCounter];
		
		switch(eAutomateState)
		{
			case TOKEN:
				if(tempValue == ' ') eAutomateState = DELIMITER;
				else if(tokenNumber == MAX_TOKEN_NUMBER) return tokenNumber;
				else if(tempValue == NULL_SIGN) return tokenNumber;
				break;
			
			case DELIMITER:
				if(tempValue == ' ') eAutomateState = DELIMITER;
				else if(tempValue == NULL_SIGN) return tokenNumber;
				else
				{
					token[tokenNumber].value.string = &string[characterCounter];
					eAutomateState = TOKEN;
					tokenNumber++;
				}
				break;
		}
	}
}

result_t eStringToKeyword(uint8_t string[], keywordCode_t *keywordCode){
	uint8_t keywordCounter;
	
	for(keywordCounter = 0; keywordCounter < MAX_KEYWORD_NUMBER; keywordCounter++)
	{
		if(strcmp((char *) string, (char *) keywordList[keywordCounter].string) == 0)
		{
			*keywordCode = keywordList[keywordCounter].code;
			return OK;
		}
	}
	return ERROR_;
}

void DecodeTokens(void){
	uint8_t tokenCounter;
    token_t *tokenBuf;
    uint8_t *tokenString;
    uint32_t *tokenValue;
    keywordCode_t *tokenKeyword;
    
    for(tokenCounter = 0; tokenCounter < tokenNumber; tokenCounter++){
    	tokenBuf = &token[tokenCounter];
    	tokenString = tokenBuf->value.string;
    	tokenValue = &(tokenBuf->value.number);
    	tokenKeyword = &(tokenBuf->value.keyword);

		if(eStringToKeyword(tokenString, tokenKeyword) == 0)
		{
			tokenBuf->type = KEYWORD;
		}
		else if(eHexStringToUInt(tokenString, tokenValue) == 0)
		{
			tokenBuf->type = NUMBER;
		}
		else
		{
			tokenBuf->value.string = tokenString;
			tokenBuf->type = STRING;
		}
	}
}

void DecodeMsg(uint8_t *string){
	ucFindTokensInString(string);
	ReplaceCharactersInString(string, ' ', NULL_SIGN);
	DecodeTokens();
}

void ReplaceCharactersInString(uint8_t string[], uint8_t oldChar, uint8_t newChar){
	uint8_t characterCounter;

	for(characterCounter = 0; string[characterCounter] != NULL_SIGN; characterCounter++){
		if(string[characterCounter] == oldChar)
		{
			string[characterCounter] = newChar;
		}
	}
}

result_t eHexStringToUInt(uint8_t string[], uint32_t * value){
	uint8_t characterCounter, strCharacter;
	uint32_t currentValue;
	
	*value = 0;

	if((string[0] != '0') || (string[1] != 'x') || (string[2] == NULL_SIGN)){
		return ERROR_;
	}
	
	for(characterCounter = 2; string[characterCounter] != NULL_SIGN; characterCounter++){
		if(characterCounter == 6) return ERROR_;

		strCharacter = string[characterCounter];
		
	if(strCharacter < 'A') currentValue = strCharacter - '0';
		else if(strCharacter < 'a') currentValue = strCharacter - 'A' + 10;
		else currentValue = strCharacter - 'a' + 10;

		*value = ((*value << 4) | currentValue);
	}
	return OK;
}

void UIntToHexStr(uint32_t value, uint8_t string[]){
	uint8_t nibbleCounter;
	uint8_t nibbleValue;

	string[0] = '0';
	string[1] = 'x';

    for(nibbleCounter = 0; nibbleCounter < 4; nibbleCounter ++){
    	nibbleValue = ((value >> ( 4*nibbleCounter )) & 0x0F);

		if (nibbleValue < 10) string[5 - nibbleCounter] = '0' + nibbleValue;
		else string[5-nibbleCounter] = 'A' + nibbleValue - 10;
    }
    string[6] = NULL_SIGN;
}

void CopyString(uint8_t source[], uint8_t destination[]){
	uint8_t characterCounter;

	for(characterCounter = 0; source[characterCounter] != NULL_SIGN; characterCounter++){
	  destination[characterCounter] = source[characterCounter];
	}

	destination[characterCounter] = NULL_SIGN;
}

void AppendString(uint8_t source[], uint8_t destination[]){
	uint8_t characterCounter;

	for(characterCounter = 0; destination[characterCounter] != NULL_SIGN; characterCounter++) {}

	CopyString(source, destination+characterCounter);
}
