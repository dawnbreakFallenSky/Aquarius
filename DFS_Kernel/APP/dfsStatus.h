/*-----------------------------------------------------------------------------------------
**
** 版权(Copyright): Inory, 2022~
**
** 文件名(FileName): dfsStatus.h
**
** 描述(Description): 本文件定义dfsStatus类,该类实现状态接口
**
** 设计注记(Design Annotation):
**
** 作者(Author):
**      Inory, 2022年6月5日开始编写本文件
**
** 更改历史(Modification History):
**      Inory, 2022年6月5日 创建本文件
**      $Revision$
**      $Date$
**      $Author$
**      $Log$
**
**-----------------------------------------------------------------------------------------
*/
#ifndef DFSSTATUS_H
#define DFSSTATUS_H

/*-----------------------------------------------------------------------------------------
**                                         Include
**-----------------------------------------------------------------------------------------
*/
#include "kernelInterface.h"
#include <mutex>

using std::mutex;

/*-----------------------------------------------------------------------------------------
**                                     Class Definition
**-----------------------------------------------------------------------------------------
*/
/* <类描述> */
/* 1)Status接口实例 */
/* 2) */
class DfsStatus: public Status
{
public:
    /* 构造函数 */
    DfsStatus(int initValue = STATUS_INVALID);
    /* 析构函数 */
    ~DfsStatus();

    /* 赋值运算符重载 */
    Status& operator = (int value);
    /* int转换函数 */
    operator int();
private:
    /* 状态互斥锁 */
    mutex _mtx;
    /* 状态值 */
    int _value;
};

#endif /* DFSSTATUS_H */
