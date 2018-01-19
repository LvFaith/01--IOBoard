/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  *
  * Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */     
#include "main.h"
/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
osThreadId NameGPIOInHandle;
osThreadId NameCanRxHandle;
osThreadId NameCanTxHandle;
osThreadId NameReserveHandle;

/* USER CODE BEGIN Variables */
osMessageQId myQueue_GCSHandle;
osMessageQId myQueue_GCS_SECDHandle;
/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void GPIOInDaemon(void const * argument);
void CanRxDaemon(void const * argument);
void CanTxDaemon(void const * argument);
void ReserveDaemon(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/* Hook prototypes */

/* Init FreeRTOS */

void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of NameGPIOIn */
  osThreadDef(NameGPIOIn, GPIOInDaemon, osPriorityNormal, 0, 128);
  NameGPIOInHandle = osThreadCreate(osThread(NameGPIOIn), NULL);

  /* definition and creation of NameCanRx */
  osThreadDef(NameCanRx, CanRxDaemon, osPriorityNormal, 0, 128);
  NameCanRxHandle = osThreadCreate(osThread(NameCanRx), NULL);

  /* definition and creation of NameCanTx */
  osThreadDef(NameCanTx, CanTxDaemon, osPriorityNormal, 0, 128);
  NameCanTxHandle = osThreadCreate(osThread(NameCanTx), NULL);

  /* definition and creation of NameReserve */
  osThreadDef(NameReserve, ReserveDaemon, osPriorityNormal, 0, 128);
  NameReserveHandle = osThreadCreate(osThread(NameReserve), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  osMessageQDef(myQueue_GCS, 512, uint32_t);
  myQueue_GCSHandle = osMessageCreate(osMessageQ(myQueue_GCS), NULL);
  osMessageQDef(myQueue_GCS_SECD, 512, uint32_t);
  myQueue_GCS_SECDHandle = osMessageCreate(osMessageQ(myQueue_GCS_SECD), NULL);
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}

/* GPIOInDaemon function */
__weak void GPIOInDaemon(void const * argument)
{

  /* USER CODE BEGIN GPIOInDaemon */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END GPIOInDaemon */
}

/* CanRxDaemon function */
__weak void CanRxDaemon(void const * argument)
{
  /* USER CODE BEGIN CanRxDaemon */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END CanRxDaemon */
}

/* CanTxDaemon function */
__weak void CanTxDaemon(void const * argument)
{
  /* USER CODE BEGIN CanTxDaemon */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END CanTxDaemon */
}

/* ReserveDaemon function */
__weak void ReserveDaemon(void const * argument)
{
  /* USER CODE BEGIN ReserveDaemon */
  /* Infinite loop */
  for(;;)
  {
    osDelay(200);
//    DrvGcsSendData(testsedbuf,10);
  }
  /* USER CODE END ReserveDaemon */
}

/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
