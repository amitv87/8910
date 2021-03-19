#ifndef _MUTIL_APP_COMMON_H_
#define _MUTIL_APP_COMMON_H_


typedef struct
{
	INT32(*sys_thread_create)(void *pvTaskCode, INT8 *pcName, UINT32 usStackDepth, void *pvParameters, UINT32 uxPriority);
	void (*sys_thread_delete)();
    void *(*sys_malloc)(UINT32 size);
    INT32 (*sys_free)(void *buffer);
	void (*sys_taskSleep)(UINT32 msec);
	INT32 (*sys_test_201)();
} APP_SERRICES_T;


typedef enum
{
	GAPP_SIG_TEST1 = 0,
	GAPP_SIG_TEST2 = 1,
}THIRD_APP_SIGNAL_ID_T;

typedef void (*third_app_signal_t)(THIRD_APP_SIGNAL_ID_T sig, va_list arg);


struct THIRD_APP_RECV_SIG_CALLBACK_S
{
	third_app_signal_t fibo_signal;
};


typedef struct THIRD_APP_RECV_SIG_CALLBACK_S THIRD_APP_RECV_SIG_CALLBACK_T;



#endif 

