/*
******************************************************************************
*@文件名字  : BspCan.c
*@描述	    : Can BSP 层代码，RTOS版本
*       
******************************************************************************
*@作者		: 丁志铖
*@强制性规定: 移植时只可以在 USER CODE BEGIN  USER CODE END  之间删改代码!!!
*@版本		: V1.00 2016/5/11 初始版本(STM32F405测试)
*             V1.01 2016/8/4  邸兴超 修改cube生成的bug，完善can的处理流程

******************************************************************************
*/

#include "BspCan.h"

BspCanProcessStruct g_CanProcess;

/** 重构库函数的HAL_CAN_Transmit_IT
  * @brief  Initiates and transmits a CAN frame message.
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.  
  * @retval HAL status
  */
static HAL_StatusTypeDef HAL_CAN_Transmit_IT(CAN_HandleTypeDef* hcan)
{
  uint32_t  transmitmailbox = 5;
  uint32_t tmp = 0;
  
  /* Check the parameters */
  assert_param(IS_CAN_IDTYPE(hcan->pTxMsg->IDE));
  assert_param(IS_CAN_RTR(hcan->pTxMsg->RTR));
  assert_param(IS_CAN_DLC(hcan->pTxMsg->DLC));
  
  tmp = hcan->State;
  if(1)//((tmp == HAL_CAN_STATE_READY) || (tmp == HAL_CAN_STATE_BUSY_RX))
  {
  /*屏蔽if判断,否则即是使用计数型信号量，也会只是用一个邮箱，
    因为它要等待发送完成后才能再发送 
    ,如果安装请求顺序发送数据，一定要将TXFP设置为ENABLE*/
    /* Process Locked */
    __HAL_LOCK(hcan);
    
    /* Select one empty transmit mailbox */
    if((hcan->Instance->TSR&CAN_TSR_TME0) == CAN_TSR_TME0)
    {
      transmitmailbox = 0;
    }
    else if((hcan->Instance->TSR&CAN_TSR_TME1) == CAN_TSR_TME1)
    {
      transmitmailbox = 1;
    }
    else if((hcan->Instance->TSR&CAN_TSR_TME2) == CAN_TSR_TME2)
    {
      transmitmailbox = 2;
    }
    else
    {
      transmitmailbox = CAN_TXSTATUS_NOMAILBOX;
    }

    if(transmitmailbox != CAN_TXSTATUS_NOMAILBOX)
    {
      /* Set up the Id */
      hcan->Instance->sTxMailBox[transmitmailbox].TIR &= CAN_TI0R_TXRQ;
      if(hcan->pTxMsg->IDE == CAN_ID_STD)
      {
        assert_param(IS_CAN_STDID(hcan->pTxMsg->StdId));  
        hcan->Instance->sTxMailBox[transmitmailbox].TIR |= ((hcan->pTxMsg->StdId << 21) | \
                                                  hcan->pTxMsg->RTR);
      }
      else
      {
        assert_param(IS_CAN_EXTID(hcan->pTxMsg->ExtId));
        hcan->Instance->sTxMailBox[transmitmailbox].TIR |= ((hcan->pTxMsg->ExtId << 3) | \
                                                  hcan->pTxMsg->IDE | \
                                                  hcan->pTxMsg->RTR);
      }
    
      /* Set up the DLC */
      hcan->pTxMsg->DLC &= (uint8_t)0x0000000F;
      hcan->Instance->sTxMailBox[transmitmailbox].TDTR &= (uint32_t)0xFFFFFFF0;
      hcan->Instance->sTxMailBox[transmitmailbox].TDTR |= hcan->pTxMsg->DLC;

      /* Set up the data field */
      hcan->Instance->sTxMailBox[transmitmailbox].TDLR = (((uint32_t)hcan->pTxMsg->Data[3] << 24) | 
                                             ((uint32_t)hcan->pTxMsg->Data[2] << 16) |
                                             ((uint32_t)hcan->pTxMsg->Data[1] << 8) | 
                                             ((uint32_t)hcan->pTxMsg->Data[0]));
      hcan->Instance->sTxMailBox[transmitmailbox].TDHR = (((uint32_t)hcan->pTxMsg->Data[7] << 24) | 
                                             ((uint32_t)hcan->pTxMsg->Data[6] << 16) |
                                             ((uint32_t)hcan->pTxMsg->Data[5] << 8) |
                                             ((uint32_t)hcan->pTxMsg->Data[4]));
    
      if(hcan->State == HAL_CAN_STATE_BUSY_RX) 
      {
        /* Change CAN state */
        hcan->State = HAL_CAN_STATE_BUSY_TX_RX;
      }
      else
      {
        /* Change CAN state */
        hcan->State = HAL_CAN_STATE_BUSY_TX;
      }
      
      /* Set CAN error code to none */
      hcan->ErrorCode = HAL_CAN_ERROR_NONE;
      
      /* Process Unlocked */
      __HAL_UNLOCK(hcan);

      #if 1
      /* Enable Error warning Interrupt */
      __HAL_CAN_ENABLE_IT(hcan, CAN_IT_EWG);
      
     /* Enable Error passive Interrupt */
      __HAL_CAN_ENABLE_IT(hcan, CAN_IT_EPV);
      
      /* Enable Bus-off Interrupt */
      __HAL_CAN_ENABLE_IT(hcan, CAN_IT_BOF);
      
      /* Enable Last error code Interrupt */
      __HAL_CAN_ENABLE_IT(hcan, CAN_IT_LEC);
      
      /* Enable Error Interrupt */
      __HAL_CAN_ENABLE_IT(hcan, CAN_IT_ERR);
      #endif
      /* Request transmission */
      hcan->Instance->sTxMailBox[transmitmailbox].TIR |= CAN_TI0R_TXRQ;

      /* Enable Transmit mailbox empty Interrupt */
      __HAL_CAN_ENABLE_IT(hcan, CAN_IT_TME);
      
      
    }
    else
    {
        __HAL_UNLOCK(hcan);
        return HAL_BUSY;
    }
  }
  else
  {
    return HAL_BUSY;
  }
  
  return HAL_OK;
}

