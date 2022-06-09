/*-----------------------------------------------------------------------------------------
**
** 版权(Copyright): Inory, 2022~
**
** 文件名(FileName): statusManager.h
**
** 描述(Description): 本文件定义statusManager类,该类用于管理应用程序所需的所有状态
**
** 设计注记(Design Annotation):
**
** 作者(Author):
**      Inory, 2022年6月7日开始编写本文件
**
** 更改历史(Modification History):
**      Inory, 2022年6月7日 创建本文件
**      $Revision$
**      $Date$
**      $Author$
**      $Log$
**
**-----------------------------------------------------------------------------------------
*/
#ifndef STATUSMANAGER_H
#define STATUSMANAGER_H

/*-----------------------------------------------------------------------------------------
**                                         Include
**-----------------------------------------------------------------------------------------
*/
#include "dfsStatus.h"
#include "logger.h"
#include <string>
#include <map>

using std::string;
using std::map;

/*-----------------------------------------------------------------------------------------
**                                     Class Definition
**-----------------------------------------------------------------------------------------
*/
/* <类描述> */
/* 1)管理应用程序所需的状态 */
/* 2) */
class StatusManager
{
public:
    /* 构造函数 */
    StatusManager(Logger* logger);
    /* 析构函数 */
    ~StatusManager();

    /* 获取状态 */
    Status& getStatus(const string& statusName, int initValue = STATUS_INVALID);
private:
    /* 日志指针 */
    Logger* _logger;
    /* 无效状态 */
    DfsStatus _invalidStatus;
    /* 状态表互斥锁 */
    mutex _mtx;
    /* 状态表 */
    map<string, DfsStatus*> _statusMap;
};

#endif /* STATUSMANAGER_H */
