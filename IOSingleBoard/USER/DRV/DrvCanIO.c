/*
******************************************************************************
*@文件名字  : DrvComIO.c
*@描述	    : 和5院IO板通讯
******************************************************************************
*@作者		: 吕全辉
*@强制性规定: 移植时只可以在 USER CODE BEGIN  USER CODE END  之间删改代码!!!
*@版本		: V1.00      2017/9/7
******************************************************************************
*/


#include "DrvCanIO.h"

#include "main.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "can.h"
#include "gpio.h"

#define Tx_TO_IO_ID     ((u32)(0x0120))    //(0x0500))//0521


#define COM_IO_RX_FIFO_SIZE      (256)
#define COM_IO_RX_FRAME_LEN_MAX  (8)
#define COM_IO_RX_FRAME_LEN_MIN  (8)

#define COM_IO_RX_SEM_WAIT_MS    (1000)//100ms



#define TX_QUEUE_BUF_SIZE   (5)
#define TX_BUF_SIZE_MAX     (64)

static CommonStreamTxQueueBufStruct s_QueueBuf[TX_QUEUE_BUF_SIZE];
u8 s_TxBuf[TX_QUEUE_BUF_SIZE][TX_BUF_SIZE_MAX];

CommonStreamTxStruct g_TxStreamToIO;

CommonStreamRxFifoStruct s_ComIORxFifo;

static osSemaphoreId s_IORxSem;

static u8 s_ComIORxFifoBuf[COM_IO_RX_FIFO_SIZE];
static u8 s_ComIORxFrameBuf[COM_IO_RX_FRAME_LEN_MAX];


u8 sum1=0,sum2 = 0;
u8 Buftest[10] = {0};
u16 lentest = 0;
static void ComIOSaveData(u8* buf, u32 size);   //存储数据到fifo

static u8 ComIOCheckSum(const u8* const Buf, const u16 Len)
{
    u8 Sum = 0;
  //  memcpy(Buftest,Buf,Len);
    for(u16 i = 0; i < Len - 1; i++ )
    {
        Sum += Buf[i];
    }
    return (Sum == Buf[Len-1]);    
}
//u8 DrvTxToIO(CAN_HandleTypeDef *hcan, u8* buf, u32 size ) 
//{
//  return BspCanTxData(hcan, Tx_TO_IO_ID, buf, size);
//}


static u8 ComIOSearchFrame(CommonStreamRxFifoStruct* fifo)
{
  u16 i, FrameLen, ReadPos;
  u8 TempBuf[2];
  u16 len = 0;
  /* check whether data enough */
  if (fifo->CurFifoLen < COM_IO_RX_FRAME_LEN_MIN) /* make an error: "buffer_length() > MIN_UBX_MSG_LENGTH" */
  {
    //SEGGER_RTT_printf(0,"fun %s,Line %d \n",__FUNCTION__,__LINE__);
    return RX_FRAME_LESS;
    
  }

  ReadPos = fifo->ReadPos;

  //SEGGER_RTT_printf(0,"fun %s,Line %d \n",__FUNCTION__,__LINE__);
  /* check header */
  if (0xA0 != fifo->FifoBuf[ReadPos++])
  {
    
    return RX_FRAME_ERR;
  }
  ReadPos %= fifo->MaxFifoLen;

  //SEGGER_RTT_printf(0,"fun %s,Line %d \n",__FUNCTION__,__LINE__);

    ReadPos++;
    ReadPos %= fifo->MaxFifoLen;
    
  //SEGGER_RTT_printf(0,"fun %s,Line %d \n",__FUNCTION__,__LINE__);
  /* check frame header */

   len = fifo->FifoBuf[ReadPos++];
   ReadPos %= fifo->MaxFifoLen;

  if( len !=8 )
  {
    return RX_FRAME_ERR;
  }

  /* check frame length */
  FrameLen = len;
  if ((FrameLen > fifo->MaxFrameLen)) 
  {
    //trace(TEROR,"error,invalid frame len=%d",FrameLen);
    return RX_FRAME_ERR;
  }

  /* check data enough or not */        
  if (fifo->CurFifoLen < FrameLen)
  {
    //trace(TINFO,"data is coming, fifolen=%d, framelen=%d",fifo->CurFifoLength,FrameLen);
    return RX_FRAME_LESS;
  }

  ReadPos = fifo->ReadPos;

  /* copy bytes from fifo to frame[] */
  for (i = 0; i < FrameLen; i++)
  {
    fifo->FrameBuf[i] = fifo->FifoBuf[ReadPos++];
    ReadPos %= fifo->MaxFifoLen;
  }
  memcpy(Buftest,fifo->FrameBuf,8);
//  sum1 = 0;
//   for(u16 i = 0; i < len-1; i++ )
//  {
//        sum1 += fifo->FrameBuf[i];
//  }
//  sum2 = fifo->FrameBuf[7];
  /* check sum */
  if (!ComIOCheckSum(&fifo->FrameBuf[0], len))
  {
// //    trace(TEROR,"checksum failed\r\n");
    return RX_FRAME_ERR;        
  }

  /* return frame length */
  fifo->CurFrameLen = FrameLen;
  
  return RX_FRAME_OK;
  
}

