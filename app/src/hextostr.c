#include "hextostr.h"
#include "elog.h"

void hextostr(void *hexbuff, void * strtarget, uint32_t len)//十六进制转字符串
{
    uint32_t    i            = 0;
    char        *hexbuff_p   = (char *)hexbuff;
    char        *strtarget_p = (char *)strtarget;

    if(!hexbuff || !strtarget)
    {
        log_a("hextostr address default!");
    }
    if(!len)
    {
        log_a("data len error");
    }

    for(i = 0; i < len; i++)
    {
        strtarget_p[2*i]     = "0123456789ABCDEF"[(hexbuff_p[i] >> 4) & 0x0f];
        strtarget_p[2*i + 1] = "0123456789ABCDEF"[(hexbuff_p[i]) & 0x0f];
    }
}