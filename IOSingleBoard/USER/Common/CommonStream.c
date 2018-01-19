/**
* @file     CommonStream.c
* @brief    
* @version  V0.01
* @author   zhicheng-ding@zerotech.com
* @date     2016-9-10
* @note     ͨ�������������
*/



/*============================ INCLUDES ======================================*/
#include "CommonStream.h"

 

/*============================ IMPLEMENTATION ================================*/


/******************************************************************************
* @�� �� ���� CommonStreamRxFifoInit
* @���������� ��ʼ������FIFO
* @��    ���� 
* @�� �� ֵ�� �� 
* @��    ע�� 
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
* @�� �� ���� CommonStreamRxFifoWriteIn
* @���������� ������д�����FIFO�У����ͷ�һ���ź�����CommonStreamRxFifoProcess
* @��    ���� 
* @�� �� ֵ�� �� 
* @��    ע�� 
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
* @�� �� ���� CommonStreamRxFifoRead
* @���������� �����ݴ�FIFO�ж���
* @��    ���� 
* @�� �� ֵ�� �Ƿ�ɹ� 
* @��    ע�� 
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
* @�� �� ���� GetRxFifoLen
* @���������� ����FIFO�д��������ݵĳ���
* @��    ���� 
* @�� �� ֵ�� �� 
* @��    ע�� 
******************************************************************************/
static inline void GetRxFifoLen(CommonStreamRxFifoStruct * const fifo)
{
  fifo->CurFifoLen = (u16)((fifo->WritePos + fifo->MaxFifoLen - fifo->ReadPos) % fifo->MaxFifoLen);
}

/******************************************************************************
* @�� �� ���� CommonStreamRxFifoUpdateReadPos
* @���������� ���½���FIFO��ָ��
* @��    ���� 
* @�� �� ֵ�� �� 
* @��    ע�� 
******************************************************************************/
static inline void CommonStreamRxFifoUpdateReadPos(	CommonStreamRxFifoStruct * const fifo,
																																	u16 const increment )
{
  fifo->ReadPos += increment;
  fifo->ReadPos %= fifo->MaxFifoLen;
}

/******************************************************************************
* @�� �� ���� CommonStreamRxFifoProcess
* @���������� ��������������һ���ɽ��մ����������
* @��    ���� 
* @�� �� ֵ�� �� 
* @��    ע�� 
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
* @�� �� ���� BspTxStreamInit
* @���������� ��ʼ��������
* @��    ���� 
* @�� �� ֵ�� �� 
* @��    ע�� 
******************************************************************************/
void CommonStreamTxInit(CommonStreamTxStruct * const TxStream, //��ʼ�������ؽṹ��
																			CommonStreamTxQueueBufStruct * const QueueBuf, //��������ָ��
																			u8* 	BufPtr, 													 //��ά������Ԫ�ص�ַ BufPtr[QueueBufSize][BufSizeMax]
																			u32 	QueueBufSize, 										 //���������С����BufPtr��һά��С
																			u32 	BufSizeMax, 											 //ʵ�ʷ����������󳤶ȣ���BufPtr�ڶ�ά��С
																			u32 	QueueWaitTime,										 //��Ϣ���еȴ���ʱʱ��
																			u32 	MinIFS, 													 //��̵Ĵ�����
																			void* EnterCriticalFunction,						 //�����ٽ�������
																			void* ExitCriticalFunction, 						 //�˳��ٽ�������
																			void* TransmitDataFunction, 						 //ʵ�ʵĵײ㷢�ͺ��������뷢����ɲſ��Է���
																			void* TimeOutFunction                    //��Ϣ���еȴ���ʱ����ɶ
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

  if( ( NULL == EnterCriticalFunction ) || ( NULL == ExitCriticalFunction ) )//ֻҪ��һ��ΪNULL���Ͳ���ʹ���ٽ籣��
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
* @�� �� ���� CommonStreamTxPutDataToQueue
* @���������� �������ݵ���Ϣ����
* @��    ���� 
* @�� �� ֵ�� �� 
* @��    ע�� 
******************************************************************************/
u8 CommonStreamTxPutDataToQueue(CommonStreamTxStruct* const TxStream, u8 const * const buf, u32 const size)
{
  //��Ҫ�������0���ȵ����ݻ���Խ������ݣ�����txstream�����ʼ�����
  if( (0 == size) || (size > TxStream->BufSizeMax) || (INIT_OK != TxStream->InitStatus) )
  {
      return FALSE;
  }

  if(osOK == osSemaphoreWait(TxStream->CountingSemEmptyQueueSize, 0))//wait conuting sem
  {
    //*****************�ٽ���**********************//
    if ( NULL != TxStream->EnterCritical ) //��ʼ����ʱ���Ѿ���֤��EnterCritical ExitCriticalͬΪNULL���߶���ΪNULL
    {
        ENTER_CRITICAL(TxStream);//�����ٽ���
    }
    
    memcpy(TxStream->QueueBuf[TxStream->QueueBufIndex].buf, buf, size);
    TxStream->QueueBuf[TxStream->QueueBufIndex].size = size;
    osMessagePut(TxStream->Queue, (u32)( &(TxStream->QueueBuf[TxStream->QueueBufIndex]) ), 0);
    TxStream->QueueBufIndex++;
    TxStream->QueueBufIndex %= TxStream->QueueBufSize;

    if ( NULL != TxStream->ExitCritical ) //��ʼ����ʱ���Ѿ���֤��EnterCritical ExitCriticalͬΪNULL���߶���ΪNULL
    {
        EXIT_CRITICAL(TxStream);//�˳��ٽ���
    }
    return TRUE;
    //*****************�ٽ���**********************//
  }
  else
  {
    return FALSE;
  }
    
}


/******************************************************************************
* @�� �� ���� CommonStreamTxTask
* @���������� �����������ػ����񣬵ײ��ػ�����
* @��    ���� 
* @�� �� ֵ�� �� 
* @��    ע�� 
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
      if( 0 != ((CommonStreamTxQueueBufStruct*)(Event.value.p))->size )//��ֹ���͵����ݳ���Ϊ0
      {
        TRANCEMIT_DATA( TxStream,
                        ((CommonStreamTxQueueBufStruct*)(Event.value.p))->buf, 
                        ((CommonStreamTxQueueBufStruct*)(Event.value.p))->size);
      }
      osSemaphoreRelease(TxStream->CountingSemEmptyQueueSize);
      
      if( TxStream->MinIFS )
      {
        osDelay( TxStream->MinIFS );//��С֡���
      }
    }
  	else if( (osEventTimeout == Event.status) && (NULL != TxStream->TimeOut) )
  	{
  	  ( *(TxStream->TimeOut) )();
  	}
  }
}


/*********END OF FILE****/



