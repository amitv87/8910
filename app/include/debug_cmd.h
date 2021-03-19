#ifndef __EDBUG_CMD_H__
#define __EDBUG_CMD_H__

void  Capital2Lowercase_str(void *buf, u16_t len);
u8_t  Capital_To_Lowercase(u8_t bHex);
void  STRCMD_AtTest(void *para);
void  uart_set_get_para(void *command);

#endif // __EDBUGCMD_H__