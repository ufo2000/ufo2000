#if !defined (EXCHNDL_H)
#define EXCHNDL_H

#ifdef WIN32
#ifdef __cplusplus
extern "C"
#endif
char *GenerateExceptionReport(PEXCEPTION_POINTERS pExceptionInfo);
#endif

#endif
