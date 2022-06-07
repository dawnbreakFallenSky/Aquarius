/*-----------------------------------------------------------------------------------------
**
** 版权(Copyright): Inory, 2022~
**
** 文件名(FileName): dfsStatus.cpp
**
** 描述(Description): 本文件定义dfsStatus类成员函数实现
**
** 包含的函数(Included Function):
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
/*-----------------------------------------------------------------------------------------
**                                         Include
**-----------------------------------------------------------------------------------------
*/
#include "dfsStatus.h"

/*-----------------------------------------------------------------------------------------
**                                   Function Definition
**-----------------------------------------------------------------------------------------
*/
/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): DfsStatus
**
** 描述(Description): 本函数为DfsStatus类构造函数
**
** 输入参数(Input Parameter):
**      initValue , int 状态初始值
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**      Inory, 2022年6月6日 创建本函数
**      $Log$
**
**.EH--------------------------------------------------------------------------------------
*/
DfsStatus::DfsStatus(int initValue)
{
    _value = initValue;
}
/* END of DfsStatus */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): ~DfsStatus
**
** 描述(Description): 本函数为DfsStatus类析构函数
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
**      Inory, 2022年6月6日 创建本函数
**      $Log$
**
**.EH--------------------------------------------------------------------------------------
*/
DfsStatus::~DfsStatus()
{
#ifdef DEBUG
    printf("DfsStatus::~DfsStatus()\n");
#endif
}
/* END of ~DfsStatus */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): operator=
**
** 描述(Description): 本函数实现赋值运算符重载
**
** 输入参数(Input Parameter):
**      value , int 状态值
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**      Status& 对象引用
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**      Inory, 2022年6月6日 创建本函数
**      $Log$
**
**.EH--------------------------------------------------------------------------------------
*/
Status& DfsStatus::operator=(int value)
{
    _mtx.lock();

    _value = value;

    _mtx.unlock();

    return *this;
}
/* END of operator= */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): operator int
**
** 描述(Description): 本函数实现赋值运算符重载
**
** 输入参数(Input Parameter):
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**      _value , int 状态值
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**      Inory, 2022年6月6日 创建本函数
**      $Log$
**
**.EH--------------------------------------------------------------------------------------
*/
DfsStatus::operator int()
{
    return _value;
}
/* END of operator int */