/******************************************************************************
* @函 数 名： ComIOProcessFrame
* @函数描述： 校验成功对一帧数据进行处理
* @参    数： 
* @返 回 值：
* @备    注：

******************************************************************************/
                 
static void ComIOProcessFrame(CommonStreamRxFifoStruct *fifo)
{
  u8 idTempp;
  
  idTempp = fifo->FrameBuf[1];
  switch(idTempp)
  {
    default:
      break;
  }
}

void DrvComIOInit(void)
{
//    memset( s_QueueBuf, 0, sizeof(s_QueueBuf) );
//    memset( s_TxBuf, 0, sizeof(s_TxBuf) );
//    memset( &g_TxStreamToIO, 0, sizeof(CommonStreamTxStruct));

//    osSemaphoreDef(CountingSemTxToIO);
//    g_TxStreamToIO.CountingSemEmptyQueueSize = osUserSemaphoreCreate(osSemaphore(
//    CountingSemTxToIO), TX_QUEUE_BUF_SIZE, TX_QUEUE_BUF_SIZE);

//    osMessageQDef(QueueTxToIO, TX_QUEUE_BUF_SIZE, u32);
//    g_TxStreamToIO.Queue = osMessageCreate(osMessageQ(QueueTxToIO), NULL);
//     

//    CommonStreamTxInit( &g_TxStreamToIO,
//                   s_QueueBuf,
//                   &(s_TxBuf[0][0]),
//                   (u32)TX_QUEUE_BUF_SIZE,
//                   (u32)TX_BUF_SIZE_MAX,
//                   1,
//                   5,                   
//                   NULL,
//                   NULL,
//                   (void*)DrvTxToIO,
//                   (void*)DrvTxToIO);
    //rx                
    memset(s_ComIORxFifoBuf,0,sizeof(s_ComIORxFifoBuf));
    memset(s_ComIORxFrameBuf,  0, sizeof(s_ComIORxFrameBuf));
    
    osSemaphoreDef(IOBinarySemRx);
    s_IORxSem = osUserSemaphoreCreate(osSemaphore(IOBinarySemRx), 1, 0);
	  s_ComIORxFifo.SemRxRdy = &s_IORxSem;

    CommonStreamRxFifoInit(  &s_ComIORxFifo,
                  "SlaverRxFifo",
                  s_ComIORxFifoBuf, 
                  COM_IO_RX_FIFO_SIZE, 
                  s_ComIORxFrameBuf, 
                  COM_IO_RX_FRAME_LEN_MAX, 
                  s_ComIORxFifo.SemRxRdy,
									100,
                  NULL,
                  (void*)(&ComIOSearchFrame),
                  (void*)(&ComIOProcessFrame) );
    	           
    osSemaphoreDef(BinarySemComIORx);
    s_ComIORxFifo.SemRxRdy = osUserSemaphoreCreate(osSemaphore(BinarySemComIORx), 1, 0);

    g_CanProcess.Can3ReadData = &ComIOSaveData;//注册接收处理函数 ;//必须最后注册!!  

}


