/***************************Copyright QYKJ   2015-06-18*************************
文	件：	Swap.c
说	明：	数据交换相关函数
修　改：	2015.06.28 Unarty establish
            2017.08.11 Unarty change Swap_hexChar & Swap_charHex add ' ' 
*******************************************************************************/
#ifdef _PLATFORM_BC25_
#include "ql_stdlib.h"			        //mike 20200828
#endif

#include "eyblib_swap.h"
#include "eyblib_r_stdlib.h"			//mike 20200803
// #include "eybpub_debug.h"

/*******************************************************************************
函 数 名：	Swap_bigSmallShort
功能说明： 	0xaa55 -> 0x55aa
参	  数： 	pShort:     短整形数据
返 回 值：	无
*******************************************************************************/
void Swap_bigSmallShort(u16_t *pShort)
{
    u8_t *p = (u8_t*)pShort;

    *pShort = (p[0]<<8)  | p[1];
}
                                                                                
/*******************************************************************************
函 数 名：	Swap_bigSmallLong
功能说明： 	0x12345678 -> 0x87654321
参	  数： 	pLong:  长整形数据
返 回 值：	无
*******************************************************************************/
void Swap_bigSmallLong(u32_t *pLong)
{
    u8_t *p = (u8_t*)pLong;

    *pLong = (p[0]<<24) | (p[1]<<16) | (p[2]<<8) | p[3];
}

/*******************************************************************************
函 数 名：	Swap_headTail
功能说明： 	0xaabbccddeeffgg -> 0xggffeeddccbbaa
参	  数： 	*head:  数据首地址
            len:    数据长度
返 回 值：	无
*******************************************************************************/
void Swap_headTail(u8_t *head, u16_t len)
{
    u8_t *p = head + len - 1;

    while (head < p)
    {
        *head ^= *p;
        *p ^= *head;
        *head++ ^= *p--;
    }
}

/*******************************************************************************
函 数 名：	Swap_hexChar
功能说明： 	0x12,0x34,0x56,0x78,0xab,0x0c -> "12 34 56 78 ab 0c"
参	  数： 	*buf:  字符存储地址
            *hex;   目标转换字
            len:    字长度
            fill:   每个数据间需填充的值，0: 不做填充 
返 回 值：	字符长度
*******************************************************************************/
u16_t Swap_hexChar(char *buf, u8_t *hex, u16_t len, char fill)
{
    u8_t i;
    u16_t l = len << 1;
    
    while (len-- > 0)
    {
        i = (*hex) >> 4;
        *buf++ = i > 9 ? i + ('A' - 10) : i + '0';
        i = (*hex++)&0x0F;
        *buf++ = i > 9 ? i + ('A' - 10) : i + '0';
        if (fill != 0)
        {
            *buf++ = fill;
        }
    }
    *buf = '\0';

    return l;
}

/*******************************************************************************
函 数 名：	Swap_charHex
功能说明： 	"12 34 56 87 ab 0c" -> 0x12,0x34,0x56,0x78,0xab,0x0c
参	  数： 	*hex;   目标存储地址
            *buf:   字符转换字
返 回 值：	转换后目标长度
*******************************************************************************/
u16_t Swap_charHex(u8_t *hex, char *buf)  
{
    char i;
    u16_t cnt = 0;

    *hex = 0;
    while (1)
    {
        i = *buf++;

        if ('0' <= i && i <= '9')
        {
            cnt++;
            i -= '0';
        }
        else if ('a' <= i && i <= 'f')
        {
            cnt++;
            i -= 'a' - 10;
        }
        else if ('A' <= i && i <= 'F')
        {
            cnt++;
            i -= 'A' - 10;
        }
        else if (' ' == i)
        {
            continue;
        }
        else
        {
            break;
        }
        
        *hex <<= 4;
        *hex |= i; 
        if (0 == (cnt&0x01))
        {
            *(++hex) = 0;
        }
    }

    return ((cnt+1)>>1);
}

/*******************************************************************************
函 数 名：	Swap_numCharFixed
功能说明： 	数值转换成字符串，按固定长度，
参	  数： 	num;   目标转换数字 64位
            *buf:  转换后的字符串
            bufLen: 字符串的长度
返 回 值：	NODE
*******************************************************************************/
void Swap_numCharFixed(char *buf, u8_t bufLen, u64_t num) 
{
/*    u8_t i = bufLen;
    char *p = buf;

    while (bufLen-- > 0)
    {
        *p++ = num%10 + '0';		//mike 20200806 64位整除、取余会死机
        num /= 10;
    }
    *p = '\0';
    Swap_headTail((u8_t*)buf, i);*/

	u8_t i = bufLen;
    char *p = buf;
	u32_t nTemp_num = num;

    while (bufLen-- > 0)
    {
        *p++ = nTemp_num%10 + '0';		//mike 20200806
        nTemp_num /= 10;
    }
    *p = '\0';
    Swap_headTail((u8_t*)buf, i);
}


