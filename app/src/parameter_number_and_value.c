#include "parameter_number_and_value.h"
#include "fibo_opencpu.h"
#include "data_collector_parameter_table.h"
#include "md5_test.h"
#include "elog.h"
#include "struct_type.h"
#include "hextostr.h"
#include "stdio.h"
/*----------------------------------------------------------------------------*
**                             Global Vars                                    *
**----------------------------------------------------------------------------*/
extern int number_of_array_elements;

/*----------------------------------------------------------------------------*
**                             Local Vars                                     *
**----------------------------------------------------------------------------*/
#define g_recName_parameter_a  "/parameter_configuration_file_a.ini"    //文件名
INT32                       g_iFd_parameter_a       = 0;                                   //文件描述符
static UINT8                *parameter_a_md5        = NULL;
static UINT8                *parameter_a_md5_s      = NULL;
static INT32                a_md5_verify_len        = 0;
static char                 *parameter_a_value      = NULL;             //参数值
static char                 *parameter_a_value_buf  = NULL;
static UINT16               parameter_a_len         = 64;  
#define g_recName_parameter_b   "/parameter_configuration_file_b.ini"    //文件名
INT32                        g_iFd_parameter_b      = 0;                                   //文件描述符
static UINT8                *parameter_b_md5        = NULL;
static UINT8                *parameter_b_md5_s      = NULL;
static char                 *parameter_b_value_buf  = NULL;

static INT32 parameter_a_MD5Verify_Func (UINT8 bHash[], UINT32 Verify_Len, UINT32 DataBlock_Len);//生成MD5值
static INT32 parameter_b_MD5Verify_Func (UINT8 bHash[], UINT32 Verify_Len, UINT32 DataBlock_Len);//生成MD5值

void parameter_a_module(void)
{
    // log_d("\r\n%s()\r\n", __func__);

    //int check = cm_file_check(g_recName_parameter_a);
    // log_d("\r\ncheck is %d\r\n",check);
    //cm_file_check(g_recName_parameter_a);
    
    //int delete = cm_file_delete(g_recName_parameter_a);
    // log_d("\r\ndelete is %d\r\n",delete);
    fibo_file_delete(g_recName_parameter_a);

    g_iFd_parameter_a = fibo_file_open(g_recName_parameter_a, FS_O_WRONLY|FS_O_CREAT|FS_O_APPEND);
    
    a_md5_verify_len = 0;

    if (g_iFd_parameter_a < 0)
    {
        log_d("\r\nCreate parameter_configuration_file_a failed\r\n");
    }

    // int8_t a_number = 0;
    UINT8   a_number_char[4]={0};       
    for (int j = 0; j < number_of_array_elements; j++)   
    {
        // a_number = PDT[j].num;
        // if(a_number<100)
        // {
        //    a_number_char[0] = '0';
        //    if(a_number<10)
        //    {
        //       a_number_char[1]='0'; 
        //       cm_itoa(a_number,&a_number_char[2],10);  
        //    }
        //    else
        //    {
        //        cm_itoa(a_number,&a_number_char[1],10);  
        //    }
        // }
        // else
        // {
        //   cm_itoa(a_number,a_number_char,10);   
        // }

        sprintf((char *)a_number_char,"%03d",PDT[j].num);

        parameter_a_value_buf = fibo_malloc(sizeof(char)*64);
        memset(parameter_a_value_buf, 0, sizeof(char)*64); 

        PDT[j].rFunc(&PDT[j],parameter_a_value_buf, &parameter_a_len);
        parameter_a_value = fibo_malloc(sizeof(char)*parameter_a_len);
        memset(parameter_a_value, 0, parameter_a_len); 

        // if(0 == a_number)
        // {
        //     *parameter_a_value_buf = (*parameter_a_value_buf)++;
        //     log_d("\r\nparameter_a_value_buf is %c\r\n",*parameter_a_value_buf);
        // } 

        memcpy(parameter_a_value,parameter_a_value_buf,parameter_a_len);

        fibo_file_write(g_iFd_parameter_a, (UINT8 *)"para", 4);     
        fibo_file_write(g_iFd_parameter_a, a_number_char, 3);   //写参数编号到文件
        fibo_file_write(g_iFd_parameter_a, (UINT8 *)"=", 1);                                           //等号
        fibo_file_write(g_iFd_parameter_a, (UINT8 *)parameter_a_value, parameter_a_len);               //写参数值到文件
     
        fibo_file_write(g_iFd_parameter_a, (UINT8 *)"\0\r\n", 3);                                        //回车换行
       
        a_md5_verify_len= a_md5_verify_len + 4 + 3 + 1 + parameter_a_len + 3;
        
        fibo_free(parameter_a_value);
        fibo_free(parameter_a_value_buf);

        fibo_file_fsync(g_iFd_parameter_a);

    }
    fibo_file_close(g_iFd_parameter_a);

    parameter_a_md5     = fibo_malloc(sizeof(UINT8)*32);                                       //MD5值  
    parameter_a_md5_s   = fibo_malloc(sizeof(UINT8)*32);                                       //MD5值  
    memset(parameter_a_md5,   0, sizeof(UINT8)*32); 
    memset(parameter_a_md5_s, 0, sizeof(UINT8)*32); 
    parameter_a_MD5Verify_Func(parameter_a_md5,a_md5_verify_len,512);
    hextostr(parameter_a_md5, parameter_a_md5_s, 16);
    // log_d("\r\nparameter_a_md5_s %s\r\n",parameter_a_md5_s);
    g_iFd_parameter_a = fibo_file_open(g_recName_parameter_a, FS_O_WRONLY|FS_O_APPEND);
    fibo_file_write(g_iFd_parameter_a, parameter_a_md5_s, 32);                                //写内容到文件
    fibo_file_close(g_iFd_parameter_a);
    fibo_free(parameter_a_md5);
    fibo_free(parameter_a_md5_s);
    log_d("\r\nparameter_a_module ok\r\n");
}

