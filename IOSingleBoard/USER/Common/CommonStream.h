/**
* @file     CommonStream.h
* @brief    
* @version  V0.01
* @author   zhicheng-ding@zerotech.com
* @date     2016-9-10
* @note     通用输入输出流控
*/

#ifndef _COMMON_STREAM_H_
#define _COMMON_STREAM_H_

#ifdef __cplusplus
 extern "C" {
#endif 
 
/*============================ INCLUDES ======================================*/
 
#include "Config.h"


/*============================ Rx Stream =========================================*/

//解析所有输入数据流协议只能出现以下三种状态

typedef enum
{
	RX_FRAME_OK,
	RX_FRAME_ERR,
	RX_FRAME_LESS,
}FrameStatusEnum;
 
typedef struct StreamRxFifo
{
  s8 *instance;						//名字
  u8 *FifoBuf;						//FifoBuf:环形fifo
	u8 *FrameBuf;				
  u16 MaxFifoLen;					//fifo的最大长度
	u16 MaxFrameLen;							
  u16 CurFifoLen;					//CurFifoLen:当前fifo的长度
	u16 CurFrameLen;				//当前解析成功出来的帧长度		
  u16 ReadPos;						//读指针
	u16 WritePos;						//写指针
  u16 ErrTimes;						//超时次数
  osSemaphoreId *SemRxRdy; //接收到一包数据释放掉信号量，唤醒接收处理任务
  u32 SemWaitMs;

	void						(*pfunTimeOut     )(                  void                ); //接收超时处理函数
  FrameStatusEnum (*pfunSearchFrame )(struct StreamRxFifo       * const fifo); //解协议函数的指针
  void 						(*pfunProcessFrame)(struct StreamRxFifo const * const fifo); //解协议成功后的处理函数的指针 
}CommonStreamRxFifoStruct;
 


 
                  /************对外接口函数*****************/
 
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
  INIT_OK = 0xA5,//需要判断此项
}InitStatusEnum;

typedef struct
{
	u32 size;
	u8* buf;    //用户定义数组
}CommonStreamTxQueueBufStruct;


/*	uart tx queue struct	*/
typedef struct TxStreamStruct
{
  InitStatusEnum InitStatus;
  CommonStreamTxQueueBufStruct* QueueBuf;  //发送数据结构体指针，一个发送流就是一个buffer 加 发送大小size
  u32 QueueBufSize;
  u32 QueueBufIndex;                       //Queue数组的下标
  u32 BufSizeMax;                          //QueueBuf中每个数组的最大长度
  u32 QueueWaitMs;                         //队列超时等待时间

  /*以下系统资源用来做流量控制*/
  //osSemaphoreId BinarySemTxFinish;       //二值信号量，初始化为0，起始资源不可用，指示一帧数据发送完成
  osSemaphoreId CountingSemEmptyQueueSize; //计数型信号量，初始化为QueueBufSize
  osMessageQId  Queue;                     //消息队列，管理UartTxQueue

  osMutexId CriticalMutex;				 				 //临界区互斥信号量，用户决定是否选用

  u32 MinIFS;                              //发送过程中的最小帧间隔，单位ms
  u8 (*EnterCritical)(void);               //进入临界区函数
  u8 (*ExitCritical)(void);                //退出临界区函数

  u8 (*TransmitData)(u8 * const buf, u32 const size);   //实际的底层发送函数
  void (*TimeOut)(void);                                //超时没收到消息

}CommonStreamTxStruct;



                    /************对外接口函数*****************/
extern void CommonStreamTxInit(CommonStreamTxStruct * const TxStream, //初始化的流控结构体
                                    CommonStreamTxQueueBufStruct * const QueueBuf, //队列数组指针
                                    u8*   BufPtr,                            //二维数组首元素地址 BufPtr[QueueBufSize][BufSizeMax]
                                    u32   QueueBufSize,                      //队列数组大小，即BufPtr第一维大小
                                    u32   BufSizeMax,                        //实际发送数组的最大长度，即BufPtr第二维大小
                                    u32   QueueWaitTime,                     //消息队列等待超时时间
                                    u32   MinIFS,                            //最短的处理间隔
                                    void* EnterCriticalFunction,             //进入临界区函数
                                    void* ExitCriticalFunction,              //退出临界区函数
                                    void* TransmitDataFunction,               //实际的底层发送函数，必须发送完成才可以返回
                                    void* TimeOutFunction
                                  );
extern u8   CommonStreamTxPutDataToQueue(CommonStreamTxStruct * const TxStream, u8 const * const buf, u32 const size);
extern void CommonStreamTxTask( CommonStreamTxStruct * const TxStream );

 
#ifdef __cplusplus
}
#endif


#endif


/*********END OF FILE****/


