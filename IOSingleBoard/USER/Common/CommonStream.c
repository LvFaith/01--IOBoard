/**
* @file     CommonStream.c
* @brief    
* @version  V0.01
* @author   zhicheng-ding@zerotech.com
* @date     2016-9-10
* @note     通用输入输出流控
*/



/*============================ INCLUDES ======================================*/
#include "CommonStream.h"

 

/*============================ IMPLEMENTATION ================================*/


/******************************************************************************
* @函 数 名： CommonStreamRxFifoInit
* @函数描述： 初始化接收FIFO
* @参    数： 
* @返 回 值： 无 
* @备    注： 
******************************************************************************/
void CommonStreamRxFifoInit( CommonStreamRxFifoStruct *fifo,
                                          s8 *instance,
                                          u8 *FifoBuf,
                                          u16 MaxFifoLen,
                                          u8 *FrameBuf,
                                          u16 MaxFrameLen,
                                          osSemaphoreId * sem,
                                          u32 SemWaitMs,
                                          void *pfunTimeOut,
                                          void *pfunSearchFrame,
                                          void *pfunProcessFrame
                                        )                  
{

  fifo->instance         = instance;
  fifo->FifoBuf       	 = FifoBuf;
	fifo->FrameBuf      	 = FrameBuf;
  fifo->MaxFifoLen    	 = MaxFifoLen;
  
  fifo->MaxFrameLen   	 = MaxFrameLen;
  fifo->CurFifoLen    	 = 0;
  fifo->CurFrameLen   	 = 0;
  fifo->ReadPos          = 0;
  fifo->WritePos         = 0;
  fifo->ErrTimes         = 0;
	fifo->SemRxRdy				 = sem;
  fifo->SemWaitMs        = SemWaitMs;

	fifo->pfunTimeOut      = ( void (*)(void) )pfunTimeOut;
  fifo->pfunSearchFrame  = ( FrameStatusEnum (*)(struct StreamRxFifo * const) )pfunSearchFrame;
  fifo->pfunProcessFrame = ( void (*)(struct StreamRxFifo const * const) )pfunProcessFrame;

}

/******************************************************************************
* @函 数 名： CommonStreamRxFifoWriteIn
* @函数描述： 将数据写入接收FIFO中，并释放一个信号量给CommonStreamRxFifoProcess
* @参    数： 
* @返 回 值： 无 
* @备    注： 
******************************************************************************/
void inline CommonStreamRxFifoWriteIn( CommonStreamRxFifoStruct * const fifo, 
                                                      u8  const * const buf,
                                                      u16 const size )
{

  if( (size <= 0) || (size > fifo->MaxFifoLen) )
  {
    return;
  }

#if 1
	if( size > (fifo->MaxFifoLen - fifo->WritePos) )
	{
		memcpy( &(fifo->FifoBuf[fifo->WritePos]), buf, fifo->MaxFifoLen - fifo->WritePos );
		memcpy( fifo->FifoBuf, &(buf[fifo->MaxFifoLen - fifo->WritePos]), size - (fifo->MaxFifoLen - fifo->WritePos) );
    fifo->WritePos = size - (fifo->MaxFifoLen - fifo->WritePos);
	}
	else
	{
		memcpy( &(fifo->FifoBuf[fifo->WritePos]), buf, size );
    fifo->WritePos = ( (size + fifo->WritePos) % (fifo->MaxFifoLen) );
	}

#else	
  for (u16 i = 0; i < size; i++)
  {
    fifo->FifoBuf[fifo->WritePos++] = buf[i];
    fifo->WritePos %= fifo->MaxFifoLen;
  }
#endif

  osSemaphoreRelease(*(fifo->SemRxRdy));
	
}

/******************************************************************************
* @函 数 名： CommonStreamRxFifoRead
* @函数描述： 将数据从FIFO中读出
* @参    数： 
* @返 回 值： 是否成功 
* @备    注： 
******************************************************************************/
u8 CommonStreamRxFifoRead( CommonStreamRxFifoStruct * const fifo, 
                                                      u8 * const buf,
                                                      u16  size )
{
  #define MAX_FIFO_LEN (fifo->MaxFifoLen)
	#define READ_POS     (fifo->ReadPos)

	if( ( size > MAX_FIFO_LEN ) || ( 0 == size ) )
	{
	  return FALSE;
	}
	
  if( size > (MAX_FIFO_LEN - READ_POS) )
	{
		memcpy( buf, &(fifo->FifoBuf[READ_POS]), MAX_FIFO_LEN - READ_POS );
		memcpy( &(buf[MAX_FIFO_LEN - READ_POS]), fifo->FifoBuf, size - (MAX_FIFO_LEN - READ_POS) );
	}
	else
	{
		memcpy( buf, &(fifo->FifoBuf[READ_POS]), size );
	}

  return TRUE;
}