/*******************************************************************************
函 数 名：	Swap_numChar
功能说明： 	0x0400 -> "1024" 
参	  数： 	num;   目标转换数字 64位
            *buf:  转换后的字符串
返 回 值：	转换后字符串长度
*******************************************************************************/
u8_t  Swap_numChar(char *buf, u32_t num) 
{
    u8_t i = 0;
    char *p = buf;

    do 
    {
        i++;
        *p++ = num%10 + '0';
        num /= 10;
    } while (num > 0);
    *p = '\0';
    Swap_headTail((u8_t*)buf, i);

    return i;
}

/*******************************************************************************
函 数 名：	Swap_charNum
功能说明： 	 "1024"  -> 0x0400
参	  数： 	*buf:  目标转换字符串
返 回 值：	转换后的值
*******************************************************************************/
u32_t Swap_charNum(char *buf)
{
    u32_t num = 0;
    
    while ('0' <= *buf && *buf <= '9')
    {
        num *= 10;
        num += *buf++ - '0';
    }

    return num;
}


/*******************************************************************************
函 数 名：	Swap_numHexChar
功能说明： 	0x0400 -> "400"  0x0000 -> ""
参	  数： 	num;   目标转换数字
            *buf:  转换后的字符串
返 回 值：	转换后字符串长度
*******************************************************************************/
u8_t  Swap_numHexChar(char *buf, u16_t num) 
{
	u8_t val;
    u8_t i = 0;
    char *p = buf;

    do 
    {
        i++;
		val = num&0x0F;
        *p++ = ((val > 9) ? (val + 'A' - 10) : (val + '0'));
        num >>= 4;
    } while (num > 0);
    *p = '\0';
    Swap_headTail((u8_t*)buf, i);

    return i;
}

/*******************************************************************************
函 数 名：	Swap_HexCharNum
功能说明： 	 "1024"  -> 0x1024
参	  数： 	*buf:  目标转换字符串
返 回 值：	转换后的值
*******************************************************************************/
u32_t Swap_HexCharNum(char *buf)
{
	int i = 0;
    int num_eyb = 0;
//    int j = 0, m = 0;
//    int num_eyb = 0, pow = 1, temp = 0;
//	char strTemp[16] = {0};
    
    while (*buf != '\0')
    {
		i = *buf++;
		if ('0' <= i && i <= '9')
        {
            i -= '0';
        }
        else if ('a' <= i && i <= 'f')
        {
            i -= 'a' - 10;
        }
        else if ('A' <= i && i <= 'F')
        {
            i -= 'A' - 10;
        }
        else
        {
            break;
        }
		num_eyb <<= 4;
		num_eyb |= i;
    }
/*    u8_t len = Ql_strlen(buf);
    Ql_memset(strTemp, 0, sizeof(char) * 16);
    Ql_strncpy(strTemp, buf, 16);
    APP_DEBUG("%s len %d\r\n", strTemp, len);
    for (j = (len - 1); j >= 0; j--) {
      m = 0;
      if ('0' <= strTemp[j] && strTemp[j] <= '9') {
        m = strTemp[j] - '0';
      } else if ('a' <= strTemp[j] && strTemp[j] <= 'f') {
        m = strTemp[j] - 'a' + 10;
      } else if ('A' <= strTemp[j] && strTemp[j] <= 'F') {
        m = strTemp[j] - 'A' + 10;
      } else {
        APP_DEBUG("BREAK\r\n");
        break;
      }
      pow = 1;
      for (i = 0; i < (len - 1 - j); i++) {
        pow = pow * 16;
      }
      temp = m * pow;
      num_eyb = num_eyb + temp;
//      num_eyb = 0x2FD + 0x2;
      APP_DEBUG("index: %d input: %02X val: %ld %04X %ld %ld\r\n", j, strTemp[j], m, pow, temp, num_eyb);
    } */
    return num_eyb;
}


