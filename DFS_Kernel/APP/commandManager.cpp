/*-----------------------------------------------------------------------------------------
**
** 版权(Copyright): Inory, 2022~
**
** 文件名(FileName): commandManager.cpp
**
** 描述(Description): 本文件定义commandManager类成员函数实现
**
** 包含的函数(Included Function):
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
/*-----------------------------------------------------------------------------------------
**                                         Include
**-----------------------------------------------------------------------------------------
*/
#include "commandManager.h"
#include <string.h>
#include <thread>

using std::thread;

/*-----------------------------------------------------------------------------------------
**                                   Variable Definition
**-----------------------------------------------------------------------------------------
*/
/* 退出标志 */
bool CommandManager::_exitFlag = false;

/*-----------------------------------------------------------------------------------------
**                                   Function Definition
**-----------------------------------------------------------------------------------------
*/
/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): CommandManager
**
** 描述(Description): 本函数为CommandManager类构造函数,注册【帮助】命令,创建并启动命令行接收处理线程
**
** 输入参数(Input Parameter):
**      runMode , ECmdStyle 命令行运行模式
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**      Inory, 2022年6月11日 创建本函数
**      $Log$
**
**.EH--------------------------------------------------------------------------------------
*/
CommandManager::CommandManager(ECmdStyle runMode)
{
    /* 初始化成员变量 */
    _runMode = runMode;

    /* 注册命令行帮助命令 */
    CmdHandler helpHandler = bind(&CommandManager::help, this, _1, _2);
    registerCommand("help", helpHandler, "打印帮助信息");

    /* 终端运行模式下创建并启动命令行接收处理线程 */
    if (TERMINAL == runMode)
    {
        thread cmdProcessThread(cmdProcess, this);
        cmdProcessThread.detach();
    }
    else if (REMOTE_TERMINAL == runMode) {
        /* TODO */
    }
    else if (BROWSER == runMode) {
        /* TODO */
    }
    else {
        PASS;
    }
}
/* END of CommandManager */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): ~CommandManager
**
** 描述(Description): 本函数为CommandManager类析构函数
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
**      Inory, 2022年6月11日 创建本函数
**      $Log$
**
**.EH--------------------------------------------------------------------------------------
*/
CommandManager::~CommandManager()
{
    _exitFlag = true;
#ifdef DEBUG
    printf("CommandManager::~CommandManager()\n");
#endif
}
/* END of ~CommandManager */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): stop
**
** 描述(Description): 本函数实现停止命令行交互的功能
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
**      Inory, 2022年6月11日 创建本函数
**      $Log$
**
**.EH--------------------------------------------------------------------------------------
*/
void CommandManager::stop()
{
    _exitFlag = true;
}
/* END of stop */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): registerCommand
**
** 描述(Description): 本函数实现注册命令行交互指令的功能
**
** 输入参数(Input Parameter):
**      cmd , const std::string& 指令字符串
**      cmdHandler , CmdHandler 命令处理函数指针
**      comment , const std::string& 命令说明
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**      Inory, 2022年6月11日 创建本函数
**      $Log$
**
**.EH--------------------------------------------------------------------------------------
*/
void CommandManager::registerCommand(const string& cmd, CmdHandler cmdHandler, const string& comment)
{
    /* 锁定命令行指令列表读写互斥锁 */
    _mtx.lock();

    /* 创建命令行指令信息并加入指令列表 */
    SCmdInfo info = {cmdHandler, comment};
    _cmdMap.insert(std::pair<std::string, SCmdInfo>(cmd, info));

    /* 解除读写互斥锁 */
    _mtx.unlock();
}
/* END of registerCommand */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): cmdProcess
**
** 描述(Description): 本函数接收用户命令行输入并解析执行相应操作
**
** 输入参数(Input Parameter):
**      commandManager , CommandManager* 对象指针
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**      Inory, 2022年6月11日 创建本函数
**      $Log$
**
**.EH--------------------------------------------------------------------------------------
*/
void CommandManager::cmdProcess(CommandManager* commandManager)
{
    /* 判断输入参数有效性 */
    if (NULL == commandManager) {
        return;
    }

    /* 循环读取用户输入,并解析命令 */
    while (true)
    {
        char cmdBuf[CMD_SIZE_MAX] = {0};    /* 命令行读取字符串等参数 */
        char cmd[CMD_SIZE_MAX] = {0};
        char* argv[CMD_ARGC_MAX] = {nullptr};
        int argc = 0;

        /* 打印提示符并输入 */
        printf(":\\>");
        fgets(cmdBuf, CMD_SIZE_MAX, stdin);

        /* 判断程序退出标志 */
        if (_exitFlag) {
            break;
        }

        /* 判断是否只输入了回车 */
        if (1 == strlen(cmdBuf)) {
            continue;
        }

        /* 判断是否输入命令 */
        if (1 != sscanf(cmdBuf, "%s", cmd)) {
            continue;
        }

        /* 读取命令参数 */
        for (unsigned int i = (unsigned int)strlen(cmd); i < CMD_SIZE_MAX; i++)
        {
            if ((cmdBuf[i] == '\n') || (cmdBuf[i] == '\0'))
            {
                cmdBuf[i] = '\0';
                break;
            }
            else if (cmdBuf[i] == ' ') {
                cmdBuf[i] = '\0';
            }
            else if (cmdBuf[i - 1] == '\0')
            {
                argv[argc] = &cmdBuf[i];
                argc++;
            }
            else {
                PASS
            }
        }

        /* 锁定命令行指令列表读写互斥锁 */
        commandManager->_mtx.lock();

        /* 判断命令是否有效,并调用对应函数 */
        if (0 == commandManager->_cmdMap.count(cmd)) {
            printf(":\\%s:command not found\n", cmd);
        }
        else {
            commandManager->_cmdMap[cmd].func(argc, argv);
        }

        /* 解除读写互斥锁 */
        commandManager->_mtx.unlock();
    }
}
/* END of cmdProcess */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): help
**
** 描述(Description): 本函数实现显示帮助信息的功能
**
** 输入参数(Input Parameter):
**      argc , int 参数个数
**      argv , char** 参数列表
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**      Inory, 2022年6月11日 创建本函数
**      $Log$
**
**.EH--------------------------------------------------------------------------------------
*/
void CommandManager::help(int argc, char* argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    /* 打印所有注册的命令 */
    std::map<std::string, SCmdInfo>::iterator iter;
    iter = _cmdMap.begin();

    while (iter != _cmdMap.end())
    {
        printf(":\\>%s:%s\n", iter->first.c_str(), iter->second.comment.c_str());
        iter++;
    }
}
/* END of help */
