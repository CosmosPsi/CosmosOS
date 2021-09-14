/****************************************************************
        Kernel层日志头文件KrlLog.h
*****************************************************************
                彭东 
****************************************************************/

#ifndef _KRLLOGHEAD
#define _KRLLOGHEAD

typedef __builtin_va_list VAList;
#define VA_START(ap,np) __builtin_va_start(ap,np)
#define VA_END(ap) __builtin_va_end(ap)
#define VA_ARG(ap,ty) __builtin_va_arg(ap,ty)

private Char* FmtNumber(Char* str, UInt n, SInt base);
private Char* FmtStrCopy(Char* buf, Char* str);
private void FmtCore(Char* buf, const Char* fmt, VAList args);
public void KrlPrint(const Char* fmt, ...);
public void KrlLog(UInt level, const Char* fmt, ...);
public void KrlErrorCrashDead(const Char* fmt, ...);
public Bool KrlLogInit();

#endif