/*
******************************************************************************
*@文件名字  : CMSIS_OS_SUPPLEMENT.h
*@描述	    : 针对CMSIS_OS封装的不完善，增加一个类似的用户自定义的OS封装
*			  该文件移植过程中不需要更改
******************************************************************************
*@作者		: 丁志铖
*@版本		: V1.00      2016/2/18 (目前只针对freertos)
******************************************************************************
*/

#ifndef _CMSIS_OS_SUPPLEMENT_H_
#define _CMSIS_OS_SUPPLEMENT_H_

#include "cmsis_os.h"


#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */


/**********对外接口函数************/
osSemaphoreId osUserSemaphoreCreate (const osSemaphoreDef_t *Semaphore_def, int32_t Count ,int32_t Value);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif

/*********END OF FILE****/