#ifdef USE_CAN1

/******************************************************************************************
*函数名称：CAN1_Init
*
*入口参数：StdId
*
*出口参数：无
*
*功能说明：CAN 相关配置
*******************************************************************************************/
void BspCan1Init(void)
{
	HAL_StatusTypeDef result = HAL_OK;	
  CAN_FilterConfTypeDef  sFilterConfig;
  static CanTxMsgTypeDef TxMessage;
  static CanRxMsgTypeDef RxMessage;	

	g_CanProcess.Can1ReadData = NULL;
	
  osSemaphoreDef(CountSemCan1);
  g_CanProcess.CountSemCan1MailBoxNum = osUserSemaphoreCreate(osSemaphore(CountSemCan1), 3, 3);

	//分配MsgBox空间
	hcan1.pTxMsg = &TxMessage;
  hcan1.pRxMsg = &RxMessage;
	
  /* Configure the CAN Filter */
  sFilterConfig.FilterNumber         = 0;
  sFilterConfig.FilterMode 				   = CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale 				 = CAN_FILTERSCALE_32BIT;
  //sFilterConfig.FilterIdHigh 				 = 0x0000; 
	sFilterConfig.FilterIdHigh 				 = 0x0500 << 5; 
  sFilterConfig.FilterIdLow 				 = 0x0000;
  //sFilterConfig.FilterMaskIdHigh 		 = 0x0000; 
	sFilterConfig.FilterMaskIdHigh 		 = 0x0700 << 5;  
  sFilterConfig.FilterMaskIdLow 		 = 0x0000;
  sFilterConfig.FilterFIFOAssignment = CAN_FIFO1;
  sFilterConfig.FilterActivation     = ENABLE;
  sFilterConfig.BankNumber           = 14;
  
  result = HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig);
	if( result != HAL_OK )
  {
#ifdef ERR_PRINT
		SEGGER_RTT_printf(0,"HAL_CAN1_ConfigFilter AT %d\r\n", result);
#endif
	}	
	
//	result = HAL_CAN_Receive_IT(&hcan1, CAN_FIFO0);
//	if( result != HAL_OK )
//  {
//#ifdef ERR_PRINT
//		SEGGER_RTT_printf(0,"HAL_CAN1_Receive_IT AT %d\r\n", result);
//#endif
//	}	
		
}
#endif