/******************************************************************************
* @函 数 名： GetRxFifoLen
* @函数描述： 计算FIFO中待处理数据的长度
* @参    数： 
* @返 回 值： 无 
* @备    注： 
******************************************************************************/
static inline void GetRxFifoLen(CommonStreamRxFifoStruct * const fifo)
{
  fifo->CurFifoLen = (u16)((fifo->WritePos + fifo->MaxFifoLen - fifo->ReadPos) % fifo->MaxFifoLen);
}

/******************************************************************************
* @函 数 名： CommonStreamRxFifoUpdateReadPos
* @函数描述： 更新接收FIFO读指针
* @参    数： 
* @返 回 值： 无 
* @备    注： 
******************************************************************************/
static inline void CommonStreamRxFifoUpdateReadPos(	CommonStreamRxFifoStruct * const fifo,
																																	u16 const increment )
{
  fifo->ReadPos += increment;
  fifo->ReadPos %= fifo->MaxFifoLen;
}

/******************************************************************************
* @函 数 名： CommonStreamRxFifoProcess
* @函数描述： 接收数据流处理，一般由接收处理任务调用
* @参    数： 
* @返 回 值： 无 
* @备    注： 
******************************************************************************/
void CommonStreamRxFifoProcess(CommonStreamRxFifoStruct * const fifo)
{
  FrameStatusEnum state = RX_FRAME_LESS; 
  
  if (osSemaphoreWait(*(fifo->SemRxRdy), fifo->SemWaitMs) != osOK )
  {
  
    if(fifo->ErrTimes < 0xFFFE)
    {
      fifo->ErrTimes++;
    }
		
		if( NULL != fifo->pfunTimeOut )
		{
			(*fifo->pfunTimeOut)();
		}
		
    return;
		
  }
	
  fifo->ErrTimes = 0;
  GetRxFifoLen(fifo);   
  
  while (1)
  {
    if (NULL != fifo->pfunSearchFrame)
    {
      state = (*fifo->pfunSearchFrame)(fifo);
      switch (state)
      {
      
        case RX_FRAME_OK :
          CommonStreamRxFifoUpdateReadPos(fifo, fifo->CurFrameLen);
          fifo->CurFifoLen -= fifo->CurFrameLen;
          if (NULL != fifo->pfunProcessFrame)
          {
            (*fifo->pfunProcessFrame)(fifo);
          }
          break;
					
        case RX_FRAME_ERR :
          CommonStreamRxFifoUpdateReadPos(fifo, 1);
          fifo->CurFifoLen -= 1;
          break;
					
        case RX_FRAME_LESS :
          return;
					
        default :
          return;
					
      }
    }
		else
		{
			return;
		}
  }
}



//TxStream
#define ENTER_CRITICAL(__TX_STREAM__)       (  (*(__TX_STREAM__->EnterCritical))()  )
#define EXIT_CRITICAL(__TX_STREAM__)        (  (*(__TX_STREAM__->ExitCritical))()  )

#define TRANCEMIT_DATA(__TX_STREAM__,__BUF__,__SIZE__)    (  ( *(__TX_STREAM__->TransmitData) )(__BUF__, __SIZE__))


/******************************************************************************
* @函 数 名： BspTxStreamInit
* @函数描述： 初始化发送流
* @参    数： 
* @返 回 值： 无 
* @备    注： 
******************************************************************************/
void CommonStreamTxInit(CommonStreamTxStruct * const TxStream, //初始化的流控结构体
																			CommonStreamTxQueueBufStruct * const QueueBuf, //队列数组指针
																			u8* 	BufPtr, 													 //二维数组首元素地址 BufPtr[QueueBufSize][BufSizeMax]
																			u32 	QueueBufSize, 										 //队列数组大小，即BufPtr第一维大小
																			u32 	BufSizeMax, 											 //实际发送数组的最大长度，即BufPtr第二维大小
																			u32 	QueueWaitTime,										 //消息队列等待超时时间
																			u32 	MinIFS, 													 //最短的处理间隔
																			void* EnterCriticalFunction,						 //进入临界区函数
																			void* ExitCriticalFunction, 						 //退出临界区函数
																			void* TransmitDataFunction, 						 //实际的底层发送函数，必须发送完成才可以返回
																			void* TimeOutFunction                    //消息队列等待超时后做啥
																		)