/*******************************************************************************
函 数 名：	Swap_StringSignedNum
功能说明： 	字符串转为有符号数
参	  数： 	*buf:  转换后的字符串
            
返 回 值：	转换后字符串长度
*******************************************************************************/
s32_t  Swap_stringSigned(char *buf) 
{
    u8_t flag = 0;
    s32_t num = 0;

	if (*buf == '-')
	{
		flag = 1;
		buf++;
	}

	while ('0' <= *buf && *buf <= '9')
    {
        num *= 10;
        num += *buf++ - '0';
    }

    if (flag == 1)
    {
        num = (~num) + 1;
    }
	
    return num;
}


/*******************************************************************************
函 数 名：	Swap_StringSigned
功能说明： 	有符号数转换为字符串
参	  数： 	num;   目标转换数字
            *buf:  转换后的字符串
返 回 值：	转换后字符串长度
*******************************************************************************/
u8_t  Swap_signedString(char *buf, s32_t num) 
{
    u8_t flag = 0, i = 0;
    char *p = buf;

    if (num < 0)
    {
        *p++ = '-';
        num = ~((u32_t)(num)) + 1;
        flag = 1;
    }
    
    do 
    {
        i++;
        *p++ = num%10 + '0';
        num /= 10;
    } while (num > 0);
    *p = '\0';
    Swap_headTail((u8_t*)buf, i);

    return i + flag;
}


/*******************************************************************************
函 数 名：	Swap_s64Char
功能说明： 	0x14 -> "20" 0xFF -> "-1"
参	  数： 	num;   目标转换数字 64位
            *buf:  转换后的字符串
返 回 值：	转换后字符串长度
*******************************************************************************/
u8_t  Swap_CharString(char *buf, s8_t num) 
{
    u8_t flag = 0, i = 0;
    char *p = buf;

    if (num&0x80)
    {
        buf++;
        *p++ = '-';
        num = ~((u8_t)num) + 1;
        flag = 1;
    }
    
    do 
    {
        i++;
        *p++ = num%10 + '0';
        num /= 10;
    } while (num > 0);
    *p = '\0';
    Swap_headTail((u8_t*)buf, i);

    return i + flag;
}

/*******************************************************************************
函 数 名：	Swap_BCDChar
功能说明： 	0x123456 -> "123456" 
参	  数： 	*buf:   转换结果字符串
            *bcd:   目标转换BCD数
            len:    BCD码长度
返 回 值：	字符串长度
*******************************************************************************/
u16_t Swap_BCDChar(char *buf, u8_t *bcd, u16_t len)
{
    u8_t i;
    u16_t cnt = 0;

    while (len-- > 0)
    {
//        i = len&0x01 ? *buf>>4 : *buf&0x0F;
		i = len&0x01 ? (*bcd)>>4 : ((*bcd)&0x0F);	//mike 20200810
        if (i > 9)
        {
            break;
        }

        if (0 == cnt && 0 == i) //去除前面的零
        {}
        else 
        {
            *buf++ = i + '0';
            cnt++;
        }
    }
    *buf = '\0';

    return cnt;
}

/*******************************************************************************
函 数 名：	Swap_CharBCD
功能说明： 	98765" -> 0x098765
参	  数： 	*bcd:   转换后BCD码
            *buf:   目标转换字符串
返 回 值：	BCD码长度
*******************************************************************************/
u16_t Swap_CharBCD(u8_t *bcd, char *buf)
{
    u16_t len, cnt;

//    for (len = r_strlen(buf), cnt = 0, *bcd = 0; '0' <= *buf && *buf <= '9'; len--)
    for (len = r_strlen(buf), cnt = 0, *bcd = 0; '0' <= *buf && *buf <= '9'; len--)    // mike 20200828
    {
        *bcd |= *buf - '0';
        if (0 == (len&0x01))
        {
            *bcd <<= 4;
        }
        else
        {
            *(++bcd) = 0;
            cnt++;
        }
    }

    return cnt;
}

/*******************************************************************************
函 数 名：	Swap_numBCD
功能说明： 	0x0100 -> 0x0256
参	  数： 	*bcd:   转换后BCD码
            num:    目标转换数字
返 回 值：	BCD码长度
*******************************************************************************/
u16_t Swap_numBCD(u8_t *bcd, u64_t num)
{
/*    u16_t i;
    u8_t *p;

    for (i = 0, p = bcd; num > 0; num /= 10)
    {
        if (i*0x01)
        {
            *p++ |= (num%10)<<4;
        }
        else
        {
            *p = num%10;
        }
        i++;
    }
    i += i&0x01;
    i >>= 1; //获取bcd码长度
    Swap_headTail(bcd, i);

    return i;*/
	u16_t i = 0;	//mike 20200810
    u8_t *p;
	u32_t nTemp_num = num;

    for (i = 0, p = bcd; nTemp_num > 0; nTemp_num /= 10)
    {
        if (i && 0x01)  // mike 20201123
        {
            *p++ |= (nTemp_num%10)<<4;
        }
        else
        {
            *p = nTemp_num%10;
        }
        i++;
    }
    i += i&0x01;
    i >>= 1; //获取bcd码长度
    Swap_headTail(bcd, i);

    return i;
}

