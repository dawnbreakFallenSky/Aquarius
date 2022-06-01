/*-----------------------------------------------------------------------------------------
**
** 版权(Copyright): Inory, 2022~
**
** 文件名(FileName): lockfreeQueue.h
**
** 描述(Description): 本文件定义lockfreeQueue类模板
**
** 设计注记(Design Annotation):
**
** 作者(Author):
**      Inory, 2022年5月31日开始编写本文件
**
** 更改历史(Modification History):
**      Inory, 2022年5月31日 创建本文件
**      $Revision$
**      $Date$
**      $Author$
**      $Log$
**
**-----------------------------------------------------------------------------------------
*/
#ifndef LOCKFREEQUEUE_H
#define LOCKFREEQUEUE_H

/*-----------------------------------------------------------------------------------------
**                                         Include
**-----------------------------------------------------------------------------------------
*/
#include "global.h"
#include <new>
#include <stdio.h>
#include <string.h>

using std::bad_alloc;

/*-----------------------------------------------------------------------------------------
**                                     Class Definition
**-----------------------------------------------------------------------------------------
*/
/* <类描述> */
/* 1)无锁队列(适用于单生产-单消费者) */
/* 2) */
template<typename T, unsigned int len>
class LockfreeQueue
{
public:
    /* 构造函数 */
    LockfreeQueue()throw(bad_alloc);
    /* 析构函数 */
    ~LockfreeQueue();

    /* 判断队列是否为空 */
    bool empty();
    /* 判断队列是否已满 */
    bool full();
    /* 获取队列头数据项 */
    const T& front();
    /* 队列头数据项出列 */
    void pop();
    /* 入列 */
    bool push(const T& item);

    /* 获取队列中数据项个数 */
    unsigned int count();
    /* 获取队列容量 */
    unsigned int capacity();
private:
    /* 数据项存储数组 */
    T* _items;
    /* 头游标 */
    unsigned int _head;
    /* 尾游标 */
    unsigned int _rear;
};

/*-----------------------------------------------------------------------------------------
**                                   Function Definition
**-----------------------------------------------------------------------------------------
*/
/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): LockfreeQueue
**
** 描述(Description): 本函数为LockfreeQueue类构造函数,初始化成员变量,创建存储数组
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
**      Inory, 2022年6月1日 创建本函数
**      $Log$
**
**.EH--------------------------------------------------------------------------------------
*/
template<typename T, unsigned int len>
LockfreeQueue<T, len>::LockfreeQueue()throw(bad_alloc)
{
    /* 初始化成员变量 */
    _head = 0;
    _rear = 0;

    try
    {
        if (len < 2) {
            throw bad_alloc();
        }
        else
        {
            _items = new T[len];
            memset((void*)_items, 0, sizeof(T) * len);
        }
    }
    catch(bad_alloc&)
    {
        printf("LockfreeQueue::LockfreeQueue()--allocate memory for '_items' error.\n");
        throw;
    }
}
/* END of LockfreeQueue */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): ~LockfreeQueue
**
** 描述(Description): 本函数为LockfreeQueue类析构函数
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
**      Inory, 2022年6月1日 创建本函数
**      $Log$
**
**.EH--------------------------------------------------------------------------------------
*/
template<typename T, unsigned int len>
LockfreeQueue<T, len>::~LockfreeQueue()
{
    FREE_ARRAY(_items);
}
/* END of ~LockfreeQueue */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): empty
**
** 描述(Description): 本函数实现判断队列是否为空的功能
**
** 输入参数(Input Parameter):
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**      bool true-为空 false-不为空
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**      Inory, 2022年6月1日 创建本函数
**      $Log$
**
**.EH--------------------------------------------------------------------------------------
*/
template<typename T, unsigned int len>
bool LockfreeQueue<T, len>::empty()
{
    /* 根据头尾游标判断队列是否为空 */
    if (_head == _rear) {
        return true;
    }
    else {
        return false;
    }

}
/* END of empty */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): full
**
** 描述(Description): 本函数实现判断队列是否为满的功能
**
** 输入参数(Input Parameter):
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**      bool true-为满 false-不为满
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**      Inory, 2022年6月1日 创建本函数
**      $Log$
**
**.EH--------------------------------------------------------------------------------------
*/
template<typename T, unsigned int len>
bool LockfreeQueue<T, len>::full()
{
    /* 根据头尾游标判断队列是否为满 */
    if (_head == LOOP(_rear + 1, len)) {
        return true;
    }
    else {
        return false;
    }
}
/* END of full */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): front
**
** 描述(Description): 本函数实现获取队列头数据项的功能
**
** 输入参数(Input Parameter):
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**      const T& , 队列头数据项
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**      Inory, 2022年6月1日 创建本函数
**      $Log$
**
**.EH--------------------------------------------------------------------------------------
*/
template<typename T, unsigned int len>
const T& LockfreeQueue<T, len>::front()
{
    /* 返回队列头数据项 */
    return _items[_head];
}
/* END of front */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): pop
**
** 描述(Description): 本函数实现弹出队列头数据项的功能
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
**      Inory, 2022年6月1日 创建本函数
**      $Log$
**
**.EH--------------------------------------------------------------------------------------
*/
template<typename T, unsigned int len>
void LockfreeQueue<T, len>::pop()
{
    /* 队列不为空时,将头游标后移一项 */
    if (_head != _rear) {
        _head = LOOP(_head + 1, len);
    }
}
/* END of pop */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): push
**
** 描述(Description): 本函数实现压入队列数据项的功能
**
** 输入参数(Input Parameter):
**      item , const T&
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**      Inory, 2022年6月1日 创建本函数
**      $Log$
**
**.EH--------------------------------------------------------------------------------------
*/
template<typename T, unsigned int len>
bool LockfreeQueue<T, len>::push(const T& item)
{
    /* 根据头、尾游标判断队列是否为满 */
    if (_head == LOOP(_head + 1, len)) {
        return false;
    }

    /* 将数据项存入队列 */
    _items[_rear] = item;
    _rear = LOOP(_rear + 1, len);

    return true;
}
/* END of push */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): count
**
** 描述(Description): 本函数实现获取队列中数据项个数的功能
**
** 输入参数(Input Parameter):
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**      unsigned int 队列中数据项个数
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**      Inory, 2022年6月1日 创建本函数
**      $Log$
**
**.EH--------------------------------------------------------------------------------------
*/
template<typename T, unsigned int len>
unsigned int LockfreeQueue<T, len>::count()
{
    return LOOP(len + _rear - _head, len);
}
/* END of count */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): capacity
**
** 描述(Description): 本函数实现获取队列容量的功能
**
** 输入参数(Input Parameter):
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**      unsigned int 队列容量
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**      Inory, 2022年6月1日 创建本函数
**      $Log$
**
**.EH--------------------------------------------------------------------------------------
*/
template<typename T, unsigned int len>
unsigned int LockfreeQueue<T, len>::capacity()
{
    return (len - 1);
}
/* END of count */

#endif /* LOCKFREEQUEUE_H */
