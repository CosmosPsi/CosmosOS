/**********************************************************
		    队列头文件Queue.h
***********************************************************
				彭东
**********************************************************/
#ifndef _QUEUEHEAD
#define _QUEUEHEAD
typedef struct QNODE
{
    List Node;
    void*  Obj;
    void*  Ext;
}QNode;

typedef struct QUEUE
{
    List QHead;
    QNode* QCurr;
}Queue;

KLINE void QnodeInit(QNode* init)
{
	if(NULL == init)
	{
		return;
	}
	ListInit(&init->Node);
	init->Obj = NULL;
	init->Ext = NULL;
	return;
}

KLINE void QueueInit(Queue* init)
{
	if(NULL == init)
	{
		return;
	}
	ListInit(&init->QHead);
	init->QCurr = NULL;
	return;
}

KLINE Bool QueueIsEmpty(Queue* q)
{
    if(NULL == q)
    {
        return TRUE;
    }
    if(ListIsEmptyCareful(&q->QHead) == TRUE)
    {
        return TRUE;
    }
    return FALSE;
}

KLINE QNode* QueuePop(Queue* q)
{
    List* prev;
    QNode* qn;
    if(NULL == q)
    {
        return NULL;
    }
    if(NULL == q->QCurr || ListIsEmptyCareful(&q->QHead) == TRUE)
    {
        return NULL;
    }

    qn = q->QCurr;
    prev = qn->Node.Prev;

    ListDel(&qn->Node);
    if(ListIsEmptyCareful(&q->QHead) == TRUE)
    {
        q->QCurr = NULL;
        return qn;
    }
    q->QCurr = ListEntry(prev, QNode, Node);
    return qn;
}

KLINE QNode* QueuePush(Queue* q, QNode* qn)
{
    if(NULL == q || NULL == qn)
    {
        return NULL;
    }
    ListAddTail(&qn->Node, &q->QHead);
    q->QCurr = qn;
    return qn;
}

KLINE QNode* QueueAdd(Queue* q, QNode* qn)
{
    if(NULL == q || NULL == qn)
    {
        return NULL;
    }
    ListAddTail(&qn->Node, &q->QHead);
    return qn;
}

KLINE QNode* QueuePoll(Queue* q)
{
    QNode* qn;
    if(NULL == q)
    {
        return NULL;
    }
    if(ListIsEmptyCareful(&q->QHead) == TRUE)
    {
        return NULL;
    }
    qn = ListFirstOneObj(&q->QHead, QNode, Node);
    ListDel(&qn->Node);
    return qn;
}

KLINE QNode* NewQNode(void* obj)
{
    QNode* qn = (QNode*)kmsob_new(sizeof(QNode));
    if(NULL == qn)
    {
        return NULL;
    } 
    QNodeInit(qn);
    qn->qn_obj = obj;
    return qn;
}

KLINE Bool DelQonde(QNode* qn)
{
    if(NULL == qn)
    {
        return FALSE;
    }
    return kmsob_delete((void*)qn, sizeof(QNode));
}

#endif
