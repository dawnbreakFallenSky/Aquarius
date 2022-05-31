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



#endif /* LOCKFREEQUEUE_H */
