/*
******************************************************************************
*@�ļ�����  : BspTim.c
*@����	    : Tim��صײ������������RTOS��֧��
******************************************************************************
*@����		: ��־��
*@ǿ���Թ涨: ��ֲʱֻ������ USER CODE BEGIN  USER CODE END  ֮��ɾ�Ĵ���!!!
*@�汾		: V1.00      2016/3/28
*           V1.01      2016/12/1 �ĳ�֧���¼ܹ��ĳ���ɾ����BspTypeDef.h��
******************************************************************************
*/


#include "BspTim.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

BspTimFuncStruct g_BspTimCallBackFunc;


/******************************************************************************
* @�� �� ���� BspTimInit
* @���������� ��ʼ��Tim
* @��    ���� 
* @�� �� ֵ�� 
* @��    ע�� 
******************************************************************************/

void BspTimInit(void)
{
  /* USER CODE BEGIN 0*/
  memset( &g_BspTimCallBackFunc,0,sizeof(g_BspTimCallBackFunc) );

  //1KHz Start
  __HAL_TIM_CLEAR_IT(&htim6, TIM_IT_UPDATE);
  HAL_TIM_Base_Start_IT(&htim6);

	//400Hz Start
  __HAL_TIM_CLEAR_IT(&htim14, TIM_IT_UPDATE);
  HAL_TIM_Base_Start_IT(&htim14);
  
  	//1MHz Start
  __HAL_TIM_CLEAR_IT(&htim10, TIM_IT_UPDATE);
  HAL_TIM_Base_Start_IT(&htim10);
  /* USER CODE END   0*/
}


/******************************************************************************
* @�� �� ���� BspTimPwmSetCompareValue
* @���������� ���ö�ʱ��PWM�Ƚ�ֵ
* @��    ���� Tim: &htimX(X=1,2,3...) Channel:TIM_CHANNEL_X(X=1,2,3,4) Value:�µ�����ֵ
* @�� �� ֵ�� ��
* @��    ע�� 
******************************************************************************/

void BspTimPwmSetCompareValue(TIM_HandleTypeDef* Tim,u32 Channel,u32 Value)
{
	switch(Channel)
	{
		case TIM_CHANNEL_1:
			Tim->Instance->CCR1 = Value;
			break;
			
		case TIM_CHANNEL_2:
			Tim->Instance->CCR2 = Value;
			break;

		case TIM_CHANNEL_3:
			Tim->Instance->CCR3 = Value;
			break;

		case TIM_CHANNEL_4:
			Tim->Instance->CCR4 = Value;
			break;

		default:
			break;
	}
}


/*********END OF FILE****/


