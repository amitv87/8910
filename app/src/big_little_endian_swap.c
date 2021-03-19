#include "big_little_endian_swap.h"

UINT16 swap_endian(UINT16 val)
{
	UINT16 result;
	result = ((val << 8)&0xFF00) | ((val >> 8)&0x00FF);
	return result;
}