#ifdef USE_CAN2

/******************************************************************************************
*函数名称：CAN2_Init
*
*入口参数：StdId
*
*出口参数：无
*
*功能说明：CAN 相关配置
*******************************************************************************************/
void BspCan2Init(void)
{
	HAL_StatusTypeDef result = HAL_OK;	
  CAN_FilterConfTypeDef  sFilterConfig;
  static CanTxMsgTypeDef TxMessage;
  static CanRxMsgTypeDef RxMessage;	

	g_CanProcess.Can2ReadData = NULL;
	osSemaphoreDef(CountSemCan2);
  g_CanProcess.CountSemCan2MailBoxNum = osUserSemaphoreCreate(osSemaphore(CountSemCan2), 3, 3);
	//分配MsgBox空间
	hcan2.pTxMsg = &TxMessage;
  hcan2.pRxMsg = &RxMessage;
	
  /* Configure the CAN Filter */
  sFilterConfig.FilterNumber         = 14;  //CAN2 从14开始
  sFilterConfig.FilterMode 				   = CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale 				 = CAN_FILTERSCALE_32BIT;
//  sFilterConfig.FilterIdHigh 				 = 0x0000; 
	sFilterConfig.FilterIdHigh 				 = 0x0000;     //0x0500 << 5; //0x8080 << 5;
  sFilterConfig.FilterIdLow 				 = 0x0000;
//  sFilterConfig.FilterMaskIdHigh 		 = 0x0000; 
	sFilterConfig.FilterMaskIdHigh 		 = 0x0000;     // << 5 ;//0x0700 << 5;  //0xffff << 5;
  sFilterConfig.FilterMaskIdLow 		 = 0x0000;
  sFilterConfig.FilterFIFOAssignment = CAN_FIFO1;
  sFilterConfig.FilterActivation     = ENABLE;
  sFilterConfig.BankNumber           = 14;
  
  result = HAL_CAN_ConfigFilter(&hcan2, &sFilterConfig);
	if( result != HAL_OK )
  {

#ifdef ERR_PRINT
		SEGGER_RTT_printf(0,"HAL_CAN2_ConfigFilter AT %d\r\n", result);
#endif

	}	
	
//	result = HAL_CAN_Receive_IT(&hcan2, CAN_FIFO1);
//  __HAL_CAN_ENABLE_IT(&hcan2, CAN_IT_FMP1);
//	if( result != HAL_OK )
// {
//#ifdef ERR_PRINT
//		SEGGER_RTT_printf(0,"HAL_CAN2_Receive_IT AT %d\r\n", result);
//#endif
//	}	
		
}
#endif

#ifdef USE_CAN3

/******************************************************************************************
*函数名称：CAN2_Init
*
*入口参数：StdId
*
*出口参数：无
*
*功能说明：CAN 相关配置
*******************************************************************************************/
void BspCan3Init(void)
{
	HAL_StatusTypeDef result = HAL_OK;	
  CAN_FilterConfTypeDef  sFilterConfig;
  static CanTxMsgTypeDef TxMessage;
  static CanRxMsgTypeDef RxMessage;	

	g_CanProcess.Can3ReadData = NULL;
	osSemaphoreDef(CountSemCan3);
  g_CanProcess.CountSemCan3MailBoxNum = osUserSemaphoreCreate(osSemaphore(CountSemCan3), 3, 3);
	//分配MsgBox空间
	hcan3.pTxMsg = &TxMessage;
  hcan3.pRxMsg = &RxMessage;
	
  /* Configure the CAN Filter */
  sFilterConfig.FilterNumber         = 0;  //CAN2 从14开始
  sFilterConfig.FilterMode 				   = CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale 				 = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh 				 = 0x8080;    
  sFilterConfig.FilterIdLow 				 = 0x0000;
	sFilterConfig.FilterMaskIdHigh 		 = 0xffff; 
  sFilterConfig.FilterMaskIdLow 		 = 0x0000;
  sFilterConfig.FilterFIFOAssignment = CAN_FIFO1;
  sFilterConfig.FilterActivation     = ENABLE;
  sFilterConfig.BankNumber           = 14;
  
  result = HAL_CAN_ConfigFilter(&hcan3, &sFilterConfig);
	if( result != HAL_OK )
  {

#ifdef ERR_PRINT
		SEGGER_RTT_printf(0,"HAL_CAN2_ConfigFilter AT %d\r\n", result);
#endif

	}	
	//  __HAL_CAN_ENABLE_IT(&hcan3, CAN_IT_FMP1);
//	result = HAL_CAN_Receive_IT(&hcan3, CAN_FIFO1);
//	if( result != HAL_OK )
//  {
//#ifdef ERR_PRINT
//		SEGGER_RTT_printf(0,"HAL_CAN2_Receive_IT AT %d\r\n", result);
//#endif
//	}	
		
}
#endif

