#ifndef __DATA_BLOCK_BURNING_RECORD_TABLE_H__
#define __DATA_BLOCK_BURNING_RECORD_TABLE_H__

#include "fibo_opencpu_comm.h"

void  data_block_burning_record_table_init(UINT16 block_sum);
void  data_block_burning_record_table_set(UINT16 block_code);
UINT8 *data_block_burning_record_table_value_get(void);
UINT8 data_block_burning_record_table_size_get(void);
void  data_block_burning_record_table_destroy(void);

#endif