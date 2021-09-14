/****************************************************************
        Kernel层日志文件KrlLog.c
*****************************************************************
                彭东 
****************************************************************/
#include "BaseType.h"
#include "HalCPU.h"
#include "HalSync.h"
#include "HalFirmware.h"
#include "KrlLog.h"

private Char* FmtNumber(Char* str, UInt n, SInt base)
{
	register Char* p;
	Char strbuf[36];
	p = &strbuf[36];
	*--p = 0;
	if (n == 0)
	{
		*--p = '0';
	}
	else
	{
		do
		{
			*--p = "0123456789abcdef"[n % base];
		} while (n /= base);
	}
	while (*p != 0)
	{
		*str++ = *p++;
	}
	return str;
}

private Char* FmtStrCopy(Char* buf, Char* str)
{
	while(*str)
	{
		*buf = *str;
		buf++;
		str++;
	}
	return buf;
}

private void FmtCore(Char* buf, const Char* fmt, VAList args)
{
	Char* p = buf;
	while(*fmt)
	{
		if(*fmt != '%')
		{
			*p++ = *fmt++;
			continue;
		}
		fmt++;
		switch(*fmt)
		{
		case 'x':
			p = FmtNumber(p, VA_ARG(args, UInt), 16);
			fmt++;
			break;
		case 'd':
			p = FmtNumber(p, VA_ARG(args, UInt), 10);
			fmt++;
			break;
		case 's':
			p = FmtStrCopy(p, (Char*)VA_ARG(args, UInt));
			fmt++;
			break;
		default:
			break;
		}
	}
	*p = 0;
	return;
}

public void KrlPrint(const Char* fmt, ...)
{
	CPUFlg cpuflg;
	Char buf[512];
	VAList ap;
	HalCliFlags(&cpuflg);
	VA_START(ap, fmt);
	FmtCore(buf, fmt, ap);
	HalDefStringWrite(buf);
	VA_END(ap);
	HalStiFlags(&cpuflg);
	return;
}

public void KrlLog(UInt level, const Char* fmt, ...)
{
    CPUFlg cpuflg;
	Char buf[512];
	VAList ap;
    if(0 != level)
    {
        return;
    }
	HalCliFlags(&cpuflg);
	VA_START(ap, fmt);
	FmtCore(buf, fmt, ap);
	HalDefStringWrite(buf);
	VA_END(ap);
	HalStiFlags(&cpuflg);
    return;
}

public void KrlErrorCrashDead(const Char* fmt, ...)
{
    CPUFlg cpuflg;
	Char buf[512];
	VAList ap;
	HalCliFlags(&cpuflg);
	VA_START(ap, fmt);
	FmtCore(buf, fmt, ap);
	HalDefStringWrite(buf);
	VA_END(ap);
	HalStiFlags(&cpuflg);
    HalDead(0);
    return;
}

public Bool KrlLogInit()
{
    return TRUE;
}