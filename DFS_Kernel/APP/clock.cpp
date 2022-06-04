/*-----------------------------------------------------------------------------------------
**
** 版权(Copyright): Inory, 2022~
**
** 文件名(FileName): clock.cpp
**
** 描述(Description): 本文件定义Clock类成员函数实现
**
** 包含的函数(Included Function):
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
/*-----------------------------------------------------------------------------------------
**                                         Include
**-----------------------------------------------------------------------------------------
*/
#include "clock.h"
#ifdef WINDOWS
#include <time.h>
#else
#include <signal.h>
#include <time.h>
#endif

/*-----------------------------------------------------------------------------------------
**                                   Variable Definition
**-----------------------------------------------------------------------------------------
*/
/* 单例对象指针 */
Clock* Clock::_clock = nullptr;
/* 垃圾回收对象 */
Clock::GC Clock::_gc;
/* 每月天数 */
unsigned char Clock::_days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
/* 时间戳 */
unsigned char Clock::_timestampBcd[8] = {0};
unsigned char Clock::_timestampHex[8] = {0};

/*-----------------------------------------------------------------------------------------
**                                   Function Definition
**-----------------------------------------------------------------------------------------
*/
/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): Clock
**
** 描述(Description): 本函数为Clock类构造函数,初始化成员变量
**
** 输入参数(Input Parameter):
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**      Inory, 2022年5月30日 创建本函数
**      $Log$
**
**.EH--------------------------------------------------------------------------------------
*/
Clock::Clock()
{
    /* 成员变量初始化 */
    _exitFlag = false;
    _clockHandler = nullptr;
    _period.it_value.tv_sec = PERIOD_SECOND;
    _period.it_value.tv_usec = PERIOD_USECOND;
    _period.it_interval = _period.it_value;
#ifdef WINDOWS
    /* 定时器句柄 */
    _timerId = 0;
#endif

    _appTime = 0;
    _utcTimeBegin = time((time_t*)nullptr);
    struct tm* localTime = localtime((time_t*)&_utcTimeBegin);
    int year = 1900 + localTime->tm_year;
    if ((year % 400 == 0) || ((year % 4 == 0) && (year % 100 != 0))) {
        _days[1] = 29;
    }

    _timestampBcd[0] = hexToBcd((unsigned char)(year / 100));
    _timestampBcd[1] = hexToBcd((unsigned char)(year % 100));
    _timestampBcd[2] = hexToBcd((unsigned char)(1 + localTime->tm_mon));
    _timestampBcd[3] = hexToBcd((unsigned char)(localTime->tm_mday));
    _timestampBcd[4] = hexToBcd((unsigned char)(localTime->tm_hour));
    _timestampBcd[5] = hexToBcd((unsigned char)(localTime->tm_min));
    _timestampBcd[6] = hexToBcd((unsigned char)(localTime->tm_sec));
    _timestampBcd[7] = 0x00;

    _timestampHex[0] = (unsigned char)((1900 + localTime->tm_year) / 100);
    _timestampHex[1] = (unsigned char)((1900 + localTime->tm_year) % 100);
    _timestampHex[2] = (unsigned char)(1 + localTime->tm_mon);
    _timestampHex[3] = (unsigned char)(localTime->tm_mday);
    _timestampHex[4] = (unsigned char)(localTime->tm_hour);
    _timestampHex[5] = (unsigned char)(localTime->tm_min);
    _timestampHex[6] = (unsigned char)(localTime->tm_sec);
    _timestampHex[7] = 0x00;
}
/* END of Clock */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): ~Clock
**
** 描述(Description): 本函数为Clock类析构函数
**
** 输入参数(Input Parameter):
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**      Inory, 2022年5月30日 创建本函数
**      $Log$
**
**.EH--------------------------------------------------------------------------------------
*/
Clock::~Clock()
{
#ifdef DEBUG
    printf("Clock::~Clock()\n");
#endif
#ifdef WINDOWS
    if (0 != _timerId) {
        timeKillEvent(_timerId);
    }
#endif
    _clock = nullptr;
}
/* END of ~Clock */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): setPeriod
**
** 描述(Description): 本函数实现设置时钟周期的功能
**
** 输入参数(Input Parameter):
**      second , unsigned int 定时周期秒
**      microsecond , unsigned long long 定时周期微秒
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**      Inory, 2022年5月30日 创建本函数
**      $Log$
**
**.EH--------------------------------------------------------------------------------------
*/
void Clock::setPeriod(unsigned int second, unsigned long long microsecond)
{
    _period.it_value.tv_sec = second;
    _period.it_value.tv_usec = microsecond;
    _period.it_interval = _period.it_value;
}
/* END of setPeriod */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): setClockHandler
**
** 描述(Description): 本函数实现设置时钟用户处理函数的功能
**
** 输入参数(Input Parameter):
**      handler , ClockHandler 用户时钟处理函数
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**      Inory, 2022年5月30日 创建本函数
**      $Log$
**
**.EH--------------------------------------------------------------------------------------
*/
void Clock::setClockHandler(ClockHandler handler)
{
    _clockHandler = handler;
}
/* ENd of setClockHandler */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): start
**
** 描述(Description): 本函数实现启动时钟的功能
**
** 输入参数(Input Parameter):
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**      bool false-启动失败 true-启动成功
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**      Inory, 2022年5月30日 创建本函数
**      $Log$
**
**.EH--------------------------------------------------------------------------------------
*/
bool Clock::start()
{
#ifdef WINDOWS
    _timerId = timeSetEvent((UINT)(_period.it_value.tv_sec * 1000 + _period.it_value.tv_usec / 1000), 1, (LPTIMECALLBACK)timerHandler, 0, (UINT)TIME_PERIODIC);
    if (0 == _timerId) {
        return false;
    }
    else {
        return true;
    }
#else
    /* 将定时器信号同处理函数进行绑定 */
    signal(SIGALRM, timerHandler);

    /* 启动定时器 */
    if (-1 == setitimer(ITIME_REAL, &_period, NULL)) {
        return false;
    }
    else {
        return true;
    }
#endif
}
/* END of start */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): stop
**
** 描述(Description): 本函数实现关闭时钟的功能
**
** 输入参数(Input Parameter):
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**      Inory, 2022年5月30日 创建本函数
**      $Log$
**
**.EH--------------------------------------------------------------------------------------
*/
void Clock::stop()
{
    _exitFlag = true;
}
/* END of stop */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): appTime
**
** 描述(Description): 本函数实现返回程序运行时间的功能
**
** 输入参数(Input Parameter):
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**      unsigned long long 程序运行时间
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**      Inory, 2022年5月30日 创建本函数
**      $Log$
**
**.EH--------------------------------------------------------------------------------------
*/
unsigned long long Clock::appTime()
{
    if (NULL != _clock) {
        return _clock->_appTime;
    }
    else {
        return 0;
    }
}
/* END of appTime */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): utcTime
**
** 描述(Description): 本函数实现获取UTC时间(单位为S)的功能
**
** 输入参数(Input Parameter):
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**      long long UTC时间
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**      Inory, 2022年5月30日 创建本函数
**      $Log$
**
**.EH--------------------------------------------------------------------------------------
*/
long long Clock::utcTime()
{
    if (NULL != _clock) {
        return _clock->_utcTimeBegin + (_clock->_appTime / INTERVAL_1S);
    }
    else {
        return time((time_t*)NULL);
    }
}
/* END of utcTime */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): timestamp
**
** 描述(Description): 本函数实现获取时间戳的功能
**
** 输入参数(Input Parameter):
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**      _timestampBcd , unsigned char* 时间戳
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**      Inory, 2022年5月30日 创建本函数
**      $Log$
**
**.EH--------------------------------------------------------------------------------------
*/
unsigned char* Clock::timestamp()
{
    return _timestampBcd;
}
/* END of timestamp */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): instance
**
** 描述(Description): 本函数实现返回时钟单例指针的功能
**
** 输入参数(Input Parameter):
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**      Inory, 2022年5月30日 创建本函数
**      $Log$
**
**.EH--------------------------------------------------------------------------------------
*/
Clock* Clock::instance()
{
    /* 创建并返回时钟对象指针 */
    if (NULL == _clock)
    {
        try {
            _clock = new Clock();
        }
        catch(bad_alloc&) {
            printf("Clock::instance()--create Clock error.\n");
        }
    }

    return _clock;
}
/* END of instance */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): hexToBcd
**
** 描述(Description): 本函数实现HEX-BCD码转换的功能
**
** 输入参数(Input Parameter):
**      value , unsigned char HEX码
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**
** 设计注记(Design Annotation):
**      unsigned char BCD码
**
** 更改历史(Modification History):
**      Inory, 2022年5月30日 创建本函数
**      $Log$
**
**.EH--------------------------------------------------------------------------------------
*/
unsigned char Clock::hexToBcd(unsigned char value)
{
    value = (value >= 200 ? value - 200 : (value >= 100 ? value - 100 : value));
    return (value / 10 << 4) | (value % 10);
}
/* END of hexToBcd */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): bcdToHex
**
** 描述(Description): 本函数实现BCD-HEX码转换的功能
**
** 输入参数(Input Parameter):
**      value , unsigned char BCD码
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**
** 设计注记(Design Annotation):
**      unsigned char HEX码
**
** 更改历史(Modification History):
**      Inory, 2022年5月30日 创建本函数
**      $Log$
**
**.EH--------------------------------------------------------------------------------------
*/
unsigned char Clock::bcdToHex(unsigned char value)
{
    return (value >> 4) * 10 + (value & 0x0F);
}
/* END of bcdToHex */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): updateTimestamp
**
** 描述(Description): 本函数实现更新时间戳的功能
**
** 输入参数(Input Parameter):
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**      Inory, 2022年5月30日 创建本函数
**      $Log$
**
**.EH--------------------------------------------------------------------------------------
*/
void Clock::updateTimestamp()
{
    if (++_clock->_timestampHex[7] >= 100)
    {
        _clock->_timestampHex[7] = 0;
        if (++_clock->_timestampHex[6] >= 60)
        {
            _clock->_timestampHex[6] = 0;
            if (++_clock->_timestampHex[5] >= 60)
            {
                _clock->_timestampHex[5] = 0;
                if (++_clock->_timestampHex[4] >= 24)
                {
                    _clock->_timestampHex[4] = 0;
                    if (++_clock->_timestampHex[3] >= _days[_clock->_timestampHex[2] - 1])
                    {
                        _clock->_timestampHex[3] = 0;
                        if (++_clock->_timestampHex[2] >= 13)
                        {
                            _clock->_timestampHex[2] = 1;
                            if (++_clock->_timestampHex[1] >= 100)
                            {
                                _clock->_timestampHex[1] = 0;
                                if (++_clock->_timestampHex[0] >= 100) {
                                    _clock->_timestampHex[0] = 0;
                                }
                                _clock->_timestampBcd[0] = hexToBcd(_clock->_timestampHex[0]);
                            }
                            _clock->_timestampBcd[1] = hexToBcd(_clock->_timestampHex[1]);
                        }
                        _clock->_timestampBcd[2] = hexToBcd(_clock->_timestampHex[2]);
                    }
                    _clock->_timestampBcd[3] = hexToBcd(_clock->_timestampHex[3]);
                }
                _clock->_timestampBcd[4] = hexToBcd(_clock->_timestampHex[4]);
            }
            _clock->_timestampBcd[5] = hexToBcd(_clock->_timestampHex[5]);
        }
        _clock->_timestampBcd[6] = hexToBcd(_clock->_timestampHex[6]);
    }
    _clock->_timestampBcd[7] = hexToBcd(_clock->_timestampHex[7]);
}
/* END of updateTimestamp */
#ifdef WINDOWS
/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): timerHandler
**
** 描述(Description): 本函数实现时钟中断处理的功能
**
** 输入参数(Input Parameter):
**      wTimerID , UINT 定时器ID
**      msg , UINT 消息
**      dwUser , DWORD 用户
**      dw1 , DWORD 预留参数1
**      dw2 , DWORD 预留参数2
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**      Inory, 2022年5月30日 创建本函数
**      $Log$
**
**.EH--------------------------------------------------------------------------------------
*/
void Clock::timerHandler(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
    UNUSED(wTimerID);
    UNUSED(msg);
    UNUSED(dwUser);
    UNUSED(dw1);
    UNUSED(dw2);

    /* 判断时钟是否启动 */
    if ((NULL == _clock) || (_clock->_exitFlag)) {
        return;
    }

    /* 更新应用程序运行时间 */
    _clock->_appTime++;
    /* 更新时间戳 */
    updateTimestamp();

    /* 调用用户时钟处理函数 */
    if (nullptr != _clock->_clockHandler) {
        _clock->_clockHandler();
    }
}
/* END of timerHandler */
#else
/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): timerHandler
**
** 描述(Description): 本函数实现时钟中断处理的功能
**
** 输入参数(Input Parameter):
**      param , int 变量
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**      Inory, 2022年5月30日 创建本函数
**      $Log$
**
**.EH--------------------------------------------------------------------------------------
*/
void Clock::timerHandler(int param)
{
    UNUSED(param);

    /* 判断时钟是否启动 */
    if ((NULL == _clock) || (_clock->_exitFlag)) {
        return;
    }

    /* 更新应用程序运行时间 */
    _clock->_appTime++;
    /* 更新时间戳 */
    updateTimestamp();

    /* 调用用户时钟处理函数 */
    if (nullptr != _clock->_clockHandler) {
        _clock->_clockHandler();
    }
}
/* END of timerHandler */
#endif