static INT32 parameter_a_MD5Verify_Func (UINT8 bHash[], UINT32 Verify_Len, UINT32 DataBlock_Len)//生成MD5值
{
    HASH_MD5_CTX  MD5;
    HASH_MD5Init (&MD5);
    static UINT8 s_pTempMd5[512] = {0};
    UINT32       dwOff           = 0;
    memset(s_pTempMd5, 0, sizeof(s_pTempMd5));
    log_d("\r\nVerify_Len is %d",Verify_Len);
    while (dwOff < Verify_Len)
    {
        fibo_file_open(g_recName_parameter_a, FS_O_RDONLY);
        fibo_file_seek(g_iFd_parameter_a, dwOff, 0);
        fibo_file_read(g_iFd_parameter_a, s_pTempMd5, DataBlock_Len);
        // log_d("\r\n%s()read:\r\n%s\r\n",__func__,s_pTempMd5);
        if ((Verify_Len - dwOff) < DataBlock_Len) 
        {
            DataBlock_Len = Verify_Len - dwOff;
        }
        HASH_MD5Update(&MD5, s_pTempMd5, DataBlock_Len);
        dwOff += DataBlock_Len;
        fibo_file_close(g_iFd_parameter_a);
    }
    HASH_MD5Final (bHash, &MD5);
    return 0;
}

static INT32 parameter_b_MD5Verify_Func (UINT8 bHash[], UINT32 Verify_Len, UINT32 DataBlock_Len)//生成MD5值
{
    HASH_MD5_CTX  MD5;
    HASH_MD5Init (&MD5);
    static UINT8 s_pTempMd5[512] = {0};
    UINT32       dwOff           = 0;
    memset(s_pTempMd5, 0, sizeof(s_pTempMd5));
    log_d("\r\nVerify_Len is %d\r\n",Verify_Len);
    while (dwOff < Verify_Len)
    {
        fibo_file_open(g_recName_parameter_b, FS_O_RDONLY);
        fibo_file_seek(g_iFd_parameter_b, dwOff, 0);
        fibo_file_read(g_iFd_parameter_b, s_pTempMd5, DataBlock_Len);
        // log_d("\r\n%s()read:\r\n%s\r\n",__func__,s_pTempMd5);
        if ((Verify_Len - dwOff) < DataBlock_Len) 
        {
            DataBlock_Len = Verify_Len - dwOff;
        }
        HASH_MD5Update(&MD5, s_pTempMd5, DataBlock_Len);
        dwOff += DataBlock_Len;
        fibo_file_close(g_iFd_parameter_b);
    }
    HASH_MD5Final (bHash, &MD5);
    return 0;
}

