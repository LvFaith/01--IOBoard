/*
******************************************************************************
*@文件名字  : CMSIS_OS_SUPPLEMENT.c
*@描述	    : 针对CMSIS_OS封装的不完善，增加一个类似的用户自定义的OS封装
*             该文件移植过程中不需要更改
******************************************************************************
*@作者		: 丁志铖
*@版本		: V1.00      2016/2/18 (目前只针对freertos)
******************************************************************************
*/

#include "CMSIS_OS_SUPPLEMENT.h"


/******************************************************************************
* @函 数 名： osUserSemaphoreCreate
* @函数描述： 创建信号量，包括二值信号量和计数型(资源型)信号量
* @参    数： semaphore_def和count:参照osSemaphoreCreate用法，Value:信号量的初始值 <= Count
* @返 回 值： Semaphore Id
* @备    注： 该函数是osSemaphoreCreate的超集，解决了osSemaphoreCreate创建信号量初始值无法自定义的问题
******************************************************************************/
osSemaphoreId osUserSemaphoreCreate (const osSemaphoreDef_t *Semaphore_def, int32_t Count ,int32_t Value)
{
  (void) Semaphore_def;
  osSemaphoreId Sema;

  if(Count < Value)
  	return NULL;
  
  if (Count == 1) {
  	vSemaphoreCreateBinary(Sema);
  	if(Value == 0) {
    	osSemaphoreWait(Sema,0);
  	}
	return Sema;
  }

#if (configUSE_COUNTING_SEMAPHORES == 1 )	
  return xSemaphoreCreateCounting(Count, Value);
#else
  return NULL;
#endif
}


/*********END OF FILE****/