/*******************************************************************************
函 数 名：	Swap_BCDNum
功能说明： 	0x1024 -> 0x0400
参	  数： 	*bcd:   目标转换BCD码
            len:    BCD码长度  
返 回 值：	转换后数字
*******************************************************************************/
u64_t Swap_BCDNum(u8_t *bcd, u16_t len)
{
    u64_t num;

    for (num = 0; len > 0; len--, bcd++)
    {
        num *= 100;
        num += (*bcd>>4)*10 + ((*bcd)&0x0f);
    }

    return num;
}

/*******************************************************************************
函 数 名：	Swap_IP6String
功能说明： 	0xfe80, 0, 0, 0, 0, 0x0212,0x4b00,0x0ef3  -> FE80::212:4b00:EF3
参	  数： 	*ip:    IP地址
            ipStr:  转换后字符串
返 回 值：	ipStr长度
*******************************************************************************/
u8_t Swap_IP6String(u16_t *ip, char *ipStr)
{
	u8_t i, flag;
	u16_t val;
	u8_t *pStr;
	u8_t *p;
	
	for (i = 0, p = (u8_t*)ipStr, flag = 1; i < 8; i++)
	{
		val = *ip++;
		pStr = p;
		while (val)
		{
			i = val&0x0F;
			val >>= 4;
			*p++ = i > 9 ? i + ('A' - 10) : i + '0';
			flag = 2;
		}
		Swap_headTail(pStr, (u16_t)(p - pStr));
		if (flag > 0)
		{
			*p++ = ':';
			flag--;
		}
	}

	*(--p) = '\0';

	return ((u8_t)(p - (u8_t*)ipStr));
}

/*******************************************************************************
函 数 名：	Swap_StringIP6
功能说明： 	FE80::EF3
参	  数： 	ipStr:  目标字符串
            ip:		转换后的IP
返 回 值：	无
*******************************************************************************/
void Swap_StringIP6(char *ipStr, u16_t *ip)
{
	
	ListHandler_t SubStr;
	u8_t i = r_strsplit(&SubStr, ipStr, ':');

	i  = 8 - i;
	r_memset(ip, 0, 2*8);
	
	if (SubStr.node != null)
	{
		char *p;
		Node_t *node = SubStr.node;

		do
		{
			p = (char*)*(mcu_t*)node->payload;
			node = node->next;
			*ip = Swap_HexCharNum(p);
			if (*ip == 0)
			{
				ip += i;
				i = 1;
			}
			else
			{
				ip++;
			}
		} while(node !=  SubStr.node);
	}

	list_delete(&SubStr);
}


/*******************************************************************************
  * @brief  字符串分割成字符串数组
  * @note   None
  * @param  subStr:  子字符串头
  			string:  母亲字符串 
            ch;      目标比较字符
  * @retval 子字符串个数
*******************************************************************************/
int r_strsplit(ListHandler_t *subStr , char *str, char ch)
{
	char *p = str;
	int    i = 0;

	list_init(subStr);

	while (1)
	{
		if (*str == ch || *str == '\0')
		{
			void *payload = list_nodeApply(sizeof(p));
              
			if (payload == null)
			{
				return -1;
			}
			list_bottomInsert(subStr, payload);
			i++;
            *(int*)payload = (int)p; 
			if (*str == '\0')
			{
				break;
			}
			*str++ = '\0';
			p = str;
		}
        else
        {
            str++;
        }
	}
    
    return i;
}

void hextostr(void *hexbuff, void * strtarget, u32_t len) {  //十六进制转字符串 
  u32_t i = 0;
  char *hexbuff_p = (char *)hexbuff;
  char *strtarget_p = (char *)strtarget;

  if(!hexbuff || !strtarget) {
//  log_a("hextostr address default!");
  }
  if(!len) {
//  log_a("data len error");
  }
  
  for(i = 0; i < len; i++) {
    strtarget_p[2*i]     = "0123456789ABCDEF"[(hexbuff_p[i] >> 4) & 0x0f];
    strtarget_p[2*i + 1] = "0123456789ABCDEF"[(hexbuff_p[i]) & 0x0f];
  }
}

/*******************************Copyright QYKJ*********************************/

