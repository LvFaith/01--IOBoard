/*
******************************************************************************
*@�ļ�����  : CMSIS_OS_SUPPLEMENT.h
*@����	    : ���CMSIS_OS��װ�Ĳ����ƣ�����һ�����Ƶ��û��Զ����OS��װ
*			  ���ļ���ֲ�����в���Ҫ����
******************************************************************************
*@����		: ��־��
*@�汾		: V1.00      2016/2/18 (Ŀǰֻ���freertos)
******************************************************************************
*/

#ifndef _CMSIS_OS_SUPPLEMENT_H_
#define _CMSIS_OS_SUPPLEMENT_H_

#include "cmsis_os.h"


#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */


/**********����ӿں���************/
osSemaphoreId osUserSemaphoreCreate (const osSemaphoreDef_t *Semaphore_def, int32_t Count ,int32_t Value);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif

/*********END OF FILE****/

