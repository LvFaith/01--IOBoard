
#ifndef TRACE_H
#define TRACE_H
#ifdef __cplusplus
 extern "C" {
#endif 

#include <stdint.h>
     
#define TRACE 1

#if TRACE

#define TEROR 0
#define TWARN 1
#define TNOTE 2
#define TINFO 3

#define TRACE_BUF_LENGTH  (1024*4)

extern int  trace_buf_len;
extern char tracebuf[TRACE_BUF_LENGTH];

extern void traceopen_(const char *file);
extern void traceclose_(void);
extern void tracelevel_(int level);
extern void trace_(int level, const char *format, ...);
extern void traceb_(int level, const unsigned char *p, int n);

#define traceopen  traceopen_
#define traceclose traceclose_
#define tracelevel tracelevel_
#define trace      trace_buf_len = sprintf(tracebuf, "HEAD\t");trace_
#define traceb     traceb_

#else

#define traceopen(...)
#define traceclose(...)
#define tracelevel(...)
#define trace(...)
#define traceb(...)

#endif // #if TRACE

#ifdef __cplusplus
}
#endif

#endif // #ifndef TRACE_H

