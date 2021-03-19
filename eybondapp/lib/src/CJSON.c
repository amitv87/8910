/**************Copyright(C)，2015-2026，QY Temp, Ltd.***************
  *@brief   : JSON 创建/解析
  *@notes   : 2017.02.27 CGQ 创建   
*******************************************************************************/
#include "CJSON.h"
#include "stdlib/r_stdlib.h"
#include "swap/swap.h"

static int CJSON_member(ListHandler_t *JSONMember, char *jsonString);
static u8_t CJSON_nameCmp(void *dest, void *itemName);
static int char_creat(char *str, void *dest, int size);
static int char_parse(char *str, void *dest, int size);
static int signed_creat(char *str, void *dest, int size);
static int signed_parse(char *str, void *dest, int size);
static int unsigned_creat(char *str, void *dest, int size);
static int unsigned_parse(char *str, void *dest, int size);


static JSONTrans creatTab[] = {
	char_creat,
	signed_creat,
	unsigned_creat
};

static JSONTrans parseTab[] = {
	char_parse,
	signed_parse,
	unsigned_parse
};

/*******************************************************************************
  * @brief  字符创建
  * @note   None
  * @param  
  * @retval 转换后字符串长度
*******************************************************************************/
static int char_creat(char *str, void *dest, int size)
{
	return r_strcpy(str, dest);
}

/*******************************************************************************
  * @brief  字符转换
  * @note   None
  * @param  
  * @retval 返回json字符串长度
*******************************************************************************/
static int char_parse(char *str, void *dest, int size)
{
	return r_strcpy(dest, str);
}


/*******************************************************************************
  * @brief  有符号数创建
  * @note   None
  * @param  
  * @retval 转换后字符串长度
*******************************************************************************/
static int signed_creat(char *str, void *dest, int size)
{
	int val;
	u8_t *p = (u8_t*)dest;
	
	val = (0x80 == (p[size-1]&0x80)) ? -1 : 0;
	r_memcpy((void*)&val, dest, size);
	return Swap_signedString(str, val);
}

/*******************************************************************************
  * @brief  有符号数转换
  * @note   None
  * @param  
  * @retval 返回json字符串长度
*******************************************************************************/
static int signed_parse(char *str, void *dest, int size)
{
	int val;
	
	val = Swap_stringSigned(str);
	r_memcpy(dest, &val, size);

	return val;
}

/*******************************************************************************
  * @brief  无符号数创建
  * @note   None
  * @param  
  * @retval 转换后字符串长度
*******************************************************************************/
static int unsigned_creat(char *str, void *dest, int size)
{
	mcu_t val = 0;
	
	r_memcpy(&val, dest, size);
	return Swap_numChar(str, val);
}

/*******************************************************************************
  * @brief  无符号数转换
  * @note   None
  * @param  
  * @retval 返回json字符串长度
*******************************************************************************/
static int unsigned_parse(char *str, void *dest, int size)
{
	mcu_t val;
	
	val = Swap_charNum(str);
	r_memcpy(dest, &val, size);

	return val;
}

/*******************************************************************************
  * @brief  成员结构体添加
  * @note   None
  * @param  jsonMember: 	结构体成员描述链表头
  			name:			成员名
  			offset: 		成员相对偏移地址
  			type:			成员类型
  			size:			占用空间大小 
  * @retval 返回json字符串长度
*******************************************************************************/
void CJSON_itemAdd(ListHandler_t *jsonMember, char *name, int offset, MemberType_e type, int size)
{
	int len;
	Node_t *node;
	StructMember_t *member;

	len = r_strlen(name);
	node = list_nodeApply(jsonMember, sizeof(StructMember_t)+len);

	member = (StructMember_t*)node->data;
	r_strcpy(member->name, name);
	member->offset = offset;
	member->size = size;
	member->create = creatTab[type];
	member->parse = parseTab[type];
}

