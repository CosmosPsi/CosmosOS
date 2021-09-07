/**********************************************************
		链表头文件list.h
***********************************************************
				彭东
**********************************************************/
#ifndef _LISTHEAD
#define _LISTHEAD

typedef struct LIST {
       struct LIST *Prev;
	   struct LIST *Next;
}List;

KLINE void ListInit(List *list)
{
	list->Prev = list;
	list->Next = list;
	return;
}

KLINE void ListDelRealize(List *prev, List *next)
{
	next->Prev = prev;
	prev->Next = next;
	return;
}

KLINE void ListAddRealize(List *new, List *prev, List *next)
{
	next->Prev = new;
	new->Next = next;
	new->Prev = prev;
	prev->Next = new;
	return;
}

KLINE void ListAdd(List *new, List *head)
{
	ListAddRealize(new, head, head->Next);
	return;
}

KLINE void ListAddTail(List *new, List *head)
{
	ListAddRealize(new, head->Prev, head);
	return;
}

KLINE void ListDelEntryRealize(List *entry)
{
	ListDelRealize(entry->Prev, entry->Next);
	return;
}

KLINE void ListDel(List *entry)
{
	ListDelRealize(entry->Prev, entry->Next);
	ListInit(entry);
	return;
}

KLINE void ListMove(List *list, List *head)
{
	ListDel(list);
	ListAdd(list, head);
	return;
}
KLINE void ListMoveTail(List *list, List *head)
{
	ListDel(list);
	ListAddTail(list, head);
	return;
}

KLINE Bool ListIsEmpty(const List *head)
{
	if (head->Next == head)
	{
		return TRUE;
	}
	return FALSE;
}

KLINE Bool ListIsFirst(const List* list, const List* head)
{
	if(list->Prev == head)
	{
		return TRUE;
	}
	return FALSE;
}

KLINE Bool ListIsLast(const List* list, const List* head)
{
	if(list->Next == head)
	{
		return TRUE;
	}
	return FALSE;
}

KLINE Bool ListIsEmptyCareful(const List *head)
{
	List *Next = head->Next;
	if (Next == head && Next == head->Prev)
	{
		return TRUE;
	}
	return FALSE;
}

#define ListForEach(pos, head) for (pos = (head)->Next; pos != (head); pos = pos->Next)

#define ListForEachDeleteOneList(pos, head) for (pos = (head)->Next; pos != (head); pos = (head)->Next)

#define ListEntry(ptr, type, member) \
	((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))

#define ListFirstOne(head, o_type, o_member) ListEntry((head)->Next, o_type, o_member)

#define ListNextEntry(pos, type, member) \
	ListEntry((pos)->member.Next, type, member)


#define ListPrevEntry(pos, type, member) \
	ListEntry((pos)->member.Prev, type, member)
	

#endif
