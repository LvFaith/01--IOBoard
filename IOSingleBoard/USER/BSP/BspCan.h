/*
******************************************************************************
*@�ļ�����  : BspSpi.h
*@����	    : Spi BSP ����룬�����RTOS��֧��
*       
******************************************************************************
*@����		: ��־��
*@ǿ���Թ涨: ��ֲʱֻ������ USER CODE BEGIN  USER CODE END  ֮��ɾ�Ĵ���!!!
*@�汾		: V1.00 2016/3/28 ��ʼ�汾��ֻ֧��DMA(STM32F405����)
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
extern CAN_HandleTypeDef hcan1;   //CAN1ͨ�����
#endif

#ifdef USE_CAN2

extern CAN_HandleTypeDef hcan2;   //CAN2ͨ�����
   
#endif

#ifdef USE_CAN3
extern CAN_HandleTypeDef hcan3;   //CAN2ͨ�����
#endif




typedef struct
{
    void (*Can1ReadData)(u8* buf, u32 size);  //�жϽ��մ�����ָ��
    void (*Can2ReadData)(u8* buf, u32 size);  //�жϽ��մ�����ָ��
    void (*Can3ReadData)(u8* buf, u32 size);  //�жϽ��մ�����ָ��
    osSemaphoreId CountSemCan1MailBoxNum;     //Can1����ո���
    osSemaphoreId CountSemCan2MailBoxNum;     //Can2������ɶ�ֵ�ź���
    osSemaphoreId CountSemCan3MailBoxNum;     //Can3������ɶ�ֵ�ź���
}BspCanProcessStruct;


extern BspCanProcessStruct g_CanProcess;

extern void BspCanInit(void);
extern u8 BspCanTxData( CAN_HandleTypeDef *hcan, u32 CanId, u8* buf, u32 size );
HAL_StatusTypeDef BspCanWriteData(CAN_HandleTypeDef *hcan, u32 StdId, u8 *buf, u8 len);

/* 
�жϵ��ô����� ����CAN_RX0_IRQHandler�жϺ����� 
���ú��return,��Ҫ����Cube���ɵ��жϴ�����
*/
extern void BspCanReceiveIrqHandler(CAN_HandleTypeDef* hcan, uint8_t FIFONumber);

/* 
�жϵ��ô����� ����CAN_TX_IRQHandler�жϺ����� 
���ú��return,��Ҫ����Cube���ɵ��жϴ�����
*/
extern void BspCanTransmitIrqHandler(CAN_HandleTypeDef* hcan);

#ifdef __cplusplus
  }
#endif
  
  
#endif

/*********END OF FILE****/