{
  TxStream->QueueBuf      = QueueBuf;
  TxStream->QueueBufSize  = QueueBufSize;
  TxStream->BufSizeMax    = BufSizeMax;
  TxStream->QueueBufIndex = 0;

  for( u32 i = 0; i < QueueBufSize; i++ )
  {
    TxStream->QueueBuf[i].buf = BufPtr + BufSizeMax*i;
  }

  TxStream->MinIFS = MinIFS;
	TxStream->QueueWaitMs = QueueWaitTime;

  if( ( NULL == EnterCriticalFunction ) || ( NULL == ExitCriticalFunction ) )//只要有一个为NULL，就不能使用临界保护
  {
    TxStream->EnterCritical = NULL;
    TxStream->ExitCritical  = NULL;
  }
  else
  {
    TxStream->EnterCritical = (u8 (*)(void))EnterCriticalFunction;
    TxStream->ExitCritical = (u8 (*)(void))ExitCriticalFunction;
  }

  TxStream->TransmitData  = (u8 (*)(u8* const , u32 const))TransmitDataFunction;

	if( NULL == TimeOutFunction)
	{
	  TxStream->TimeOut = NULL;
	}
	else
	{
	  TxStream->TimeOut = (void (*)(void))TimeOutFunction;
	}

  TxStream->InitStatus = INIT_OK;
}


/******************************************************************************
* @函 数 名： CommonStreamTxPutDataToQueue
* @函数描述： 发送数据到消息队列
* @参    数： 
* @返 回 值： 无 
* @备    注： 
******************************************************************************/
u8 CommonStreamTxPutDataToQueue(CommonStreamTxStruct* const TxStream, u8 const * const buf, u32 const size)
{
  //不要送入队列0长度的数据或者越界的数据，并且txstream必须初始化完成
  if( (0 == size) || (size > TxStream->BufSizeMax) || (INIT_OK != TxStream->InitStatus) )
  {
      return FALSE;
  }

  if(osOK == osSemaphoreWait(TxStream->CountingSemEmptyQueueSize, 0))//wait conuting sem
  {
    //*****************临界区**********************//
    if ( NULL != TxStream->EnterCritical ) //初始化的时候已经保证了EnterCritical ExitCritical同为NULL或者都不为NULL
    {
        ENTER_CRITICAL(TxStream);//进入临界区
    }
    
    memcpy(TxStream->QueueBuf[TxStream->QueueBufIndex].buf, buf, size);
    TxStream->QueueBuf[TxStream->QueueBufIndex].size = size;
    osMessagePut(TxStream->Queue, (u32)( &(TxStream->QueueBuf[TxStream->QueueBufIndex]) ), 0);
    TxStream->QueueBufIndex++;
    TxStream->QueueBufIndex %= TxStream->QueueBufSize;

    if ( NULL != TxStream->ExitCritical ) //初始化的时候已经保证了EnterCritical ExitCritical同为NULL或者都不为NULL
    {
        EXIT_CRITICAL(TxStream);//退出临界区
    }
    return TRUE;
    //*****************临界区**********************//
  }
  else
  {
    return FALSE;
  }
    
}


/******************************************************************************
* @函 数 名： CommonStreamTxTask
* @函数描述： 发送数据流守护任务，底层守护任务
* @参    数： 
* @返 回 值： 无 
* @备    注： 
******************************************************************************/
void CommonStreamTxTask( CommonStreamTxStruct * const TxStream )
{
  osEvent Event;

  if( ( NULL == TxStream ) || (INIT_OK != TxStream->InitStatus) )
  {
      return;
  }

  while(1)
  {
    Event = osMessageGet (TxStream->Queue, TxStream->QueueWaitMs);
    if(osEventMessage == Event.status)
    {
      if( 0 != ((CommonStreamTxQueueBufStruct*)(Event.value.p))->size )//防止发送的数据长度为0
      {
        TRANCEMIT_DATA( TxStream,
                        ((CommonStreamTxQueueBufStruct*)(Event.value.p))->buf, 
                        ((CommonStreamTxQueueBufStruct*)(Event.value.p))->size);
      }
      osSemaphoreRelease(TxStream->CountingSemEmptyQueueSize);
      
      if( TxStream->MinIFS )
      {
        osDelay( TxStream->MinIFS );//最小帧间隔
      }
    }
  	else if( (osEventTimeout == Event.status) && (NULL != TxStream->TimeOut) )
  	{
  	  ( *(TxStream->TimeOut) )();
  	}
  }
}


/*********END OF FILE****/



