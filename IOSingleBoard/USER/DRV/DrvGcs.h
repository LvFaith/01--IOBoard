
/**
* @file     
* @brief    
* @version  
* @author   
* @date     2016-6-22
* @note     
*/

#ifndef DRV_GCS_H__
#define DRV_GCS_H__
 
/*============================ INCLUDES ======================================*/
 
#include "Config.h"
#include "BspUart.h" 
#include "CommonStream.h"
 
/*============================ MACROS ========================================*/
 

 
/*============================ TYPES =========================================*/
 

 
/*============================ GLOBAL VARIABLES ==============================*/

/*============================ PROTOTYPES ====================================*/
extern void DrvGcsSendData(u8* buf, u16 size);

extern void ComGcsNuaaReceiveData_Queue(void);

#endif

