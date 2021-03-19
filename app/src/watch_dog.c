#include "watch_dog.h"
#include "elog.h"
#include "fibo_opencpu.h"
#include "osi_mem.h"
#include "gpio_operate.h"

int g_stop_dog_flag = 0;

void feed_the_dog()
{
	fibo_gpio_set(WDI,high);	
    fibo_taskSleep(500);        //500ms翻转一次
	fibo_gpio_set(WDI,low);	
    fibo_taskSleep(500);        //500ms翻转一次
}

void heap_status(void);

void watch_dog_task()
{
    while(1)
    {
        log_d("\r\n%s()\r\n",__func__);
        if(0 == g_stop_dog_flag)
        {
            feed_the_dog();
        }
        // heap_status();
    }
}

void feed_dog_task(void *param)
{	
	watch_dog_task();	
}

void heap_status(void)
{
    log_d("\r\n%s()\r\n",__func__);

    uint32_t heap_size              =   0;
    uint32_t heap_avail_size        =   0;
    uint32_t heap_max_block_size    =   0;

    INT32 heap_ret = fibo_get_heapinfo(&heap_size,&heap_avail_size,&heap_max_block_size);

    log_d("\r\nheap_ret             is %d\r\n",heap_ret);
    log_d("\r\nheap_size            is %d\r\n",heap_size);
    log_d("\r\nheap_avail_size      is %d\r\n",heap_avail_size);
    log_d("\r\nheap_max_block_size  is %d\r\n",heap_max_block_size);
}