/*******************************************************************************
  * @brief  创建JSON数据包
  * @note   None
  * @param  item: 	结构体项描述
  			struct: 目标结构休内容
  			jsonString: 打包后存储地址
  * @retval 返回json字符串长度
*******************************************************************************/
int CJSON_create(ListHandler_t *jsonMember, void *pStruct, char *jsonString)
{
	StructMember_t *member;
	Node_t *node = jsonMember->node;
	char *p = jsonString;
	u8_t *pData = (u8_t*)pStruct;
	
	*p++ = '{';
	
	do {
		member = (StructMember_t *)node->data;
		*p++ = '\"';
		p += r_strcpy(p, member->name);
		p += r_stradd(p, "\":\"");
		p += member->create(p, &pData[member->offset], member->size);
		p += r_stradd(p, "\",");
		node = node->next;
	}while (node != jsonMember->node);
	
	*--p = '}';
	*++p = '\0';
	
    return ((int)p - (int)jsonString);
}

/*******************************************************************************
  * @brief  创建JSON数据包
  * @note   None
  * @param  item: 	结构体项描述
  			struct: 目标结构休内容
  			jsonString: 打包后存储地址
  * @retval 返回转换结果
*******************************************************************************/
int CJSON_parse(ListHandler_t *jsonMember, void *pStruct, char *jsonString)
{
	ListHandler_t JSONItem;
	int i;

	i = CJSON_member(&JSONItem, jsonString);
	
	if (i > 0)
	{
		StructMember_t *pMember;
        CJSONMember_t  *item;
		Node_t *node;
		Node_t *memberNode = jsonMember->node;
		u8_t *data = (u8_t*)pStruct;
		
		do {
			pMember = (StructMember_t *)memberNode->data;
			node = list_find(&JSONItem, CJSON_nameCmp, pMember->name);
			if (node == null)
			{
				i = -2;
				break;
			}
			item = (CJSONMember_t*)(node->data);
			pMember->parse(item->para, &data[pMember->offset], pMember->size);
			list_nodeDelete(&JSONItem, node);
			memberNode = memberNode->next;
		}while (jsonMember->node != null);
	}
	list_delete(&JSONItem);
	
	return i;
}



/*******************************************************************************
  * @brief  获取Json成员
  * @note   None
  * @param  item: 	结构体项描述
  			struct: 目标结构休内容
  			jsonString: 打包后存储地址
  * @retval 返回转换结果 >0 结构体成员个数 0: JSON格式错误 -1 : 内存申请失败
*******************************************************************************/
static int CJSON_member(ListHandler_t *JSONMember, char *jsonString)
{
	CJSONMember_t *pMember = null;
	char *p = jsonString;
    int i = 0;

	list_init(JSONMember);

	while (*jsonString != '\0')
	{
		if (*jsonString++ == '\"')
		{
			if (*jsonString == ':')
			{
				if (pMember == null)	
				{
					Node_t *node = list_nodeApply(JSONMember, sizeof(CJSONMember_t));
					if (node == null)
					{
						return -1;
					}
		            pMember = (CJSONMember_t *)node->data;
				}
				
                *jsonString++ = '\0';
				pMember->name = p;
				p = jsonString;
			}
			else if (*jsonString == ',' || *jsonString == '}')
			{
                *jsonString++ = '\0';
				if (pMember != null && pMember->name != null)
				{
                    i++;
					pMember->para = p;
					pMember = null;
					p = jsonString;
				}
				else
				{
					return 0;
				}
			}
		}
	}
    
    return i;
}


/*******************************************************************************
  * @brief  结构体成员名比较
  * @note   None
  * @param  dest: 	目标
  			itemName: 项定义名
  * @retval 返回转换结果 >0 结构体成员个数 0: JSON格式错误 -1 : 内存申请失败
*******************************************************************************/
static u8_t CJSON_nameCmp(void *dest, void *itemName)
{
	CJSONMember_t *pMember = (CJSONMember_t *)dest;
	char *p = (char*)itemName;

	return r_strcmp(pMember->name, p);
}

/******************************************************************************/

