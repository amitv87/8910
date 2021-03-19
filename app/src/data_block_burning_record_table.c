#include "data_block_burning_record_table.h"
#include "big_little_endian_swap.h"
#include "elog.h"

static UINT8 record_table_size                 = 0;         //记录表大小
static UINT8 *data_block_burning_record_table  = NULL;      //数据块烧写记录表

void data_block_burning_record_table_init(UINT16 block_sum)//数据块总数
{
    UINT16 total = swap_endian(block_sum);
	if(total%8)
    {
        record_table_size =(total/8)+1;   
        log_d("\r\nrecord_table_size：%d\r\n",record_table_size);//数据块烧写记录表大小

        data_block_burning_record_table=fibo_malloc(sizeof(UINT8)*record_table_size);

        for(int i = 0; i < (record_table_size-1); i++)
        {
            //数据块烧写记录表清零
            data_block_burning_record_table[i]=0;
        }

        int x=0xff;
        for(int j=0;j<(total%8);j++)
        {
            x&=~(1<<j)  ;          //将X的第j位清0
        }
        data_block_burning_record_table[record_table_size-1]=x;
        log_d("\r\nx：%x\r\n",x);
    }
    else
    {
        record_table_size = (total/8)+1;
        log_d("\r\nrecord_table_size：%d\r\n",record_table_size);//数据块烧写记录表大小

        data_block_burning_record_table=fibo_malloc(sizeof(char)*record_table_size);

        for(int i = 0; i < record_table_size; i++)
        {
            //数据块烧写记录表清零
            data_block_burning_record_table[i]=0;
        }
    }
}

void data_block_burning_record_table_set(UINT16 block_code)//数据块编号
{
    UINT16 number = swap_endian(block_code);
	data_block_burning_record_table[(number/8)] |= (1 << (number%8));
}

UINT8 *data_block_burning_record_table_value_get(void)
{
    return data_block_burning_record_table;
}

UINT8 data_block_burning_record_table_size_get(void)
{
    return record_table_size;
}

void data_block_burning_record_table_destroy(void)
{
	fibo_free(data_block_burning_record_table);
}