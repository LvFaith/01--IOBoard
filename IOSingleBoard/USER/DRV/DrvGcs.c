
/*============================ INCLUDES ======================================*/

#include "CommonStream.h"
#include "DrvGcs.h"
#include "freertos.h"
#include "DrvRTK.h"

extern osMutexId GcsTxMutexHandle;

extern osMessageQId myQueue_GCSHandle;

static void ComGcsNuaaSaveData_Queue(u8* buf, u32 size);  //协议处理

/*============================ MACROS ========================================*/

/*============================ TYPES =========================================*/

/*============================ GLOBAL VARIABLES ==============================*/

/*============================ IMPLEMENTATION ================================*/

/*============================ INCLUDES ======================================*/

/*========================= tx stream to GPS ==================================*/
#define GCS_TX_QUEUE_BUF_SIZE   	(2) //发送消息队列大小
#define GCS_TX_BUF_SIZE_MAX       (256)//目前一次最多发多少字节

static CommonStreamTxStruct s_GcsTxStream;

static CommonStreamTxQueueBufStruct s_GcsTxQueueBuf[GCS_TX_QUEUE_BUF_SIZE];
static u8 s_GcsTxBuf[GCS_TX_QUEUE_BUF_SIZE][GCS_TX_BUF_SIZE_MAX];
/*==================================== end ======================================*/

/*========================= rx stream from GPS ================================*/
#define GCS_RX_FIFO_SIZE         (512)
#define GCS_RX_FRAME_LEN_MAX     (255)
#define GCS_RX_FRAME_LEN_MIN     (4)

static CommonStreamRxFifoStruct s_GcsRxFifo;
static u8 s_GcsRxFifoBuf[GCS_RX_FIFO_SIZE];
static u8 s_GcsRxFrameBuf[GCS_RX_FRAME_LEN_MAX];

static osSemaphoreId s_GcsRxSem;

static void GcsRxProcessFrame(CommonStreamRxFifoStruct const * const fifo);
static FrameStatusEnum GcsRxSearchFrame(CommonStreamRxFifoStruct * const fifo);


static FrameStatusEnum GcsRxSearchFrame(CommonStreamRxFifoStruct * const fifo)
{
  return RX_FRAME_OK;
}

static void GcsRxProcessFrame(CommonStreamRxFifoStruct const * const fifo)
{
}
/*============================ IMPLEMENTATION ================================*/

static void GcsSaveRxData(u8 const * buf, u16 const size)
{
  CommonStreamRxFifoWriteIn(&s_GcsRxFifo, buf, size);

	#if 1
	for(int i = 0; i < size; i++)
	{
		SEGGER_RTT_printf(0,"%.2x ",buf[i]);
	}
//	SEGGER_RTT_printf(0,"size %d sum %d\n",size,CalcSum(buf,size-1));
	#endif
}

static inline void GcsDataSendBlock( u8 * const buf, u32 const size )
{
  BspUartTransmitDataBlock( &(g_BspUartTxGroup.uart6), buf, size);
}

