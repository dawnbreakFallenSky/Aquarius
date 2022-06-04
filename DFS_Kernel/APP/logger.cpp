/*-----------------------------------------------------------------------------------------
**
** 版权(Copyright): Inory, 2022~
**
** 文件名(FileName): logger.cpp
**
** 描述(Description): 本文件定义Logger类成员函数实现
**
** 包含的函数(Included Function):
**
** 设计注记(Design Annotation):
**
** 作者(Author):
**      Inory, 2022年6月2日开始编写本文件
**
** 更改历史(Modification History):
**      Inory, 2022年6月2日 创建本文件
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
#include "logger.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <thread>
#include <vector>
#include <algorithm>
#ifdef WINDOWS
#include <io.h>
#include <process.h>
#include <Windows.h>
#else
#include <unistd.h>
#endif

/*-----------------------------------------------------------------------------------------
**                                   Function Definition
**-----------------------------------------------------------------------------------------
*/
/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): Logger
**
** 描述(Description): 本函数为Logger类构造函数
**
** 输入参数(Input Parameter):
**      logPath , const string& 日志文件路径
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**      Inory, 2022年6月2日 创建本函数
**      $Log$
**
**.EH--------------------------------------------------------------------------------------
*/
Logger::Logger(const string& logPath)
{
    /* 初始化成员变量 */
    _fd = FD_INVALID;
    _hasError = false;
    _exitFlag = false;

    /* 获取当时时间并以此命名日志文件 */
    time_t curTime;
    time(&curTime);
    string fileName = ctime(&curTime);
    fileName.erase(fileName.end() - 1);
    vector<char> nameVector = vector<char>(make_move_iterator(fileName.begin()), make_move_iterator(fileName.end()));
    for_each(nameVector.begin(), nameVector.end(), [](char& c) {
       if (c == ' ' || c == ':') c = '_';
    });
    fileName = string(make_move_iterator(nameVector.begin()), make_move_iterator(nameVector.end()));
    string path = logPath + fileName + ".log";

    /* 新建并打开日志文件 */
#ifdef WINDOWS
    _fd = _open(path.c_str(), O_WRONLY | O_CREAT, S_IREAD);
#else
    _fd = open(path.c_str(), O_WRONLY | O_CREAT, S_IREAD);
#endif
    if (FD_INVALID == _fd)
    {
        printf("Logger::Logger()--open log file error.\n");
        throw bad_alloc();
    }

    /* 创建并启动日志写入线程 */
    thread logThread(writeLogFile, this);
    logThread.detach();
}
/* END of Logger */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): ~Logger
**
** 描述(Description): 本函数为Logger类析构函数
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
**      Inory, 2022年6月2日 创建本函数
**      $Log$
**
**.EH--------------------------------------------------------------------------------------
*/
Logger::~Logger()
{
#ifdef DEBUG
    printf("Logger::~Logger()\n");
#endif
    /* 关闭日志文件 */
#ifdef WINDOWS
    _close(_fd);
#else
    close(_fd);
#endif
}
/* END of ~Logger */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): log
**
** 描述(Description): 本函数实现记录日志的功能
**
** 输入参数(Input Parameter):
**      info , const char* 待记录信息
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**      Inory, 2022年6月2日 创建本函数
**      $Log$
**
**.EH--------------------------------------------------------------------------------------
*/
void Logger::log(const char* info)
{
    /* 判断日志记录是否出错或退出 */
    if (_hasError || _exitFlag) {
        return;
    }
    /* 判断数据有效性 */
    if ((NULL == info) || (strlen(info) > LOG_INFO_SIZE)) {
        return;
    }
    /* 将日志信息加入队列 */
    unique_lock<mutex> lock(_mtx);

    SLogInfo logInfo;
    memset((char*)&logInfo, 0, sizeof(SLogInfo));
    memcpy(logInfo.info, info, strlen(info));

    _logQueue.push(logInfo);
    _cv.notify_one();

    lock.unlock();
}
/* END of log */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): stop
**
** 描述(Description): 本函数实现刷新日志缓冲并停止记录的功能
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
**      Inory, 2022年6月2日 创建本函数
**      $Log$
**
**.EH--------------------------------------------------------------------------------------
*/
void Logger::stop()
{
    _exitFlag = true;

    /* 等待日志缓冲清空 */
    while (true)
    {
        unique_lock<mutex> lock(_mtx);

        if (_logQueue.empty())
        {
            _cv.notify_one();
            lock.unlock();
            break;
        }
        else
        {
            lock.unlock();
#ifdef WINDOWS
            Sleep(10);
#else
            usleep(10);
#endif
        }
    }
}
/* END of stop */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): writeLogFile
**
** 描述(Description): 本函数实现写入日志文件线程函数的功能
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
**      Inory, 2022年6月2日 创建本函数
**      $Log$
**
**.EH--------------------------------------------------------------------------------------
*/
void Logger::writeLogFile(Logger* logger)
{
    /* 判断参数有效性 */
    if (NULL == logger) {
        return;
    }

    time_t logTime; /* 日志时间信息 */
    string strInfo;
    string strTime;
    string strLog;

    /* 循环等待日志写入事件,并写入日志文件 */
    while (true)
    {
        /* 等待同步信号 */
        unique_lock<mutex> lock(logger->_mtx);
        while (logger->_logQueue.empty() && (!logger->_exitFlag)) {
            logger->_cv.wait(lock);
        }
        /* 判断循环等待退出原因(记录或程序退出) */
        if (!logger->_logQueue.empty())
        {
            strInfo = logger->_logQueue.front().info;
            logger->_logQueue.pop();
            lock.unlock();
        }
        else
        {
            lock.unlock();
            break;
        }
        /* 获取当前时间,组合待写入信息 */
        time(&logTime);
        strTime = ctime(&logTime);
        strTime.erase(strTime.end() - 1);
        strLog = strTime + "<-->" + strInfo + "\n";
    }

    /* 写入日志文件 */
#ifdef WINDOWS
    if (-1 == _write(logger->_fd, strLog.c_str(), (unsigned int)strLog.length()))
#else
    if (-1 == write(logger->_fd, strLog.c_str(), (unsigned int)strLog.length()))
#endif
    {
        printf("Logger::writeLogFile()--write error.\n");
        logger->_hasError = true;
        return;
    }
}
/* END of writeLogFile */
