/*
******************************************************************************
*@�ļ�����  : Config.h
*@����	    : ��Ƭ�������ļ�
******************************************************************************
*@����			: ��־��
*@ǿ���Թ涨: ��ֲʱֻ������ USER CODE BEGIN  USER CODE END  ֮��ɾ�Ĵ���!!!
*@�汾			: V0.01      2016/8/23  ��ʼ�汾
******************************************************************************
*/

#ifndef _CONFIG_H_
#define _CONFIG_H_


#ifdef __cplusplus
 extern "C" {
#endif 


#include <stdint.h>
#include <string.h>

/*****************ѡ��STM32��Ƭ����ϵ��*********************/
/* USER CODE BEGIN 0*/

#define  STM32F4 //STM32F0 STM32F1 STM32F2 STM32F3 STM32F4 STM32F7

/* USER CODE END   0*/


/*****************�Ƿ�֧��RTOS*********************/
/* USER CODE BEGIN 1*/

#define  SUPPORT_RTOS (1)         //1:֧�� 0:��֧��

#define  SUPPORT_PROTO_THREAD (0) //1:֧�� 0:��֧��

/* USER CODE END   1*/

/*****************�Ƿ�֧��RTT��ӡ*********************/
/* USER CODE BEGIN 2*/

#define  SUPPORT_RTT (1) //1:֧�� 0:��֧��

/* USER CODE END   2*/



#ifdef STM32F0
	#include "stm32f0xx_hal.h"
#endif

#ifdef STM32F1
	#include "stm32f1xx_hal.h"
	#define UID1 ( *(volatile uint32_t *)0x1FFFF7E8 )//F10xxx F100xx�ο��ֲ���֤
	#define UID2 ( *(volatile uint32_t *)0x1FFFF7EC )
	#define UID3 ( *(volatile uint32_t *)0x1FFFF7F0 )
#endif

#ifdef STM32F2
	#include "stm32f2xx_hal.h"
	#define UID1 ( *(volatile uint32_t *)0x1FFF7A10 )//F2�ο��ֲ���֤ͨ��
	#define UID2 ( *(volatile uint32_t *)0x1FFF7A14 )
	#define UID3 ( *(volatile uint32_t *)0x1FFF7A18 )
#endif

#ifdef STM32F3
	#include "stm32f3xx_hal.h"
	#define UID1 ( *(volatile uint32_t *)0x1FFFF7AC )//F334xx�ֲ��� ����F3�ֲ���֤ͨ��
	#define UID2 ( *(volatile uint32_t *)0x1FFFF7B0 )
	#define UID3 ( *(volatile uint32_t *)0x1FFFF7B4 )
#endif

#ifdef STM32F4
	#include "stm32f4xx_hal.h"
	#define UID1 ( *(volatile uint32_t *)0x1FFF7A10 )//F446xx�ο��ֲ���֤ͨ��
	#define UID2 ( *(volatile uint32_t *)0x1FFF7A14 )
	#define UID3 ( *(volatile uint32_t *)0x1FFF7A18 )
#endif

#ifdef STM32F7
	#include "stm32f7xx_hal.h"
  #define UID1 ( *(volatile uint32_t *)0x1FF0F420 )
	#define UID2 ( *(volatile uint32_t *)0x1FF0F424 )
	#define UID3 ( *(volatile uint32_t *)0x1FF0F428 )
#endif


#if SUPPORT_RTOS
	#include "cmsis_os.h"
	#include "CMSIS_OS_SUPPLEMENT.h"
#endif

#if SUPPORT_PROTO_THREAD
	#include "osTimeSliceCore.h"
#endif


#if SUPPORT_RTT
	#include "SEGGER_RTT.h"
#endif

//#include "UserHal.h"

typedef uint8_t 	u8;
typedef uint16_t 	u16;
typedef uint32_t 	u32;

typedef int8_t 	s8;
typedef int16_t s16;
typedef int32_t s32;

typedef volatile uint8_t 		vu8;
typedef volatile uint16_t   vu16;
typedef volatile uint32_t   vu32;

typedef volatile int8_t 		vs8;
typedef volatile int16_t    vs16;
typedef volatile int32_t    vs32;


#define U16_MSB(DATA)             ((u8)((u16)(((u16)((DATA) & (u16)(0xFF00))) >> 8)))
#define U16_LSB(DATA)             ((u8)((u16)((DATA) & (u16)(0x00FF))))

/* get fields (little-endian) ------------------------------------------------*/






#define TO_U8(p) 			((vu8)(*((vu8 *)(p))))
#define TO_U16(p) 		((vu16)(*((vu16 *)(p))))
#define TO_U32(p) 		((vu32)(*((vu32 *)(p))))

#define TO_S8(p) 			((vs8)(*((vs8 *)(p))))
#define TO_S16(p) 		((vs16)(*((vs16 *)(p))))
#define TO_S32(p) 		((vs32)(*((vs32 *)(p))))

#define BUF_TO_FLOAT(buf,x)  (memcpy(&(x), buf, 4))


#define float2buf(x, y)    memcpy(x, y, 4)
#define int2buf(x, y)      memcpy(x, y, 4)

#define U16_TO_BUF(x,buf)      (memcpy(buf, &(x), 2))

#define S16_TO_BUF(x,buf)      (memcpy(buf, &(x), 2))
#define S32_TO_BUF(x,buf)      (memcpy(buf, &(x), 4))

#define FLOAT_TO_BUF(x, buf)    (memcpy(buf, &(x), 4))

/* USER CODE BEGIN 0*/
#define B10_P1 (1.0E+1) /* B: base, P: positive */
#define B10_P2 (1.0E+2)
#define B10_P3 (1.0E+3)
#define B10_P4 (1.0E+4)
#define B10_P5 (1.0E+5)
#define B10_P6 (1.0E+6)
#define B10_P7 (1.0E+7)
#define B10_P8 (1.0E+8)
#define B10_P9 (1.0E+9)

#define B10_N1 (1.0E-1) /* B: base, N: negative */
#define B10_N2 (1.0E-2)
#define B10_N3 (1.0E-3)
#define B10_N4 (1.0E-4)
#define B10_N5 (1.0E-5)
#define B10_N6 (1.0E-6)
#define B10_N7 (1.0E-7)
#define B10_N8 (1.0E-8)
#define B10_N9 (1.0E-9)

#if (SUPPORT_RTOS && SUPPORT_TRACE)
	#define BspErr() 	if(0)												\
						{													\
							trace(TEROR, "BspErr!!");				\
							osDelay(2);										\
						}													
#else
	#define BspErr()    
#endif

/* USER CODE END 0*/


typedef enum
{
	FALSE = 0,
	TRUE,
}BOOL_ENUM;

typedef enum
{
	OFF = 0,
	ON,
}SWITCH_ENUM;


typedef struct
{
	GPIO_TypeDef* GPIOX;//10ms 
	u16	PIN;
}BSP_PIN_STRUCT;



#ifdef __cplusplus
}
#endif


#endif

/*********END OF FILE****/


