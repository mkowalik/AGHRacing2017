/*
 * fixed_point.c
 *
 *  Created on: 05.05.2017
 *      Author: kowalik
 */

#include "fixed_point.h"

FixedPoint FixedPoint_constr(int32_t value,uint32_t divider, int32_t offset, uint8_t fractionalBits){	//TODO implement offset

	FixedPoint ret;

	ret.fractionalBits = fractionalBits;

	ret.integer = (value/divider) << fractionalBits;					//integer part of the number
	ret.integer |= ((value%divider) * (1<<fractionalBits)) / divider;	//(rest of division * 2^fractionalBits) / divider

	return ret;

}

uint8_t FixedPoint_a_equal_b(FixedPoint a, FixedPoint b){

	if (a.fractionalBits == b.fractionalBits)
		return a.integer == b.integer;

	uint8_t ret = 1;
	uint8_t diffBits;

	if (a.fractionalBits<b.fractionalBits){

		diffBits = b.fractionalBits - a.fractionalBits;
		ret &= (a.integer == (b.integer >> diffBits));
		ret &= ((a.integer << diffBits) == b.integer);

	} else {	// a.fractionalBits >= b.fractionalBits

		diffBits = a.fractionalBits - b.fractionalBits;
		ret &= ((a.integer >> diffBits) == b.integer);
		ret &= (a.integer == (b.integer << diffBits));

	}

	return ret;

}

uint8_t FixedPoint_a_lessorequal_b(FixedPoint a, FixedPoint b){

	if (a.fractionalBits == b.fractionalBits) return a.integer <= b.integer;

	if (a.fractionalBits<b.fractionalBits) return !FixedPoint_a_greater_b(b, a);

	//assumption a.fractionalBits >= b.fractionalBits

	return (FixedPoint_a_less_b(a, b) || FixedPoint_a_equal_b(a, b));

}

uint8_t FixedPoint_a_greaterorequal_b(FixedPoint a, FixedPoint b){

	if (a.fractionalBits == b.fractionalBits) return a.integer >= b.integer;

	if (a.fractionalBits<b.fractionalBits) return !FixedPoint_a_less_b(b, a);

	//assumption a.fractionalBits >= b.fractionalBits

	return (FixedPoint_a_greater_b(a, b) || FixedPoint_a_equal_b(a, b));

}

uint8_t FixedPoint_a_greater_b(FixedPoint a, FixedPoint b){

	if (a.fractionalBits == b.fractionalBits) return a.integer > b.integer;

	if (a.fractionalBits<b.fractionalBits) return !FixedPoint_a_lessorequal_b(b, a);

	//assumption a.fractionalBits >= b.fractionalBits

	uint8_t diffBits = a.fractionalBits - b.fractionalBits;

	if (((a.integer >> diffBits) > b.integer) != ((a.integer >> diffBits) < b.integer)) // false only if MSBits equal, checked for not losing information on MSBits of b
		return ((a.integer >> diffBits) > b.integer);

	return (a.integer > (b.integer << diffBits));

}

uint8_t FixedPoint_a_less_b(FixedPoint a, FixedPoint b){

	if (a.fractionalBits == b.fractionalBits) return a.integer < b.integer;

	if (a.fractionalBits<b.fractionalBits) return !FixedPoint_a_greaterorequal_b(b, a);

	//assumption a.fractionalBits >= b.fractionalBits

	uint8_t diffBits = a.fractionalBits - b.fractionalBits;

	if (((a.integer >> diffBits) > b.integer) != ((a.integer >> diffBits) < b.integer)) // false only if MSBits equal, checked for not losing information on MSBits of b
		return ((a.integer >> diffBits) < b.integer);

	return (a.integer < (b.integer << diffBits));
}

