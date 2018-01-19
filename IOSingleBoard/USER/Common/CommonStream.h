/**
* @file     CommonStream.h
* @brief    
* @version  V0.01
* @author   zhicheng-ding@zerotech.com
* @date     2016-9-10
* @note     ͨ�������������
*/

#ifndef _COMMON_STREAM_H_
#define _COMMON_STREAM_H_

#ifdef __cplusplus
 extern "C" {
#endif 
 
/*============================ INCLUDES ======================================*/
 
#include "Config.h"


/*============================ Rx Stream =========================================*/

//������������������Э��ֻ�ܳ�����������״̬

typedef enum
{
	RX_FRAME_OK,
	RX_FRAME_ERR,
	RX_FRAME_LESS,
}FrameStatusEnum;
 
typedef struct StreamRxFifo
{
  s8 *instance;						//����
  u8 *FifoBuf;						//FifoBuf:����fifo
	u8 *FrameBuf;				
  u16 MaxFifoLen;					//fifo����󳤶�
	u16 MaxFrameLen;							
  u16 CurFifoLen;					//CurFifoLen:��ǰfifo�ĳ���
	u16 CurFrameLen;				//��ǰ�����ɹ�������֡����		
  u16 ReadPos;						//��ָ��
	u16 WritePos;						//дָ��
  u16 ErrTimes;						//��ʱ����
  osSemaphoreId *SemRxRdy; //���յ�һ�������ͷŵ��ź��������ѽ��մ�������
  u32 SemWaitMs;

	void						(*pfunTimeOut     )(                  void                ); //���ճ�ʱ������
  FrameStatusEnum (*pfunSearchFrame )(struct StreamRxFifo       * const fifo); //��Э�麯����ָ��
  void 						(*pfunProcessFrame)(struct StreamRxFifo const * const fifo); //��Э��ɹ���Ĵ�������ָ�� 
}CommonStreamRxFifoStruct;
 


 
                  /************����ӿں���*****************/
 
extern void CommonStreamRxFifoInit(	CommonStreamRxFifoStruct * const fifo,
    														                  s8 *instance,
    														                  u8 *FifoBuffer,
    														                  u16 MaxFifoLength,
    														                  u8 *FrameBuffer,
    														                  u16 MaxFrameLength,
    														                  osSemaphoreId * sem,
    														                  u32 SemWaitMs,
    														                  void *pfunTimeOut,
    														                  void *pfunSearchFrame,
    														                  void *pfunProcessFrame
														                    );

extern void inline CommonStreamRxFifoWriteIn( CommonStreamRxFifoStruct * const fifo, 
																													   u8  const * const buf,
																													   u16 const size
																													 );

extern void CommonStreamRxFifoProcess(CommonStreamRxFifoStruct * const fifo);
extern u8 CommonStreamRxFifoRead( CommonStreamRxFifoStruct * const fifo, 
                                                      u8 * const buf,
                                                      u16  size );







/*============================ TxStream ====================================*/

typedef enum
{
  NO_INIT = 0,
  INIT_OK = 0xA5,//��Ҫ�жϴ���
}InitStatusEnum;

typedef struct
{
	u32 size;
	u8* buf;    //�û���������
}CommonStreamTxQueueBufStruct;


/*	uart tx queue struct	*/
typedef struct TxStreamStruct
{
  InitStatusEnum InitStatus;
  CommonStreamTxQueueBufStruct* QueueBuf;  //�������ݽṹ��ָ�룬һ������������һ��buffer �� ���ʹ�Сsize
  u32 QueueBufSize;
  u32 QueueBufIndex;                       //Queue������±�
  u32 BufSizeMax;                          //QueueBuf��ÿ���������󳤶�
  u32 QueueWaitMs;                         //���г�ʱ�ȴ�ʱ��

  /*����ϵͳ��Դ��������������*/
  //osSemaphoreId BinarySemTxFinish;       //��ֵ�ź�������ʼ��Ϊ0����ʼ��Դ�����ã�ָʾһ֡���ݷ������
  osSemaphoreId CountingSemEmptyQueueSize; //�������ź�������ʼ��ΪQueueBufSize
  osMessageQId  Queue;                     //��Ϣ���У�����UartTxQueue

  osMutexId CriticalMutex;				 				 //�ٽ��������ź������û������Ƿ�ѡ��

  u32 MinIFS;                              //���͹����е���С֡�������λms
  u8 (*EnterCritical)(void);               //�����ٽ�������
  u8 (*ExitCritical)(void);                //�˳��ٽ�������

  u8 (*TransmitData)(u8 * const buf, u32 const size);   //ʵ�ʵĵײ㷢�ͺ���
  void (*TimeOut)(void);                                //��ʱû�յ���Ϣ

}CommonStreamTxStruct;



                    /************����ӿں���*****************/
extern void CommonStreamTxInit(CommonStreamTxStruct * const TxStream, //��ʼ�������ؽṹ��
                                    CommonStreamTxQueueBufStruct * const QueueBuf, //��������ָ��
                                    u8*   BufPtr,                            //��ά������Ԫ�ص�ַ BufPtr[QueueBufSize][BufSizeMax]
                                    u32   QueueBufSize,                      //���������С����BufPtr��һά��С
                                    u32   BufSizeMax,                        //ʵ�ʷ����������󳤶ȣ���BufPtr�ڶ�ά��С
                                    u32   QueueWaitTime,                     //��Ϣ���еȴ���ʱʱ��
                                    u32   MinIFS,                            //��̵Ĵ�����
                                    void* EnterCriticalFunction,             //�����ٽ�������
                                    void* ExitCriticalFunction,              //�˳��ٽ�������
                                    void* TransmitDataFunction,               //ʵ�ʵĵײ㷢�ͺ��������뷢����ɲſ��Է���
                                    void* TimeOutFunction
                                  );
extern u8   CommonStreamTxPutDataToQueue(CommonStreamTxStruct * const TxStream, u8 const * const buf, u32 const size);
extern void CommonStreamTxTask( CommonStreamTxStruct * const TxStream );

 
#ifdef __cplusplus
}
#endif


#endif


/*********END OF FILE****/