static u8 DrvSum(u8* Buf, u16 Size)
{
    u8 Sum = 0,i=0;
    
    for(i = 0; i < Size; i++)
        Sum += Buf[i];

    return Sum;
}

static void ComIOSaveData(u8* buf, u32 size)   //协议处理
{
    #if 0
    SEGGER_RTT_printf(0,"Can2Rx:\n");
    for(u8 i = 0; i < size; i++)
    {
      SEGGER_RTT_printf(0,"%02x ",buf[i]);
    }
    SEGGER_RTT_printf(0,"\n");
    #endif
    CommonStreamRxFifoWriteIn(&s_ComIORxFifo, buf, size);\
    
}

CanTxMsg CanDate;

void  CanRxDaemon(void const * argument)
{
    if( HAL_CAN_Receive_IT(&hcan1, CAN_FIFO1) != HAL_OK )
    {
        SEGGER_RTT_printf(0,"HAL_CAN1_Receive_IT err\r\n");
    }
    osDelay(1000);
    DrvComIOInit();
    while(1)
    {
        CommonStreamRxFifoProcess(&s_ComIORxFifo);
    }
}


volatile u8 ReadPinDataTemp = 0 , ReadPinDataTempLast = 0 ,ReadPinTemp = 0;
void GPIOInDaemon(void const * argument)
{
    while(1)  
   {
//      ReadPinDataTemp = HAL_GPIO_ReadPin(UART1RX_GPIO_Port,UART1RX_Pin);
//    ReadPinDataTemp = ReadPinDataTemp << 1;
//    ReadPinDataTemp = HAL_GPIO_ReadPin(UART1TX_GPIO_Port,UART1TX_Pin);
//    ReadPinDataTemp = ReadPinDataTemp << 1;
//    ReadPinDataTemp = HAL_GPIO_ReadPin(UART2RX_GPIO_Port,UART2RX_Pin);
//    ReadPinDataTemp = ReadPinDataTemp << 1;
//    ReadPinDataTemp = HAL_GPIO_ReadPin(UART2TX_GPIO_Port,UART2TX_Pin);
//    ReadPinDataTemp = ReadPinDataTemp << 1;
//    ReadPinDataTemp = HAL_GPIO_ReadPin(UART3RX_GPIO_Port,UART3RX_Pin);
//    ReadPinDataTemp = ReadPinDataTemp << 1;
//    ReadPinDataTemp = HAL_GPIO_ReadPin(UART3TX_GPIO_Port,UART3TX_Pin);
//    ReadPinDataTemp = ReadPinDataTemp << 1;
//    ReadPinDataTemp = HAL_GPIO_ReadPin(UART4RX_GPIO_Port,UART4RX_Pin);
//    ReadPinDataTemp = ReadPinDataTemp << 1;
//    ReadPinDataTemp = HAL_GPIO_ReadPin(UART4TX_GPIO_Port,UART4TX_Pin);
//    ReadPinDataTemp = ReadPinDataTemp << 1;
//    
//    ReadPinDataTempLast = HAL_GPIO_ReadPin(UART5RX_GPIO_Port,UART5RX_Pin);
//    ReadPinDataTempLast = ReadPinDataTempLast << 1;
//    ReadPinDataTempLast = HAL_GPIO_ReadPin(UART5TX_GPIO_Port,UART5TX_Pin);
//    ReadPinDataTempLast = ReadPinDataTempLast << 1;
//    ReadPinDataTempLast = HAL_GPIO_ReadPin(UART6RX_GPIO_Port,UART6RX_Pin);
//    ReadPinDataTempLast = ReadPinDataTempLast << 1;
//    ReadPinDataTempLast = HAL_GPIO_ReadPin(UART6TX_GPIO_Port,UART6TX_Pin);
//    ReadPinDataTempLast = ReadPinDataTempLast << 1;
//    ReadPinDataTempLast = HAL_GPIO_ReadPin(UART7RX_GPIO_Port,UART7RX_Pin);
//    ReadPinDataTempLast = ReadPinDataTempLast << 1;
//    ReadPinDataTempLast = HAL_GPIO_ReadPin(UART7TX_GPIO_Port,UART7TX_Pin);
//    ReadPinDataTempLast = ReadPinDataTempLast << 1;
    
//    CanDate.SendBitsDataPre = ReadPinDataTemp;
//    CanDate.SendBitsDataLater = ReadPinDataTempLast;
    osDelay(50000);
  }
}


