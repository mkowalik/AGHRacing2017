/*
 * fixed_point.c
 *
 *  Created on: 05.05.2017
 *      Author: kowalik
 */

#include "fixed_point.h"

static int32_t power(int32_t x, uint8_t p){

	if (x==10 && p==3) return 1000;	//simple optimization

	int32_t ret = 1;
	while (p--){
		ret *= x;
	}
	return ret;
}

uint8_t a_greater_b(FixedPoint a, FixedPoint b){

	int32_t a_int = a.integer / power(10, a.decimalDigits);
	int32_t b_int = b.integer / power(10, b.decimalDigits);

	if (a_int > b_int) return 1;
	else if (a_int < b_int) return 0;

	int8_t decDigitsDiff = a.decimalDigits -  b.decimalDigits;
	int8_t decDigitsMax = a.decimalDigits>b.decimalDigits ? a.decimalDigits : b.decimalDigits;

	if (decDigitsDiff>0){
		b.integer *= power(10, decDigitsDiff);
	} else if (decDigitsDiff<0) {
		a.integer += power(10, -decDigitsDiff);
	}

	a.integer %= decDigitsMax;
	b.integer %= decDigitsMax;

	return a.integer > b.integer;

}

uint8_t a_less_b(FixedPoint a, FixedPoint b){

	int32_t a_int = a.integer / power(10, a.decimalDigits);
	int32_t b_int = b.integer / power(10, b.decimalDigits);

	if (a_int < b_int) return 1;
	else if (a_int > b_int) return 0;

	int8_t decDigitsDiff = a.decimalDigits -  b.decimalDigits;
	int8_t decDigitsMax = a.decimalDigits>b.decimalDigits ? a.decimalDigits : b.decimalDigits;

	if (decDigitsDiff>0){
		b.integer *= power(10, decDigitsDiff);
	} else if (decDigitsDiff<0) {
		a.integer += power(10, -decDigitsDiff);
	}

	a.integer %= decDigitsMax;
	b.integer %= decDigitsMax;

	return a.integer < b.integer;

}

