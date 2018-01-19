/*
******************************************************************************
*@文件名字  : BspTim.c
*@描述	    : Tim相关底层驱动，裸机和RTOS都支持
******************************************************************************
*@作者		: 丁志铖
*@强制性规定: 移植时只可以在 USER CODE BEGIN  USER CODE END  之间删改代码!!!
*@版本		: V1.00      2016/3/28
*           V1.01      2016/12/1 改成支持新架构的程序，删除“BspTypeDef.h”
******************************************************************************
*/


#include "BspTim.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

BspTimFuncStruct g_BspTimCallBackFunc;


/******************************************************************************
* @函 数 名： BspTimInit
* @函数描述： 初始化Tim
* @参    数： 
* @返 回 值： 
* @备    注： 
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
* @函 数 名： BspTimPwmSetCompareValue
* @函数描述： 设置定时器PWM比较值
* @参    数： Tim: &htimX(X=1,2,3...) Channel:TIM_CHANNEL_X(X=1,2,3,4) Value:新的设置值
* @返 回 值： 无
* @备    注： 
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


