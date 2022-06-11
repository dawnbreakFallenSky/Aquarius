/*-----------------------------------------------------------------------------------------
**
** 版权(Copyright): Inory, 2022~
**
** 文件名(FileName): statusManager.cpp
**
** 描述(Description): 本文件定义statusManager类成员函数实现
**
** 包含的函数(Included Function):
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
/*-----------------------------------------------------------------------------------------
**                                         Include
**-----------------------------------------------------------------------------------------
*/
#include "statusManager.h"

/*-----------------------------------------------------------------------------------------
**                                   Function Definition
**-----------------------------------------------------------------------------------------
*/
/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): StatusManager
**
** 描述(Description): 本函数为StatusManager类构造函数
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
**      Inory, 2022年6月8日 创建本函数
**      $Log$
**
**.EH--------------------------------------------------------------------------------------
*/
StatusManager::StatusManager(Logger* logger)
{
    _logger = logger;
}
/* END of StatusManager */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): ~StatusManager
**
** 描述(Description): 本函数为StatusManager类析构函数
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
**      Inory, 2022年6月8日 创建本函数
**      $Log$
**
**.EH--------------------------------------------------------------------------------------
*/
StatusManager::~StatusManager()
{
#ifdef DEBUG
    printf("StatusManager::StatusManager()\n");
#endif
    /* 释放共享内存 */
    map<string, DfsStatus*>::iterator iter;
    iter = _statusMap.begin();
    while (iter != _statusMap.end()) {
        FREE_ARRAY((*iter).second);
    }
    _statusMap.clear();
}
/* END of ~StatusManager */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): getStatus
**
** 描述(Description): 本函数实现获取状态的功能
**
** 输入参数(Input Parameter):
**      statusName , const string& 状态名称
**      initValue , int 初始值
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**      Inory, 2022年6月8日 创建本函数
**      $Log$
**
**.EH--------------------------------------------------------------------------------------
*/
Status& StatusManager::getStatus(const string& statusName, int initValue)
{
    Status* sta = nullptr;
    /* 锁定状态表互斥锁 */
    _mtx.lock();

    /* 判断状态是否有效,并返回状态值 */
    if (0 == _statusMap.count(statusName))
    {
        try
        {
            DfsStatus* newStatus = new DfsStatus(initValue);
            _statusMap[statusName] = newStatus;
        }
        catch(bad_alloc)
        {
            printf("StatusManager::getStatus()--create Status(%s) error.\n", statusName.c_str());
            KERNELLOGF("StatusManager::getStatus()--create Status(%s) error.\n", statusName.c_str());
            sta = &_invalidStatus;
        }
    }

    /* 解除状态表互斥锁 */
    _mtx.unlock();

    return *sta;
}
/* END of getStatus */
