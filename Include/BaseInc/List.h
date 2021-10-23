/**********************************************************
		链表头文件list.h
***********************************************************
				彭东
**********************************************************/
#ifndef _LISTHEAD
#define _LISTHEAD

// 双向链表
typedef struct LIST
{
	struct LIST *Prev;
	struct LIST *Next;
} List;

// 链表初始化
// @param list 链表实例
KLINE void ListInit(List *list)
{
	list->Prev = list;
	list->Next = list;
	return;
}

// 从链表从删除一个元素
// @param prev 前驱元素
// @param next 后继元素
KLINE void ListDelRealize(List *prev, List *next)
{
	next->Prev = prev;
	prev->Next = next;
	return;
}

// 从链表从添加一个元素
// @param new  新增元素
// @param prev 前驱元素
// @param next 后继元素
KLINE void ListAddRealize(List *new, List *prev, List *next)
{
	next->Prev = new;
	new->Next = next;
	new->Prev = prev;
	prev->Next = new;
	return;
}

// 向链表头添加一个元素
// @param new  新增元素
// @param head 头部元素
KLINE void ListAdd(List *new, List *head)
{
	ListAddRealize(new, head, head->Next);
	return;
}

// 向链表尾部添加一个元素
// @param new  新增元素
// @param head 尾部元素
KLINE void ListAddTail(List *new, List *head)
{
	ListAddRealize(new, head->Prev, head);
	return;
}

// 删除当前元素
// @param entry 待删除元素
KLINE void ListDelEntryRealize(List *entry)
{
	ListDelRealize(entry->Prev, entry->Next);
	return;
}

// 删除当前元素
// @param entry 待删除元素
KLINE void ListDel(List *entry)
{
	ListDelRealize(entry->Prev, entry->Next);
	ListInit(entry);
	return;
}

// 移动元素
// @param list 链表
// @param head 头部元素
KLINE void ListMove(List *list, List *head)
{
	ListDel(list);
	ListAdd(list, head);
	return;
}

// 移动元素到尾部
// @param list 链表
// @param head 头部元素
KLINE void ListMoveTail(List *list, List *head)
{
	ListDel(list);
	ListAddTail(list, head);
	return;
}

// 链表是否为空
// @param list 链表
KLINE Bool ListIsEmpty(const List *head)
{
	if (head->Next == head)
	{
		return TRUE;
	}
	return FALSE;
}

// 元素是否为头元素
// @param list 链表
// @param head 元素
KLINE Bool ListIsFirst(const List *list, const List *head)
{
	if (list->Prev == head)
	{
		return TRUE;
	}
	return FALSE;
}

// 元素是否为尾部元素
// @param list 链表
// @param head 元素
KLINE Bool ListIsLast(const List *list, const List *head)
{
	if (list->Next == head)
	{
		return TRUE;
	}
	return FALSE;
}

// 链表是否为空
// @param head 元素
KLINE Bool ListIsEmptyCareful(const List *head)
{
	List *Next = head->Next;
	if (Next == head && Next == head->Prev)
	{
		return TRUE;
	}
	return FALSE;
}

// 遍历链表
#define ListForEach(pos, head) for (pos = (head)->Next; pos != (head); pos = pos->Next)

// 遍历链表，删除链表
#define ListForEachDeleteOneList(pos, head) for (pos = (head)->Next; pos != (head); pos = (head)->Next)

// 得到链表中的一个元素
#define ListEntry(ptr, type, member) \
	((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))

// 得到链表的第一个元素
#define ListFirstOne(head, o_type, o_member) ListEntry((head)->Next, o_type, o_member)

// 得到下一个元素
#define ListNextEntry(pos, type, member) \
	ListEntry((pos)->member.Next, type, member)

// 得到上一个元素
#define ListPrevEntry(pos, type, member) \
	ListEntry((pos)->member.Prev, type, member)

#endif
