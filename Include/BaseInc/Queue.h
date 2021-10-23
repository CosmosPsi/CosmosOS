/**********************************************************
		    队列头文件Queue.h
***********************************************************
				彭东
**********************************************************/
#ifndef _QUEUEHEAD
#define _QUEUEHEAD

// 队列元素
typedef struct QNODE
{
    List Node;
    void *Obj;
    void *Ext;
} QNode;

// 队列
typedef struct QUEUE
{
    List QHead;
    QNode *QCurr;
} Queue;

// 节点初始化
// @param init 节点
KLINE void QnodeInit(QNode *init)
{
    if (NULL == init)
    {
        return;
    }
    ListInit(&init->Node);
    init->Obj = NULL;
    init->Ext = NULL;
    return;
}

// 队列初始化
// @param init 队列
KLINE void QueueInit(Queue *init)
{
    if (NULL == init)
    {
        return;
    }
    ListInit(&init->QHead);
    init->QCurr = NULL;
    return;
}

// 队列是否为空
// @param q 队列
KLINE Bool QueueIsEmpty(Queue *q)
{
    if (NULL == q)
    {
        return TRUE;
    }
    if (ListIsEmptyCareful(&q->QHead) == TRUE)
    {
        return TRUE;
    }
    return FALSE;
}

// 从队列中弹出一个元素
// @param q 队列
KLINE QNode *QueuePop(Queue *q)
{
    List *prev;
    QNode *qn;
    if (NULL == q)
    {
        return NULL;
    }
    if (NULL == q->QCurr || ListIsEmptyCareful(&q->QHead) == TRUE)
    {
        return NULL;
    }

    qn = q->QCurr;
    prev = qn->Node.Prev;

    ListDel(&qn->Node);
    if (ListIsEmptyCareful(&q->QHead) == TRUE)
    {
        q->QCurr = NULL;
        return qn;
    }
    q->QCurr = ListEntry(prev, QNode, Node);
    return qn;
}

// 向队列中推入一个元素
// @param q  队列
// @param qn 元素
KLINE QNode *QueuePush(Queue *q, QNode *qn)
{
    if (NULL == q || NULL == qn)
    {
        return NULL;
    }
    ListAddTail(&qn->Node, &q->QHead);
    q->QCurr = qn;
    return qn;
}

// 向队列中加入一个元素
// @param q  队列
// @param qn 元素
KLINE QNode *QueueAdd(Queue *q, QNode *qn)
{
    if (NULL == q || NULL == qn)
    {
        return NULL;
    }
    ListAddTail(&qn->Node, &q->QHead);
    return qn;
}

// 从队列中拿出
// @param q 队列
KLINE QNode *QueuePoll(Queue *q)
{
    QNode *qn;
    if (NULL == q)
    {
        return NULL;
    }
    if (ListIsEmptyCareful(&q->QHead) == TRUE)
    {
        return NULL;
    }
    qn = ListFirstOneObj(&q->QHead, QNode, Node);
    ListDel(&qn->Node);
    return qn;
}

// 新建节点
// @param obj 节点数据 
KLINE QNode *NewQNode(void *obj)
{
    QNode *qn = (QNode *)kmsob_new(sizeof(QNode));
    if (NULL == qn)
    {
        return NULL;
    }
    QNodeInit(qn);
    qn->qn_obj = obj;
    return qn;
}

// 删除节点
// @param qn 节点 
KLINE Bool DelQonde(QNode *qn)
{
    if (NULL == qn)
    {
        return FALSE;
    }
    return kmsob_delete((void *)qn, sizeof(QNode));
}

#endif