void BspCanInit(void)
{
#ifdef USE_CAN1
  BspCan1Init();
#endif

#ifdef USE_CAN2
  BspCan2Init();
#endif
  
#ifdef USE_CAN3
  BspCan3Init();
#endif
}

/******************************************************************************************
*函数名称：BspCanWriteData(uint32_t StdId, uint8_t *buf, uint8_t len, uint32_t Timeout)
*
*入口参数：StdId
*
*出口参数：无
*
*功能说明：CAN 发送数据 最大8个一次
*******************************************************************************************/
HAL_StatusTypeDef BspCanWriteData(CAN_HandleTypeDef *hcan, u32 StdId, u8 *buf, u8 len)
{
	int i = 0;
	HAL_StatusTypeDef result = HAL_OK;

	/* transmit */
	
//  if(CAN2 == hcan->Instance)
//  {    
//      
//      hcan->pTxMsg->ExtId = StdId; 
//      hcan->pTxMsg->IDE   = CAN_ID_EXT;  
//  }	
//  else
//  {
      hcan->pTxMsg->StdId = StdId; 
      hcan->pTxMsg->IDE   = CAN_ID_STD;
 // }
  
  hcan->pTxMsg->RTR   = CAN_RTR_DATA; 
	
	if( len > 8 )
	{
		hcan->pTxMsg->DLC = 8; 
	}
	else
	{
		hcan->pTxMsg->DLC = len; 
	}
	
	for( i = 0; i < hcan->pTxMsg->DLC; i++ )
	{
		hcan->pTxMsg->Data[i] = buf[i];
	}
	
	//result = HAL_CAN_Transmit(hcan, 100);
	result = HAL_CAN_Transmit_IT(hcan);
	if( result != HAL_OK )
    {
#ifdef ERR_PRINT
		SEGGER_RTT_printf(0,"HAL_CAN_Transmit_IT ERROR at %d\r\n", result);
#endif

	}	
	
	return result;
}


u8 BspCanTxData( CAN_HandleTypeDef *hcan, u32 CanId, u8* buf, u32 size )
{
    u8* BufPtr = buf;
    u32 SizeTemp = size;
    u8 len;
    osSemaphoreId* CountSemPtr;

    if(CAN1 == hcan->Instance)
    {
        CountSemPtr = &(g_CanProcess.CountSemCan1MailBoxNum);
    }
    else if(CAN2 == hcan->Instance)
    {
        CountSemPtr = &(g_CanProcess.CountSemCan2MailBoxNum);
    }
    else if(CAN3 == hcan->Instance)
    {
        CountSemPtr = &(g_CanProcess.CountSemCan3MailBoxNum);
    }
    else
    {
        BspErr();
        return FALSE;
    }
  
    while(SizeTemp > 0)//; SizeTemp -= 8, BufPtr += 8 )
    {
        if( osOK != osSemaphoreWait(*CountSemPtr,100) )//100ms还没空闲的邮箱
        {
            BspErr();
            return FALSE;
        }
        len = (SizeTemp > 8 ) ? 8 : SizeTemp;
        if( HAL_OK != BspCanWriteData( hcan, CanId, BufPtr, len) )
        {
            BspErr();
            return FALSE;
        }
        SizeTemp -= len;
        BufPtr += len;
    }

    return TRUE;
    
}



//call back

