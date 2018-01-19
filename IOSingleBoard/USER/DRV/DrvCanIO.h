/*
******************************************************************************
*@�ļ�����  : DrvComESC.h
*@����	    : �͵����ͨѶ
******************************************************************************
*@����		: ۡ�˳�
*@ǿ���Թ涨: ��ֲʱֻ������ USER CODE BEGIN  USER CODE END  ֮��ɾ�Ĵ���!!!
*@�汾		: V1.00      2017/1/18
******************************************************************************
*/

#ifndef _DRV_CAN_IO_H_
#define _DRV_CAN_IO_H_

#ifdef __cplusplus
	 extern "C" {
#endif 



/* USER CODE BEGIN 0*/

#include "Config.h"
#include "CommonStream.h"
#include "BspCan.h"

     
     
#define  Strong_stid      0x2400
#define  XYmotor1_stid    0x0120
#define  XYmotor2_stid    0x0220
#define  XYmotor3_stid    0x0420
#define  XYmotor4_stid    0x0820
#define  HTmotor_stid     0x1020
#define  UpWingL_stid     0x2020
#define  UpWingR_stid     0x4020
#define  DownWingL_stid   0x8020
#define  DownWingR_stid   0x0140
#define  RudderL_stid     0x0240
#define  RudderR_stid     0x0440
#define  Elevator_stid    0x0840
#define  Master_stid      0x8080

#define  Strong_blid      0xffff
#define  XYmotor1_blid    0xffff
#define  XYmotor2_blid    0xffff
#define  XYmotor3_blid    0xffff
#define  XYmotor4_blid    0xffff
#define  HTmotor_blid     0xffff
#define  UpWingL_blid     0xffff
#define  UpWingR_blid     0xffff
#define  DownWingL_blid   0xffff
#define  DownWingR_blid   0xffff
#define  RudderL_blid     0xffff
#define  RudderR_blid     0xffff
#define  Elevator_blid    0xffff
/* USER CODE END   0*/



/*************************���ϴ�����ֲ��������Ҫ�޸�********************************/
/*------------------------�ָ���---------------------------------------------------*/
/*************************���´�����ֲ�����в���Ҫ�޸�******************************/



/*************************����ӿڱ���********************************************/

extern CommonStreamTxStruct g_TxStreamToIO;

#define DrvComESCTxTask() CommonStreamTxTask(&g_TxStreamToESC)


/*************************����ӿں���********************************************/
#define DrvComESCTx(__BUF__, __SIZE__) CommonStreamTxPutDataToQueue(&g_TxStreamToESC, __BUF__, __SIZE__)

extern void DrvComIOInit(void);	//��ʼ�� 
extern void DrvComIORxTask(void);//����������ͨ��FIFO
extern u8 DrvTxToIO(CAN_HandleTypeDef *hcan, u8* buf, u32 size );

#ifdef __cplusplus
}
#endif


#endif

/*********END OF FILE****/