void parameter_init(void)
{
    char    n_buf[sizeof(char)*3];
    int8_t  number              = 0;
    char    number_char[4]      ={0};   
    char    *number_char_buff   = n_buf;
    char    *parameter_value    = NULL;        //参数值
    UINT16  len                 = 64;          //长度
    char    temporary_char      = 0;

    UINT32  dwOff               = 0;            //偏移量
    UINT32  equalPlace          = 0;            //等号
    UINT32  newlinePlace        = 0;            //换行符
    g_iFd_parameter_a           = fibo_file_open(g_recName_parameter_a, FS_O_RDONLY);
    int32_t file_a_size         = fibo_file_getSize(g_recName_parameter_a); 
    fibo_file_close(g_iFd_parameter_a);


    while(dwOff<(file_a_size-32))
    {   
        g_iFd_parameter_a = fibo_file_open(g_recName_parameter_a, FS_O_RDONLY);
        fibo_file_seek(g_iFd_parameter_a, dwOff, 0);
        fibo_file_read(g_iFd_parameter_a, (UINT8 *)&temporary_char, 1);
        fibo_file_close(g_iFd_parameter_a);

        if('=' == temporary_char)
        {
            equalPlace = dwOff;
            fibo_file_open(g_recName_parameter_a, FS_O_RDONLY);
            fibo_file_seek(g_iFd_parameter_a, equalPlace-3, 0);
    
            memset(number_char_buff,0,sizeof(char)*3);
            fibo_file_read(g_iFd_parameter_a, (UINT8 *)number_char_buff, 3);
            
            fibo_file_close(g_iFd_parameter_a);
            memcpy(number_char,number_char_buff,3);
           
            number = atoi(number_char);
            memset(number_char,0,4);  
        }

        if('\r' == temporary_char)
        {
            newlinePlace = dwOff;
            fibo_file_open(g_recName_parameter_a, FS_O_RDONLY);
            fibo_file_seek(g_iFd_parameter_a, equalPlace+1, 0);
            if(parameter_value != NULL)
            {
                fibo_free(parameter_value);
                parameter_value = NULL;
            }
            parameter_value=fibo_malloc(sizeof(char)*(newlinePlace-equalPlace-1));
            memset(parameter_value,0,sizeof(char)*(newlinePlace-equalPlace-1));
            fibo_file_read(g_iFd_parameter_a, (UINT8 *)parameter_value, newlinePlace-equalPlace-1);
            fibo_file_close(g_iFd_parameter_a); 
        }

        if(('\n' == temporary_char))
        {
            for (int j = 0; j < number_of_array_elements; j++)
            {
                if(number == PDT[j].num)
                {
                    memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
                    PDT[j].wFunc(&PDT[j],parameter_value, &len);
                    fibo_free(parameter_value);
                    parameter_value = NULL;
                    break;
                } 
            } 
        }
        dwOff++;
    }
    if(parameter_value != NULL)
    {
        fibo_free(parameter_value);
        parameter_value = NULL;
    }

    log_d("\r\n%s() ok\r\n", __func__);
}

void live_a_and_b(void)
{
    INT32  live_a = fibo_file_exist(g_recName_parameter_a);
    if(1 == live_a)
    {
        log_d("\r\npara_file_a is exist\r\n");
    }

	INT32  live_b = fibo_file_exist(g_recName_parameter_b);
    if(1 == live_b)
    {
        log_d("\r\npara_file_b is exist\r\n");
    }
	
    g_iFd_parameter_a = fibo_file_open(g_recName_parameter_a, FS_O_RDONLY);
    int32_t file_a_size =  fibo_file_getSize(g_recName_parameter_a);
    fibo_file_close(g_iFd_parameter_a);
    log_d("\r\nfile_a_size is %d\r\n",file_a_size);

    g_iFd_parameter_b = fibo_file_open(g_recName_parameter_b, FS_O_RDONLY);
    int32_t file_b_size =  fibo_file_getSize(g_recName_parameter_b);
    fibo_file_close(g_iFd_parameter_b);
    log_d("\r\nfile_b_size is %d\r\n",file_b_size);

    if((1 == live_a)&&(1 == live_b))   //这个文件都存在
    {
        if((file_a_size > 32)&&(file_b_size > 32))
        {
                log_d("\r\na b is live\r\n");
                a_compare_b();
                parameter_init();
        }

        if((!(file_a_size > 32))&&(!(file_b_size > 32)))
        {
            parameter_a_module();
            a_copy_to_b();
            parameter_init();//保持统一
        }

        if((file_a_size > 32)&&(!(file_b_size > 32)))
        {
            a_copy_to_b();
            parameter_init();
        }

        if((!(file_a_size > 32))&&(file_b_size > 32))
        {
           b_copy_to_a();  
           parameter_init();
        } 
    }

    if((live_a<0)&&(live_b<0)) //两个文件都不存在
    {
        // log_d("\r\na b is die\r\n");
        parameter_a_module();
        a_copy_to_b();
        parameter_init();//保持统一
    }

    if((1 == live_a)&&(live_b<0)) //a存在b不存在
    {
        // log_d("\r\na is live b is die\r\n");  
        a_copy_to_b();
        parameter_init();
    }

    if((live_a<0)&&(1 == live_b))//b存在a不存在
    {
        // log_d("\r\nb is live a is die\r\n");
        b_copy_to_a();
        parameter_init();
    } 
}

