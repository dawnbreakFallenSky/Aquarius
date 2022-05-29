/*-----------------------------------------------------------------------------------------
**
** 版权(Copyright): Inory, 2022~
**
** 文件名(FileName): clock.h
**
** 描述(Description): 本文件定义Clock类,该类实现定时功能
**
** 设计注记(Design Annotation):
**
** 作者(Author):
**      Inory, 2022年5月28日开始编写本文件
**
** 更改历史(Modification History):
**      Inory, 2022年5月28日 创建本文件
**      $Revision$
**      $Date$
**      $Author$
**      $Log$
**
**-----------------------------------------------------------------------------------------
*/
#ifndef CLOCK_H
#define CLOCK_H

/*-----------------------------------------------------------------------------------------
**                                         Include
**-----------------------------------------------------------------------------------------
*/
#include "global.h"
#include <functional>
#ifdef WINDOWS
#define NOMINMAX                    /* 禁用VC++中的min/max宏定义 */
#define WIN32_LEAN_AND_MEAN         /* 不定义此宏,Windows.h有可能包含winsock.h */
#include <Windows.h>
#include <mmsystem.h>               /* Windows多媒体定时器头文件和库文件 */
#pragma comment(lib, "WinMM.lib")

/*-----------------------------------------------------------------------------------------
**                                     Type Definition
**-----------------------------------------------------------------------------------------
*/
/* 定时参数结构体 */
struct timeval
{
    unsigned int tv_sec;            /* Seconds */
    unsigned long long tv_usec;     /* Microseconds */
};
struct itimerval
{
    struct timeval it_interval;
    struct timeval it_value;
};
#else
/*-----------------------------------------------------------------------------------------
**                                         Include
**-----------------------------------------------------------------------------------------
*/
#include <sys/time.h>
#endif

using std::function;
using std::bad_alloc;

/*-----------------------------------------------------------------------------------------
**                                     Type Definition
**-----------------------------------------------------------------------------------------
*/
/* 用户时钟处理函数指针类型 */
typedef function<void()> ClockHandler;

/*-----------------------------------------------------------------------------------------
**                                     Macro Definition
**-----------------------------------------------------------------------------------------
*/
/* 定时周期默认值(秒+微秒) */
#define PERIOD_SECOND   0
#define PERIOD_USECOND  10000
/* 定时计数 */
#define INTERVAL_10MS   1
#define INTERVAL_20MS   2
#define INTERVAL_30MS   3
#define INTERVAL_40MS   4
#define INTERVAL_50MS   5
#define INTERVAL_100MS  10
#define INTERVAL_200MS  20
#define INTERVAL_500MS  50
#define INTERVAL_1S     100

/*-----------------------------------------------------------------------------------------
**                                     Class Definition
**-----------------------------------------------------------------------------------------
*/
/* <类描述> */
/* 1)产生周期定时信号,作为应用程序的时钟源 */
/* 2)单实例 */
class Clock
{
private:
    /* 构造函数 */
    Clock();
public:
    /* 析构函数 */
    ~Clock();

    /* 设置时钟周期 */
    void setPeriod(unsigned int second, unsigned long long microsecond);
    /* 设置时钟处理函数 */
    void setClockHandler(ClockHandler handler);
    /* 启动时钟 */
    bool start();
    /* 停止时钟 */
    void stop();

    /* 获取程序运行时间(单位为定时周期) */
    static unsigned long long appTime();
    /* 获取UTC时间(单位为S) */
    static long long utcTime();
    /* 获取时间戳 */
    static unsigned char* timestamp();

    /* 获取时钟单例 */
    static Clock* instance();
private:
    /* HEX-BCD码转换 */
    static unsigned char hexToBcd(unsigned char value);
    static unsigned char bcdToHex(unsigned char value);
    /* 更新时间戳 */
    static void updateTimestamp();
#ifdef WINDOWS
    /* 时钟中断处理函数 */
    static void CALLBACK timerHandler(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2);
#else
    /* 时钟中断处理函数 */
    static void timerHandler(int param);
#endif
private:
    /* 退出标志 */
    bool _exitFlag;
    /* 用户时钟处理函数指针 */
    ClockHandler _clockHandler;
    /* 时钟周期 */
    struct itimerval _period;
#ifdef WINDOWS
    /* 定时器句柄 */
    MMRESULT _timerId;
#endif
    /* 应用程序运行时间(周期累加) */
    unsigned long long _appTime;
    /* 应用程序运行时utc时间 */
    long long _utcTimeBegin;

    /* 类单实例指针 */
    static Clock* _clock;
    /* 每月天数 */
    static unsigned char _days[12];
    /* 时间戳 */
    static unsigned char _timestampBcd[8];
    static unsigned char _timestampHex[8];
private:
    /* 垃圾回收类 */
    class GC
    {
    public:
        /* 构造函数 */
        GC()
        {
            PASS
        }
        /* 析构函数 */
        ~GC()
        {
            /* 释放单例内存 */
            FREE_OBJECT(_clock)
        }
    };
    /* 垃圾回收对象 */
    static GC _gc;
};

#endif /* CLOCK_H */
