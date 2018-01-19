
/**
* @file     
* @brief    
* @version  
* @author   
* @date     2016-6-19
* @note     
*/

/*============================ INCLUDES ======================================*/
 
#include "trace.h"

#if TRACE

#include <assert.h>
#include <stdarg.h> 
 
/*============================ MACROS ========================================*/

#define DBG_MODE_WINFILE 0
#define DBG_MODE_JTAGRTT 1
#define DBG_MODE_STMUART 0
#define DBG_MODE_TCPNET  0
#define DBG_MODE_STMUSB  0

#if DBG_MODE_WINFILE
#pragma warning(disable:4996) /* disable warning about strcpy() & strcpy_s()*/
#include <stdio.h>            /* fopen()... */
static FILE *fp_trace=NULL;   /* file pointer of trace */
#endif

#if DBG_MODE_JTAGRTT
#include "config.h"

#include "SEGGER_RTT.h"
#define RTT_TERMINAL 0
#endif

#if DBG_MODE_STMUART
//...TO DO;         
#endif

#if DBG_MODE_TCPNET
//...TO DO;
#endif

#if DBG_MODE_STMUSB
#include "BspUSB.h"  
#endif 
 
/*============================ TYPES =========================================*/
 
 
 
/*============================ GLOBAL VARIABLES ==============================*/
 
char tracebuf[TRACE_BUF_LENGTH]={0}; /* 4K bytes buffer */
int  trace_buf_len=0; 
 
/*============================ LOCAL VARIABLES ===============================*/
 
static int level_trace=3;            /* level of trace */ 
 
/*============================ IMPLEMENTATION ================================*/

void traceopen_(const char *file)
{    
#if DBG_MODE_WINFILE 
  if (!*file || !(fp_trace=fopen(file,"w"))) 
  {
    fp_trace=stderr;
  }    
#endif    
}

void traceclose_(void)
{
#if DBG_MODE_WINFILE
  if (fp_trace && fp_trace!=stderr) 
  {
    fclose(fp_trace);
  }
  fp_trace=NULL;
#endif    
}

void tracelevel_(int level)
{
  level_trace=level;
}

void trace_(int level, const char *format, ...)
{
  int len=0;  
  va_list ap;  
    
  if (level > level_trace) 
  {
    return;    
  }  
  
  switch (level)
  {    
    case TINFO:
      tracebuf[0] = 'I';
      tracebuf[1] = 'N';
      tracebuf[2] = 'F';
      tracebuf[3] = 'O';
      break;

    case TNOTE:
      tracebuf[0] = 'N';
      tracebuf[1] = 'O';
      tracebuf[2] = 'T';
      tracebuf[3] = 'E';
      break;

    case TWARN:
      tracebuf[0] = 'W';
      tracebuf[1] = 'A';
      tracebuf[2] = 'R';
      tracebuf[3] = 'N';
      break;
      
    default:
    case TEROR:
      tracebuf[0] = 'E';
      tracebuf[1] = 'R';
      tracebuf[2] = 'O';
      tracebuf[3] = 'R';
      break;
  }
  
  va_start(ap, format); 
  len = vsprintf(tracebuf + trace_buf_len, format, ap); 
  va_end(ap);

  len += trace_buf_len;
  
  tracebuf[len++] = '\r';
  tracebuf[len++] = '\n';
  tracebuf[len] = 0;
  
#if DBG_MODE_WINFILE
  assert(fp_trace != NULL); /* if (!fp_trace) return; */
  fprintf(fp_trace, "%s", tracebuf);
  fflush(fp_trace);
#endif

#if DBG_MODE_JTAGRTT
  SEGGER_RTT_printf(RTT_TERMINAL, "%s", tracebuf); /* always use terminal 0 */
#endif

#if DBG_MODE_STMUART
  //...TO DO;
#endif  

#if DBG_MODE_TCPNET
  //...TO DO; 
#endif  

#if DBG_MODE_STMUSB
  BspUSBSendData((u8 *)tracebuf, len);
#endif
}

void traceb_(int level, const unsigned char *p, int n)
{
  int i,len=0;
    
  if (level>level_trace) 
  {
    return;    
  }
  
  for (i=0;i<n;i++) 
  {
    len+=sprintf(tracebuf+len,"%02x,",*p++);
  }
  len+=sprintf(tracebuf+len,"\r\n");
  
#if DBG_MODE_WINFILE  
  assert(NULL != fp_trace);
  fprintf(fp_trace, "%s", tracebuf);
  fflush(fp_trace);
#endif

#if DBG_MODE_JTAGRTT
  SEGGER_RTT_printf(RTT_TERMINAL,"%s",tracebuf); 
#endif

#if DBG_MODE_STMUART
  //...TO DO
#endif

#if DBG_MODE_TCPNET
  //...TO DO
#endif

#if DBG_MODE_STMUSB
  BspUSBSendData((u8 *)tracebuf, len);
#endif
}

#endif // #if TRACE