void a_compare_b(void)
{
    log_d("\r\n%s()\r\n", __func__);

    g_iFd_parameter_a = fibo_file_open(g_recName_parameter_a, FS_O_RDONLY);
    int32_t file_a_size =  fibo_file_getSize(g_recName_parameter_a); 
    fibo_file_close(g_iFd_parameter_a);
    log_d("\r\nfile_a_size is %d", file_a_size);

    parameter_a_md5     = fibo_malloc(sizeof(UINT8)*32);                                       //MD5值  
    parameter_a_md5_s   = fibo_malloc(sizeof(UINT8)*32);                                       //MD5值  
    memset(parameter_a_md5,   0, sizeof(char)*32); 
    memset(parameter_a_md5_s, 0, sizeof(char)*32); 

    parameter_a_MD5Verify_Func(parameter_a_md5,file_a_size-32,512);
    hextostr(parameter_a_md5, parameter_a_md5_s, 16);
    log_d("\r\nparameter_a_md5_s：%s\r\n",parameter_a_md5_s);
    fibo_free(parameter_a_md5);

    g_iFd_parameter_b = fibo_file_open(g_recName_parameter_b, FS_O_RDONLY);
    int32_t file_b_size =  fibo_file_getSize(g_recName_parameter_b); 
    fibo_file_close(g_iFd_parameter_b); 
    log_d("\r\nfile_b_size is %d\r\n",file_b_size);

    parameter_b_md5      = fibo_malloc(sizeof(UINT8)*32);    
    parameter_b_md5_s    = fibo_malloc(sizeof(UINT8)*32);
    memset(parameter_b_md5,   0, sizeof(char)*32); 
    memset(parameter_b_md5_s, 0, sizeof(char)*32);     
                                    
    parameter_b_MD5Verify_Func(parameter_b_md5,file_b_size-32,512);
    hextostr(parameter_b_md5, parameter_b_md5_s, 16);
    log_d("\r\nparameter_b_md5_s：%s\r\n",parameter_b_md5_s);
    fibo_free(parameter_b_md5);

    int compute_compare_result = 0;
    compute_compare_result = strncmp((char *)parameter_a_md5_s,(char *)parameter_b_md5_s,32);

    if(0 == compute_compare_result)
    {
        log_d("\r\ncompute value para_a_md5 = compute value para_b_md5\r\n");
    }
    else
    {
        log_d("\r\ncompute value para_a_md5 != compute value para_b_md5\r\n");

        UINT8 *para_a_md5 = fibo_malloc(sizeof(char)*32);
        memset(para_a_md5, 0, sizeof(char)*32); 

        g_iFd_parameter_a = fibo_file_open(g_recName_parameter_a, FS_O_RDONLY);
        file_a_size =  fibo_file_getSize(g_recName_parameter_a); 
        fibo_file_seek(g_iFd_parameter_a, file_a_size-32, 0);
        fibo_file_read(g_iFd_parameter_a, para_a_md5, 32);
        fibo_file_close(g_iFd_parameter_a);

        int compute_compare_result_a = 0;
        compute_compare_result_a = strncmp((char *)parameter_a_md5_s,(char *)para_a_md5,32);

        UINT8 *para_b_md5 = fibo_malloc(sizeof(char)*32);
        memset(para_b_md5, 0, sizeof(char)*32);

        g_iFd_parameter_b = fibo_file_open(g_recName_parameter_b, FS_O_RDONLY);
        file_b_size =  fibo_file_getSize(g_recName_parameter_b); 
        fibo_file_seek(g_iFd_parameter_b, file_b_size-32, 0);
        fibo_file_read(g_iFd_parameter_b, para_b_md5, 32);
        fibo_file_close(g_iFd_parameter_b);

        int compute_compare_result_b = 0;
        compute_compare_result_b = strncmp((char *)parameter_b_md5_s,(char *)para_b_md5,32);

        if(0 == compute_compare_result_a)
        {
            log_d("\r\ncompute value para_a_md5 = Existing value para_a_md5\r\n");
        }
        else
        {
            log_d("\r\ncompute value para_a_md5 != Existing value para_a_md5\r\n");
            //文件拷贝 b->a
            b_copy_to_a();
        }

        if(0 == compute_compare_result_b)
        {
            log_d("\r\ncompute value para_b_md5 = Existing value para_b_md5\r\n");
        }
        else
        {
            log_d("\r\ncompute value para_b_md5 != Existing value para_b_md5\r\n");
            //文件拷贝 a->b
            a_copy_to_b();
        }

        fibo_free(parameter_a_md5_s);
        fibo_free(parameter_b_md5_s);
        fibo_free(para_a_md5);
        fibo_free(para_b_md5);
        //最后一种可能
    }
}

