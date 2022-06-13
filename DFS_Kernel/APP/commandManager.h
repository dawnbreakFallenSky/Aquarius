/*-----------------------------------------------------------------------------------------
**
** 版权(Copyright): Inory, 2022~
**
** 文件名(FileName): commandManager.h
**
** 描述(Description): 本文件定义commandManager类,该类对应用程序的命令行交互进行管理
**
** 设计注记(Design Annotation):
**
** 作者(Author):
**      Inory, 2022年6月9日开始编写本文件
**
** 更改历史(Modification History):
**      Inory, 2022年6月9日 创建本文件
**      $Revision$
**      $Date$
**      $Author$
**      $Log$
**
**-----------------------------------------------------------------------------------------
*/
#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H

/*-----------------------------------------------------------------------------------------
**                                         Include
**-----------------------------------------------------------------------------------------
*/
#include "global.h"
#include <map>
#include <string>
#include <functional>
#include <mutex>

using std::map;
using std::string;
using std::function;
using std::mutex;
using std::bind;
using std::placeholders::_1;
using std::placeholders::_2;

/*-----------------------------------------------------------------------------------------
**                                     Type Definition
**-----------------------------------------------------------------------------------------
*/
/* 命令行函数类型 */
typedef function<void(int argc, char* argv[])> CmdHandler;

/*-----------------------------------------------------------------------------------------
**                                     Macro Definition
**-----------------------------------------------------------------------------------------
*/
/* 命令行最大字符数,参数最大个数 */
#define CMD_SIZE_MAX    256
#define CMD_ARGC_MAX    128

/*-----------------------------------------------------------------------------------------
**                                     Type Definition
**-----------------------------------------------------------------------------------------
*/
/* 命令行指令保存信息 */
typedef struct _CmdInfo
{
    CmdHandler func;    /* 函数指针 */
    string comment;     /* 函数注释 */
} SCmdInfo;

/*-----------------------------------------------------------------------------------------
**                                     Class Definition
**-----------------------------------------------------------------------------------------
*/
/* <类描述> */
/* 1)命令行管理 */
/* 2) */
class CommandManager
{
public:
    /* 命令行运行方式枚举 */
    typedef enum _CmdStyle
    {
        NONE,               /* 无命令行 */
        TERMINAL,           /* 控制台终端模式 */
        REMOTE_TERMINAL,    /* 远程终端模式 */
        BROWSER             /* 浏览器模式 */
    } ECmdStyle;
public:
    /* 构造函数 */
    CommandManager(ECmdStyle runMode);
    /* 析构函数 */
    ~CommandManager();

    /* 停止命令行 */
    void stop();
    /* 注册命令行指令 */
    void registerCommand(const string& cmd, CmdHandler cmdHandler, const string& comment);
private:
    /* 命令行接收处理线程函数 */
    static void cmdProcess(CommandManager* commandManager);
    /* 命令行帮助函数 */
    void help(int argc, char* argv[]);
private:
    /* 命令行指令列表 */
    map<string, SCmdInfo> _cmdMap;
    /* 命令行指令列表读写互斥锁 */
    mutex _mtx;
    /* 退出标志 */
    static bool _exitFlag;
    /* 运行模式 */
    ECmdStyle _runMode;
};

#endif /* COMMANDMANAGER_H */
