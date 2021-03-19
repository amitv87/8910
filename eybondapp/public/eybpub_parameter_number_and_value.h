/********************** COPYRIGHT 2014-2100, eybond ***************************
 * @File    : eybpub_parameter_number_and_value.h
 * @Author  : 
 * @Date    : 2020-08-20
 * @Brief   :
 ******************************************************************************/
#ifndef __EYBPUB_PARAMETER_NUMBER_AND_VALUE_H__
#define __EYBPUB_PARAMETER_NUMBER_AND_VALUE_H__

#define g_backup_parameter_c   "/paraconfig_file_c.ini"  //生产参数备份文件名


void parameter_a_module(void);
void parameter_init(void);
//void live_a_and_b(void);
int  live_a_and_b(void);
void a_compare_b(void);
void a_copy_to_b(void);
void b_copy_to_a(void);
void a_copy_to_c(void);
void rm_file_A (void);

void para_init(void);

#endif