/**
  * @brief  Transmission  complete callback in non blocking mode 
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
void HAL_CAN_TxCpltCallback(CAN_HandleTypeDef* hcan)
{
  if( hcan->Instance == CAN1 )
  {
    //SEGGER_RTT_printf(0,"Can1TxCplt\n");
    osSemaphoreRelease(g_CanProcess.CountSemCan1MailBoxNum);
  }
  else if( hcan->Instance == CAN2 )
  {
    //SEGGER_RTT_printf(0,"Can2TxCplt\n");
    osSemaphoreRelease(g_CanProcess.CountSemCan2MailBoxNum);
  }
    else if( hcan->Instance == CAN3 )
  {
    //SEGGER_RTT_printf(0,"Can2TxCplt\n");
    osSemaphoreRelease(g_CanProcess.CountSemCan3MailBoxNum);
  }
}

/**
  * @brief  Transmission  complete callback in non blocking mode 
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{
//  if( hcan->Instance == CAN1 )
//  {
//  	//获得接收数据
//  	if( NULL != g_CanProcess.Can1ReadData )
//  	{
//  		( *(g_CanProcess.Can1ReadData) )(hcan->pRxMsg->Data, hcan->pRxMsg->DLC);
//  	}
//  		
////  	if( HAL_OK != HAL_CAN_Receive_IT(hcan, CAN_FIFO0)  )
////    {
////  		SEGGER_RTT_printf(0,"CAN1_RxCpltCallback Error %04x \r\n",hcan->State);
////  	}
//  }
//  else 
  if( hcan->Instance == CAN2 )
  {
   // ComBMSSaveData(hcan->pRxMsg->Data, hcan->pRxMsg->DLC);
    //获得接收数据
  	if( NULL != g_CanProcess.Can2ReadData )
  	{
  		( *(g_CanProcess.Can2ReadData) )(hcan->pRxMsg->Data, hcan->pRxMsg->DLC);
  	}
  		
//  	if( HAL_OK != HAL_CAN_Receive_IT(hcan, CAN_FIFO0))
//    {
//  		SEGGER_RTT_printf(0,"CAN2_RxCpltCallback Error %04x\n",hcan->State);
//  	}
  }
  else if( hcan->Instance == CAN3 )
  {
    //获得接收数据
  	if( NULL != g_CanProcess.Can3ReadData )
  	{
  		( *(g_CanProcess.Can3ReadData) )(hcan->pRxMsg->Data, hcan->pRxMsg->DLC);
  	}
  		
//  	if( HAL_OK != HAL_CAN_Receive_IT(hcan, CAN_FIFO0))
//    {
//  		SEGGER_RTT_printf(0,"CAN2_RxCpltCallback Error %04x\n",hcan->State);
//  	}
  }
}

/**
  * @brief  Error CAN callback.
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
  if( hcan->Instance == CAN1 )
  {
    //CAN1_ErrorCallback(hcan);
    BspErr();
  }
  else if( hcan->Instance == CAN2 )
  {
    //CAN2_ErrorCallback(hcan);
    BspErr();
  }
   else if( hcan->Instance == CAN3 )
  {
    //CAN2_ErrorCallback(hcan);
    BspErr();
  }
}

/* 
中断调用处理函数 放在CAN_RX0_IRQHandler中断函数中 
调用后加return,不要调用Cube生成的中断处理函数
*/
void BspCanReceiveIrqHandler(CAN_HandleTypeDef* hcan, uint8_t FIFONumber)
{
  
  /* Get the Id */
  hcan->pRxMsg->IDE = (uint8_t)0x04 & hcan->Instance->sFIFOMailBox[FIFONumber].RIR;
  if (hcan->pRxMsg->IDE == CAN_ID_STD)
  {
    hcan->pRxMsg->StdId = (uint32_t)0x000007FF & (hcan->Instance->sFIFOMailBox[FIFONumber].RIR >> 21);
  }
  else
  {
    hcan->pRxMsg->ExtId = (uint32_t)0x1FFFFFFF & (hcan->Instance->sFIFOMailBox[FIFONumber].RIR >> 3);
  }
  
  hcan->pRxMsg->RTR = (uint8_t)0x02 & hcan->Instance->sFIFOMailBox[FIFONumber].RIR;
  /* Get the DLC */
  hcan->pRxMsg->DLC = (uint8_t)0x0F & hcan->Instance->sFIFOMailBox[FIFONumber].RDTR;
  /* Get the FMI */
  hcan->pRxMsg->FMI = (uint8_t)0xFF & (hcan->Instance->sFIFOMailBox[FIFONumber].RDTR >> 8);
  /* Get the data field */
  hcan->pRxMsg->Data[0] = (uint8_t)0xFF & hcan->Instance->sFIFOMailBox[FIFONumber].RDLR;
  hcan->pRxMsg->Data[1] = (uint8_t)0xFF & (hcan->Instance->sFIFOMailBox[FIFONumber].RDLR >> 8);
  hcan->pRxMsg->Data[2] = (uint8_t)0xFF & (hcan->Instance->sFIFOMailBox[FIFONumber].RDLR >> 16);
  hcan->pRxMsg->Data[3] = (uint8_t)0xFF & (hcan->Instance->sFIFOMailBox[FIFONumber].RDLR >> 24);
  hcan->pRxMsg->Data[4] = (uint8_t)0xFF & hcan->Instance->sFIFOMailBox[FIFONumber].RDHR;
  hcan->pRxMsg->Data[5] = (uint8_t)0xFF & (hcan->Instance->sFIFOMailBox[FIFONumber].RDHR >> 8);
  hcan->pRxMsg->Data[6] = (uint8_t)0xFF & (hcan->Instance->sFIFOMailBox[FIFONumber].RDHR >> 16);
  hcan->pRxMsg->Data[7] = (uint8_t)0xFF & (hcan->Instance->sFIFOMailBox[FIFONumber].RDHR >> 24);
  /* Release the FIFO */
  /* Release FIFO0 */
  if (FIFONumber == CAN_FIFO0)
  {
    __HAL_CAN_FIFO_RELEASE(hcan, CAN_FIFO0);
  }
  /* Release FIFO1 */
  else /* FIFONumber == CAN_FIFO1 */
  {
    __HAL_CAN_FIFO_RELEASE(hcan, CAN_FIFO1);
  }
  
  //  __HAL_CAN_CLEAR_FLAG(&hcan2,CAN_FLAG_FMP1);
 // __HAL_CAN_CLEAR_FLAG(&hcan2,CAN_IT_FMP1);
 // __HAL_CAN_CLEAR_FLAG(&hcan2,CAN_FLAG_FOV1);
 // __HAL_CAN_CLEAR_FLAG(&hcan2,CAN_FLAG_WKU);
 /// __HAL_CAN_CLEAR_FLAG(&hcan2,CAN_FLAG_SLAK);
  HAL_CAN_RxCpltCallback(hcan);
}

