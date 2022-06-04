/*-----------------------------------------------------------------------------------------
**
** 版权(Copyright): Inory, 2022~
**
** 文件名(FileName): logger.h
**
** 描述(Description): 本文件定义Logger类,该类对应用程序的日志进行管理
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
#ifndef LOGGER_H
#define LOGGER_H

/*-----------------------------------------------------------------------------------------
**                                         Include
**-----------------------------------------------------------------------------------------
*/
#include "kernelInterface.h"
#include "lockfreeQueue.h"
#include <string.h>
#include <string>
#include <mutex>
#include <condition_variable>

using namespace std;

/*-----------------------------------------------------------------------------------------
**                                     Macro Definition
**-----------------------------------------------------------------------------------------
*/
/* log信息队列大小 */
#define LOG_QUEUE_SIZE      512

/*-----------------------------------------------------------------------------------------
**                                     Type Definition
**-----------------------------------------------------------------------------------------
*/
/* log信息结构体 */
#pragma pack(1)
typedef struct _SLogInfo
{
    char info[LOG_INFO_SIZE + 1];   /* log信息 */
} SLogInfo;
#pragma pack()

/*-----------------------------------------------------------------------------------------
**                                     Macro Definition
**-----------------------------------------------------------------------------------------
*/
#define KERNELLOGF(...)     {char errStr[LOG_INFO_SIZE] = {0}; sprintf(errStr, __VA_ARGS__); _logger->log(errStr);}

/*-----------------------------------------------------------------------------------------
**                                     Class Definition
**-----------------------------------------------------------------------------------------
*/
/* <类描述> */
/* 1)日志管理 */
/* 2) */
class Logger
{
public:
    /* 构造函数 */
    Logger(const string& logPath);
    /* 析构函数 */
    ~Logger();

    /* 记录日志 */
    void log(const char* path);
    /* 刷新日志缓冲并停止记录 */
    void stop();
private:
    /* 写入日志文件线程函数 */
    static void writeLogFile(Logger* logger);
private:
    /* 日志文件描述符 */
    int _fd;
    /* 出错标志 */
    bool _hasError;
    /* 退出标志 */
    bool _exitFlag;
    /* 日志队列 */
    LockfreeQueue<SLogInfo, LOG_QUEUE_SIZE> _logQueue;
    /* 日志队列读写同步锁 */
    mutex _mtx;
    /* 条件变量 */
    condition_variable _cv;
};

#endif /* LOGGER_H */
