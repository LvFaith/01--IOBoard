/*
******************************************************************************
*@�ļ�����  : CMSIS_OS_SUPPLEMENT.c
*@����	    : ���CMSIS_OS��װ�Ĳ����ƣ�����һ�����Ƶ��û��Զ����OS��װ
*             ���ļ���ֲ�����в���Ҫ����
******************************************************************************
*@����		: ��־��
*@�汾		: V1.00      2016/2/18 (Ŀǰֻ���freertos)
******************************************************************************
*/

#include "CMSIS_OS_SUPPLEMENT.h"


/******************************************************************************
* @�� �� ���� osUserSemaphoreCreate
* @���������� �����ź�����������ֵ�ź����ͼ�����(��Դ��)�ź���
* @��    ���� semaphore_def��count:����osSemaphoreCreate�÷���Value:�ź����ĳ�ʼֵ <= Count
* @�� �� ֵ�� Semaphore Id
* @��    ע�� �ú�����osSemaphoreCreate�ĳ����������osSemaphoreCreate�����ź�����ʼֵ�޷��Զ��������
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


