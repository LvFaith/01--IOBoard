/*
******************************************************************************
*@文件名字  : BspSpi.h
*@描述	    : Spi BSP 层代码，裸机和RTOS都支持
*       
******************************************************************************
*@作者		: 丁志铖
*@强制性规定: 移植时只可以在 USER CODE BEGIN  USER CODE END  之间删改代码!!!
*@版本		: V1.00 2016/3/28 初始版本，只支持DMA(STM32F405测试)
******************************************************************************
*/

#ifndef _BSP_CAN_H
#define _BSP_CAN_H


#ifdef __cplusplus
 extern "C" {
#endif 


#include "Config.h"
#include "stm32f4xx_hal_can.h"

   
typedef struct
{
  u32 StdId;
  u32 ExtId;
  u8 DID;
  u8 IDE;
  u8 RTR;
  u8 DLC;
  u8 Buf[8];
  u8 SendBitsDataPre;
  u8 SendBitsDataLater;
} CanTxMsg;
/* USER CODE BEGIN PV */
#define USE_CAN1
//#define USE_CAN2
/* USER CODE END PV */

#ifdef USE_CAN1
extern CAN_HandleTypeDef hcan1;   //CAN1通道句柄
#endif

#ifdef USE_CAN2

extern CAN_HandleTypeDef hcan2;   //CAN2通道句柄
   
#endif

#ifdef USE_CAN3
extern CAN_HandleTypeDef hcan3;   //CAN2通道句柄
#endif




typedef struct
{
    void (*Can1ReadData)(u8* buf, u32 size);  //中断接收处理函数指针
    void (*Can2ReadData)(u8* buf, u32 size);  //中断接收处理函数指针
    void (*Can3ReadData)(u8* buf, u32 size);  //中断接收处理函数指针
    osSemaphoreId CountSemCan1MailBoxNum;     //Can1邮箱空个数
    osSemaphoreId CountSemCan2MailBoxNum;     //Can2发送完成二值信号量
    osSemaphoreId CountSemCan3MailBoxNum;     //Can3发送完成二值信号量
}BspCanProcessStruct;


extern BspCanProcessStruct g_CanProcess;

extern void BspCanInit(void);
extern u8 BspCanTxData( CAN_HandleTypeDef *hcan, u32 CanId, u8* buf, u32 size );
HAL_StatusTypeDef BspCanWriteData(CAN_HandleTypeDef *hcan, u32 StdId, u8 *buf, u8 len);

/* 
中断调用处理函数 放在CAN_RX0_IRQHandler中断函数中 
调用后加return,不要调用Cube生成的中断处理函数
*/
extern void BspCanReceiveIrqHandler(CAN_HandleTypeDef* hcan, uint8_t FIFONumber);

/* 
中断调用处理函数 放在CAN_TX_IRQHandler中断函数中 
调用后加return,不要调用Cube生成的中断处理函数
*/
extern void BspCanTransmitIrqHandler(CAN_HandleTypeDef* hcan);

#ifdef __cplusplus
  }
#endif
  
  
#endif

/*********END OF FILE****/