void a_copy_to_b(void)
{
    // log_d("\r\n%s()\r\n", __func__);

    //int check=cm_file_check(g_recName_parameter_b);
    // log_d("\r\ncheck is %d\r\n",check);
    // cm_file_check(g_recName_parameter_b);

    //int delete=cm_file_delete(g_recName_parameter_b);
    // log_d("\r\ndelete is %d\r\n",delete);
    fibo_file_delete(g_recName_parameter_b);

    g_iFd_parameter_b = fibo_file_open(g_recName_parameter_b,FS_O_WRONLY|FS_O_CREAT);
    g_iFd_parameter_a = fibo_file_open(g_recName_parameter_a, FS_O_RDONLY);

    int32_t file_a_size =  fibo_file_getSize(g_recName_parameter_a); 

    parameter_a_value_buf = fibo_malloc(sizeof(char)*file_a_size);

    memset(parameter_a_value_buf, 0, sizeof(char)*file_a_size); 

    fibo_file_read(g_iFd_parameter_a, (UINT8 *)parameter_a_value_buf, file_a_size);

    fibo_file_close(g_iFd_parameter_a);

    fibo_file_seek(g_iFd_parameter_b,0,0);

    fibo_file_write(g_iFd_parameter_b,(UINT8 *)parameter_a_value_buf,file_a_size);

    int32_t file_b_size =  fibo_file_getSize(g_recName_parameter_b); 

    log_d("\r\na_copy_to_b ok file_b_size is %d\r\n",file_b_size);

    fibo_file_close(g_iFd_parameter_b);

    fibo_free(parameter_a_value_buf);
}

void b_copy_to_a(void)
{
    log_d("\r\n%s()\r\n", __func__);

    int check=fibo_file_exist(g_recName_parameter_a);
    log_d("\r\ncheck is %d\r\n",check);

    int delete=fibo_file_delete(g_recName_parameter_a);
    log_d("\r\ndelete is %d\r\n",delete);

    g_iFd_parameter_a = fibo_file_open(g_recName_parameter_a,FS_O_WRONLY|FS_O_CREAT);

    g_iFd_parameter_b = fibo_file_open(g_recName_parameter_b, FS_O_RDONLY);

    int32_t file_b_size =  fibo_file_getSize(g_recName_parameter_b); 

    parameter_b_value_buf = fibo_malloc(sizeof(char)*file_b_size);

    memset(parameter_b_value_buf, 0, sizeof(char)*file_b_size);

    fibo_file_read(g_iFd_parameter_b, (UINT8 *)parameter_b_value_buf, file_b_size);

    fibo_file_close(g_iFd_parameter_b);

    fibo_file_seek(g_iFd_parameter_a,0,0);

    fibo_file_write(g_iFd_parameter_a,(UINT8 *)parameter_b_value_buf,file_b_size);

    int32_t file_a_size =  fibo_file_getSize(g_recName_parameter_a); 

    log_d("\r\nb_copy_to_a ok file_a_size is %d\r\n",file_a_size);

    fibo_file_close(g_iFd_parameter_a);
    
    fibo_free(parameter_b_value_buf);
}