static void DrvGcsInit(void)
{
  //tx stream init
  memset( s_GcsTxQueueBuf, 0, sizeof(s_GcsTxQueueBuf) );
  memset( s_GcsTxBuf,      0, sizeof(s_GcsTxBuf)      );
  memset( &s_GcsTxStream,  0, sizeof(CommonStreamTxStruct) );
  
  osSemaphoreDef(GcsCountingSemTx);
  s_GcsTxStream.CountingSemEmptyQueueSize = osUserSemaphoreCreate(osSemaphore(GcsCountingSemTx), GCS_TX_QUEUE_BUF_SIZE, GCS_TX_QUEUE_BUF_SIZE);

  osMessageQDef(GcsQueueTx, GCS_TX_QUEUE_BUF_SIZE, u32);
  s_GcsTxStream.Queue = osMessageCreate(osMessageQ(GcsQueueTx), NULL);

  CommonStreamTxInit( &s_GcsTxStream,
                   		s_GcsTxQueueBuf,
                   		&(s_GcsTxBuf[0][0]),
                   		(u32)GCS_TX_QUEUE_BUF_SIZE,
                   		(u32)GCS_TX_BUF_SIZE_MAX,
                   		osWaitForever,
                   		1,
                   		(void *)NULL,
                   		(void *)NULL,
                   		(void*)GcsDataSendBlock,
                   		NULL);
  //rx stream init
  memset( s_GcsRxFifoBuf,  0, sizeof(s_GcsRxFifoBuf)  );
  memset( s_GcsRxFrameBuf, 0, sizeof(s_GcsRxFrameBuf) );

  osSemaphoreDef(GcsBinarySemRx);
  s_GcsRxSem = osUserSemaphoreCreate(osSemaphore(GcsBinarySemRx), 1, 0);
	s_GcsRxFifo.SemRxRdy = &s_GcsRxSem;
	
	CommonStreamRxFifoInit(	&s_GcsRxFifo,
							"GcsRxFifo",
							s_GcsRxFifoBuf,
							GCS_RX_FIFO_SIZE,
							s_GcsRxFrameBuf,
							GCS_RX_FRAME_LEN_MAX,
							s_GcsRxFifo.SemRxRdy,
							osWaitForever,
							NULL,
							(void *)GcsRxSearchFrame,
							(void *)GcsRxProcessFrame);

  g_BspUartProRxFunc.uart6 = &GcsSaveRxData;//注册实际的底层接收函数
}

void GcsRx1Daemon(void const * argument)
{
  osDelay(1000);
  DrvGcsInit();
  int cnt;
  cnt = 0;
  
  while(1)
  {
       osDelay(10000); 
  }
}

void GcsRx2Daemon(void const * argument)
{
  osDelay(2000);
  while(1)
  {
    osDelay(100);
    CommonStreamTxTask(&s_GcsTxStream);
  }
}


void DrvGcsSendData(u8 * buf, u16 size)
{
  CommonStreamTxPutDataToQueue(&s_GcsTxStream, buf, size);
}


static void ComGcsNuaaSaveData_Queue(u8* buf, u32 size)   //协议处理
{
  
  	u8 TempIndex;
    u32 sed_data = 0;

    for(u16 i=0;i<size/4+1;i++)
    { 
         sed_data = 0;
        
        if(i != size/4)
        {
          memcpy(&sed_data,&buf[i*4],4);      
        }
        else
        {
          memcpy(&sed_data,&buf[i*4],size%4);
        }
        osMessagePut(myQueue_GCSHandle,(u32)sed_data , 0);
    }
}

typedef struct
{
	u16 Size;
	u8* Buf;    //用户定义数组
}ComGcsNuaaTx;

/*
 * 向地面站发消息包的接口函数
 * 参数：
 * com - 发的端口号，默认零号电台，如果有多个电台，依次递增编号
 * msg - 消息包的首地址
 * len - 消息包的长度
 * return - 0 表示发送成功
 */
int SendMessageTo(uint8_t com, uint8_t *msg, uint16_t len)
{
	DrvGcsSendData(msg, len);
	return 0;
}

uint8_t *proc_done = NULL;

void ComGcsNuaaReceiveData_Queue(void)  
{
  u8 data[4];
	uint8_t *hit;
	
	if (proc_done && *proc_done)
	{
	  //processing
		return;
	}
  else
	{
		if (proc_done && !*proc_done)
		{
		   proc_done = NULL;
		}
		
		while (xQueueReceive(myQueue_GCSHandle, data, 0))
		{
	/******************test code************************/
	//    SEGGER_RTT_printf(0,"ComGcsNuaaTx %.2x , %.2x, %.2x, %.2x",data[0],data[1],data[2],data[3]); 
	/******************test code************************/    
			for (int i = 0; i <4; ++i)
			{
		//		hit = ProcessMessageFrom(0, data[i]);
				if (hit)
				{
					proc_done = hit;
				}
			}
			if (proc_done)
				break;
		}
	}
}





