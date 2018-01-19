/*
******************************************************************************
*@文件名字  : BspTim.h
*@描述	    : Tim相关底层驱动，裸机和RTOS都支持
******************************************************************************
*@作者		: 丁志铖
*@强制性规定: 移植时只可以在 USER CODE BEGIN  USER CODE END  之间删改代码!!!
*@版本		: V1.00      2016/3/28
*           V1.01      2016/12/1 改成支持新架构的程序，删除“BspTypeDef.h”
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


/*************************以上代码移植过程中需要修改********************************/
/*------------------------分割线---------------------------------------------------*/
/*************************以下代码移植过程中不需要修改******************************/



/*************************对外接口变量********************************************/

extern BspTimFuncStruct g_BspTimCallBackFunc;

/*************************对外接口函数********************************************/

extern void BspTimInit(void);	
extern void BspTimPwmSetCompareValue(TIM_HandleTypeDef* Tim,u32 Channel,u32 Value);



#ifdef __cplusplus
}
#endif


#endif

/*********END OF FILE****/


