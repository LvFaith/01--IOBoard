/*
******************************************************************************
*@�ļ�����  : BspTim.h
*@����	    : Tim��صײ������������RTOS��֧��
******************************************************************************
*@����		: ��־��
*@ǿ���Թ涨: ��ֲʱֻ������ USER CODE BEGIN  USER CODE END  ֮��ɾ�Ĵ���!!!
*@�汾		: V1.00      2016/3/28
*           V1.01      2016/12/1 �ĳ�֧���¼ܹ��ĳ���ɾ����BspTypeDef.h��
******************************************************************************
*/

#ifndef _BSP_TIM_H_
#define _BSP_TIM_H_


#ifdef __cplusplus
 extern "C" {
#endif 


#include "config.h"
#include "tim.h"


typedef struct
{
  /* USER CODE BEGIN 0*/
    
	//void (*Tim1)(void);
	//void (*Tim2)(void);
	//void (*Tim3)(void);
	//void (*Tim4)(void);
	//void (*Tim5)(void);
	void (*Tim6)(void);
	//void (*Tim7)(void);
	//void (*Tim8)(void);
  //void (*Tim9)(void);
	void (*Tim10)(void);
	//void (*Tim11)(void);
	//void (*Tim12)(void);
	//void (*Tim13)(void);
	void (*Tim14)(void);
	/* USER CODE END   0*/
}BspTimFuncStruct; 


/*************************���ϴ�����ֲ��������Ҫ�޸�********************************/
/*------------------------�ָ���---------------------------------------------------*/
/*************************���´�����ֲ�����в���Ҫ�޸�******************************/



/*************************����ӿڱ���********************************************/

extern BspTimFuncStruct g_BspTimCallBackFunc;

/*************************����ӿں���********************************************/

extern void BspTimInit(void);	
extern void BspTimPwmSetCompareValue(TIM_HandleTypeDef* Tim,u32 Channel,u32 Value);



#ifdef __cplusplus
}
#endif


#endif

/*********END OF FILE****/