/* 
中断调用处理函数 放在CAN_TX_IRQHandler中断函数中 
调用后加return,不要调用Cube生成的中断处理函数
*/
void BspCanTransmitIrqHandler(CAN_HandleTypeDef* hcan)
{
    uint32_t tmp1 = 0, tmp2 = 0, tmp3 = 0;
    
    /* Check End of transmission flag */
    if(__HAL_CAN_GET_IT_SOURCE(hcan, CAN_IT_TME))
    {
      tmp1 = __HAL_CAN_TRANSMIT_STATUS(hcan, CAN_TXMAILBOX_0);
      tmp2 = __HAL_CAN_TRANSMIT_STATUS(hcan, CAN_TXMAILBOX_1);
      tmp3 = __HAL_CAN_TRANSMIT_STATUS(hcan, CAN_TXMAILBOX_2);
      if(tmp1 || tmp2 || tmp3)  
      {
        /* Call transmit function */
        __HAL_CAN_DISABLE_IT(hcan, CAN_IT_TME);
      
        if(hcan->State == HAL_CAN_STATE_BUSY_TX_RX) 
        {
          /* Change CAN state */
          hcan->State = HAL_CAN_STATE_BUSY_RX;
        }
        else
        {
          /* Change CAN state */
          hcan->State = HAL_CAN_STATE_READY;
        }
        
        /* Transmission complete callback */ 
        HAL_CAN_TxCpltCallback(hcan);
      }
    }
}
/*********END OF FILE****/