void CanTxDaemon(void const * argument)
{
    u16 checkdate = 0;  
    u16 time_delay = 0;
    while(1)
    {
          ReadPinDataTemp = 0 ;
          ReadPinDataTempLast = 0;
          ReadPinTemp = 0;
          checkdate = 0;
      
          ReadPinTemp = HAL_GPIO_ReadPin(UART1RX_GPIO_Port,UART1RX_Pin);
          ReadPinDataTemp = ReadPinTemp;
          ReadPinTemp = HAL_GPIO_ReadPin(UART1TX_GPIO_Port,UART1TX_Pin);
          ReadPinDataTemp |= ReadPinTemp << 1;
          ReadPinTemp = HAL_GPIO_ReadPin(UART2RX_GPIO_Port,UART2RX_Pin);
          ReadPinDataTemp |= ReadPinTemp << 2;
          ReadPinTemp = HAL_GPIO_ReadPin(UART2TX_GPIO_Port,UART2TX_Pin);
          ReadPinDataTemp |= ReadPinTemp << 3;
          ReadPinTemp = HAL_GPIO_ReadPin(UART3RX_GPIO_Port,UART3RX_Pin);
          ReadPinDataTemp |= ReadPinTemp << 4;
          ReadPinTemp = HAL_GPIO_ReadPin(UART3TX_GPIO_Port,UART3TX_Pin);
          ReadPinDataTemp |= ReadPinTemp << 5;
          ReadPinTemp = HAL_GPIO_ReadPin(UART4RX_GPIO_Port,UART4RX_Pin);
          ReadPinDataTemp |= ReadPinTemp << 6;
          ReadPinTemp = HAL_GPIO_ReadPin(UART4TX_GPIO_Port,UART4TX_Pin);
          ReadPinDataTemp |= ReadPinTemp << 7;
          
          ReadPinTemp = 0;
          ReadPinTemp = HAL_GPIO_ReadPin(UART5RX_GPIO_Port,UART5RX_Pin);
          ReadPinDataTempLast = ReadPinTemp;
          ReadPinTemp = HAL_GPIO_ReadPin(UART5TX_GPIO_Port,UART5TX_Pin);
          ReadPinDataTempLast |= ReadPinTemp << 1;
          ReadPinTemp = HAL_GPIO_ReadPin(UART6RX_GPIO_Port,UART6RX_Pin);
          ReadPinDataTempLast |= ReadPinTemp << 2;
          ReadPinTemp = HAL_GPIO_ReadPin(UART6TX_GPIO_Port,UART6TX_Pin);
          ReadPinDataTempLast |= ReadPinTemp << 3;
          ReadPinTemp = HAL_GPIO_ReadPin(UART7RX_GPIO_Port,UART7RX_Pin);
          ReadPinDataTempLast |= ReadPinTemp << 4;
          ReadPinTemp = HAL_GPIO_ReadPin(UART7TX_GPIO_Port,UART7TX_Pin);
          ReadPinDataTempLast |= ReadPinTemp << 5;
          
          CanDate.SendBitsDataPre = ReadPinDataTemp;
          CanDate.SendBitsDataLater = ReadPinDataTempLast;
     
          CanDate.Buf[0] = 0xa0;
          CanDate.Buf[1] = 0x0a;
          CanDate.Buf[2] = 8;
          CanDate.Buf[3] = CanDate.SendBitsDataPre;
          CanDate.Buf[4] = CanDate.SendBitsDataLater;
          CanDate.Buf[5] = (u8)0;
          CanDate.Buf[6] = (u8)0;  

          for (u8 i = 0; i < 7; i++) {
              checkdate += CanDate.Buf[i];
          }
          CanDate.Buf[7] = (u8)(checkdate & 0xff);
        
          BspCanTxData(&hcan1, Master_stid >> 5, CanDate.Buf, 8);
          
          time_delay++;
          if(time_delay > 10)
          {
              time_delay = 0;
              HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
          }
          osDelay(50);
    }

         
}
/*********END OF FILE****/


