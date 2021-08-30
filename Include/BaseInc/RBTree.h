/**********************************************************
		    红黑树头文件RBTree.h
***********************************************************
				彭东
**********************************************************/
#ifndef _RBTREEHEAD
#define _RBTREEHEAD

#define RBRED (0)
#define RBBLACK (1)


#define RBERR (0)
#define RBLEFT (1)
#define RBRIGHT (2)

typedef struct RBTFLAGS 
{
    U16 Type;
	U16 Color;
	U32 Hight;
}__attribute__((packed)) RBTFlags;


typedef struct RBTREE
{
	RBTFlags Flags;
	struct RBTREE* Left;
	struct RBTREE* Right;
	struct RBTREE* Parent;
}RBTree;

typedef struct RBROOT
{
	UInt Count;
	RBTree* MostLeft;
	RBTree* Node;
	RBTree* MostRight;
}RBRoot;

typedef UInt (*RBPathCMP)(RBTree* srcrb, RBTree* cmprb);
typedef UInt (*RBRePlace)(RBTree* srcrb, RBTree* reprb);
typedef UInt (*RBDelAfter)(RBTree* delrb);

KLINE void RBTreeInit(RBTree* init)
{
	if(NULL == init)
	{
		return;
	}
	init->Flags.Color = RBRED;
	init->Flags.Type = 0;
	init->Flags.Hight = 0;
	init->Left = NULL;
	init->Right = NULL;
	init->Parent = NULL;
	return;
}

KLINE void RBRootInit(RBRoot* init)
{
	if(NULL == init)
	{
		return;
	}
	init->Count = 0;
	init->MostLeft = NULL;
	init->Node = NULL;
	init->MostRight = NULL;
	return;
}

KLINE void RBTreeSetColor(RBTree* rbtree, U16 color)
{
	if(NULL == rbtree)
	{
		return;
	}
	rbtree->Flags.Color = color;
	return;
}

KLINE Bool RBTreeColorIsRed(RBTree* rbtree)
{
	if(NULL == rbtree)
	{
		return FALSE;
	}
	if(RBRED == rbtree->Flags.Color)
	{
		return TRUE;
	}
	return FALSE;
}

KLINE Bool RBTreeColorIsBlack(RBTree* rbtree)
{
	if(NULL == rbtree)
	{
		return TRUE;
	}
	if(RBBLACK == rbtree->Flags.Color)
	{
		return TRUE;
	}
	return FALSE;
}

KLINE RBTree* RBTreeSibling(RBTree* rbtree)
{
	RBTree* parent;
	if(NULL == rbtree)
	{
		return NULL;
	}
	parent = rbtree->Parent;
	if(NULL == parent)
	{
		return NULL;
	}
	if(parent->Left == rbtree)
	{
		return parent->Right;
	}
	else
	{
		return parent->Left;
	}
}

KLINE RBTree* RBTreeUncle(RBTree* rbtree)
{
	if(NULL == rbtree)
	{
		return NULL;
	}
	if(NULL == rbtree->Parent)
	{
		return NULL;
	}
	return RBTreeSibling(rbtree->Parent);
}

KLINE RBTree* RBTreeParent(RBTree* rbtree)
{
	if(NULL == rbtree)
	{
		return NULL;
	}
	return rbtree->Parent;
}

KLINE Bool RBTreeIsRight(RBTree* rbtree)
{
	RBTree* parent;
	if(NULL == rbtree)
	{
		return FALSE;
	}
	parent = rbtree->Parent;
	if(NULL == parent)
	{
		return FALSE;
	}
	if(parent->Right == rbtree)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


KLINE bool_t RBTreeIsLeft(RBTree* rbtree)
{
	RBTree* parent;
	if(NULL == rbtree)
	{
		return FALSE;
	}
	parent = rbtree->Parent;
	if(NULL == parent)
	{
		return FALSE;
	}
	if(parent->Left == rbtree)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

KLINE Bool RBTreeIsRoot(RBTree* rbtree)
{
	if(NULL == rbtree)
	{
		return FALSE;
	}
	if(NULL == rbtree->Parent)
	{
		return TRUE;
	}
	return FALSE;
}

KLINE Bool RBTreeIsLeaf(RBTree* rbtree)
{
	if(NULL == rbtree)
	{
		return FALSE;
	}
	if((NULL == rbtree->Left) && (NULL == rbtree->Right))
	{
		return TRUE;
	}
	return FALSE;
}


KLINE RBTree* RBTreeGrand(RBTree* rbtree)
{
	if(NULL == rbtree)
	{
		return NULL;
	}
	if(NULL == rbtree->Parent)
	{
		return NULL;
	}
	return rbtree->Parent->Parent;	
}

/*
查找前驱节点
即 查找小于当前节点的最大节点
*/

KLINE RBTree* RBTreeFindPrecursor(RBTree* rbtree)
{
	RBTree* rbt;
	RBTree* p;
	if(NULL == rbtree)
	{
		return NULL;
	}	
	else if(NULL != rbtree->Left)
	{
		rbt = rbtree->Left;
		while(NULL != rbt->Right)
		{
			rbt = rbt->Right;
		}
		return rbt;
	}
	else
	{
		rbt = rbtree;
		p = rbt->Parent;
		while (NULL != p && rbt == p->Left)
		{
			rbt = p;
			p = p->Parent;
		}
		return p;
	}
	return NULL;
}

/*
查找后继节点
即 查找大于当前节点的最小节点
*/
KLINE RBTree* RBTreeFindSuccessor(RBTree* rbtree)
{
	RBTree* rbt;
	RBTree* p;
	if(NULL == rbtree)
	{
		return NULL;
	}	
	else if(NULL != rbtree->Right)
	{
		rbt = rbtree->Right;
		while(NULL != rbt->Left)
		{
			rbt = rbt->Left;
		}
		return rbt;
	}
	else
	{
		rbt = rbtree;
		p = rbt->Parent;
		while (NULL != p && rbt == p->Right)
		{
			rbt = p;
			p = p->Parent;
		}
		return p;
	}
	return NULL;
}


/*
		P				  12
	   /                  /
	  PLA                8 
     /   \              / \
    ALB  ARC           7  10        
	    /   \             / \
	   CLE  CRF          9  11   
		
		P				 12
	   /                 /       
      ARC			    10
	 /   \             /  \
	PLA  CRF          8   11
   /   \             / \
  ALB CLE           7   9

第1：变色
条件：
    （1）当前节点是红色
    （2）当前节点的父亲节点是红色
    （3）当前节点的叔叔节点是红色
操作：
    （1）当前节点的父亲节点变黑色    
    （2）当前节点的叔叔节点变黑色
    （3）当前节点的爷爷节点变红色    
    （4）当前节点的爷爷节点设为当前节点 


第2：左旋
条件：
    （1）当前节点的父亲节点是红色
    （2）当前节点的叔叔节点是黑色
    （3）当前节点是父亲节点的右子树
操作：
    （1）当前节点的父亲节点为基础进行左旋 

第3：右旋
条件：
    （1）当前节点的父亲节点是红色
    （2）当前节点的叔叔节点是黑色
    （3）当前节点是父亲节点的左子树
操作：
    （1）当前节点的父亲节点变成黑色
    （2）当前节点的爷爷节点变成红色
    （3）当前节点的爷爷节点为基础进行右旋 
*/



KLINE RBTree* RBTreeRightRotate(RBRoot* rbroot, RBTree* rbtree)
{
	RBTree* P;
	RBTree* R;
	RBTree* RLN;
	RBTree* TR;
	if(NULL == rbtree || NULL == rbroot)
	{
		return NULL;
	}
	
	R = rbtree;
	P = R->Parent;
	RLN = R->Left;
    TR = RLN->Right;
	if(NULL == RLN)
	{
		return NULL;
	}
	if(NULL != TR)
	{
		TR->Parent = R;
	}
    R->Left = TR;
    RLN->Right = R;
    R->Parent = RLN;
    RLN->Parent = P;

    if(NULL == P)
    {
        rbroot->Node = RLN;
        return R;
    }
    else
    {
        if(R == P->Left)
        {
            P->Left = RLN;
        }
        else
        {
            P->Right = RLN;
        }
        return R;
    }
	return NULL;
}

KLINE RBTree* RBTreeLeftRotate(RBRoot* rbroot, RBTree* rbtree)
{
	RBTree* P;
	RBTree* R;
	RBTree* RRN;
	RBTree* TL;
	if(NULL == rbtree || NULL == rbroot)
	{
		return NULL;
	}
	
	R = rbtree;
	P = R->Parent;
	RRN = R->Right;
    TL = RRN->Left;

	if(NULL == RRN)
	{
		return NULL;
	}

	if(NULL != TL)
	{
		TL->Parent = R;	
	}

	R->Right = TL;
    RRN->Left = R;
    R->Parent = RRN;
    RRN->Parent = P;

    if(NULL == P)
    {
        rbroot->Node = RRN;
        return R;
    }
    else
    {
        if(R == P->Left)
        {
            P->Left = RRN;
        }
        else
        {
            P->Right = RRN;
        }
        return R;
    }
	return NULL;
}



KLINE RBTree* RBTreeFixColorReal(RBRoot* rbroot, RBTree* rbtree)
{
	RBTree* rbt;
	if(NULL == rbtree || NULL == rbroot)
	{
		return NULL;
	}
	rbt = rbtree;
	while(rbt)
	{
		if(RBTreeIsRoot(rbt) == TRUE)
		{
			RBTreeSetColor(rbt, RBBLACK);
			return rbt;
		}

		if(RBTreeColorIsBlack(RBTreeParent(rbt)) == TRUE)
		{
			return rbt;
		}

		if((RBTreeColorIsRed(rbt) == TRUE) && 
				(RBTreeColorIsRed(RBTreeParent(rbt)) == TRUE) && 
				(RBTreeColorIsRed(RBTreeUncle(rbt)) == TRUE))
		{
			RBTreeSetColor(RBTreeParent(rbt), RBBLACK);
			RBTreeSetColor(RBTreeUncle(rbt), RBBLACK);
			RBTreeSetColor(RBTreeGrand(rbt), RBRED);
			rbt = RBTreeGrand(rbt);
		}
		else
		{
			return rbt;
		}
	}
	return NULL;
}



/*
(LL)
	4B
	/
  2R
  /
 1R
父节点变成黑色，爷爷节点变成红色
	4R
	/
  2B
  /
 1R
爷爷节点右旋
	2B
	/ \
  1R  4R 
  
(RR)
	2B
	  \
      3R
	   \
 	   4R
父节点变成黑色，爷爷节点变成红色
	2R
	  \
      3B
	   \
 	   4R
爷爷节点左旋
	3B
	/ \
  2R  4R 

(LR)
	4B
	/
  2R
    \
	3R
当前节点变成黑色，爷爷节点变成红色
	4R
	/
  2R
    \
	3B
父节点左旋
	4R
	/
  3B
  /
 2R
爷爷节点右旋
	3B
	/ \
  2R  4R

(RL)
	2B
	  \
	   4R
      /
	 3R
当前节点变成黑色，爷爷节点变成红色
	2R
	  \
	   4R
      /
	 3B
父节点右旋
	2R
	  \
	   3B
      	\
	 	4R 
爷爷节点左旋
	3B
   /  \
  2R  4B
*/

KLINE RBTree* RBTreeFixRotate(RBRoot* rbroot, RBTree* rbtree)
{
	if(NULL == rbtree || NULL == rbroot)
	{
		return NULL;
	}

	if(RBTreeColorIsRed(RBTreeParent(rbtree)) == FALSE ||
			 RBTreeColorIsBlack(RBTreeUncle(rbtree)) == FALSE)
	{
		return NULL;
	}

	if(RBTreeIsLeft(RBTreeParent(rbtree)) == TRUE)//L
	{

		if(RBTreeIsLeft(rbtree) == TRUE)//LL父节点变成黑色，爷爷节点变成红色,爷爷节点右旋
		{
			RBTreeSetColor(RBTreeParent(rbtree), RBBLACK);
			RBTreeSetColor(RBTreeGrand(rbtree), RBRED);
			RBTreeRightRotate(rbroot, RBTreeGrand(rbtree));
			return rbtree;
		}
		else //LR 当前节点变成黑色，爷爷节点变成红色,父节点左旋,爷爷节点右旋
		{
			RBTreeSetColor(rbtree, RBBLACK);
			RBTreeSetColor(RBTreeGrand(rbtree), RBRED);
			RBTreeLeftRotate(rbroot, RBTreeParent(rbtree));
			RBTreeRightRotate(rbroot, RBTreeGrand(rbtree));
			return rbtree;
		}
	}
	else//R
	{
		if(RBTreeIsLeft(rbtree) == TRUE)//RL当前节点变成黑色，爷爷节点变成红色,父节点右旋,爷爷节点左旋
		{
			RBTreeSetColor(rbtree, RBBLACK);
			RBTreeSetColor(RBTreeGrand(rbtree), RBRED);
			RBTreeRightRotate(rbroot, RBTreeParent(rbtree));
			RBTreeLeftRotate(rbroot, RBTreeGrand(rbtree));
			return rbtree;
		}
		else//RR 父节点变成黑色，爷爷节点变成红色,爷爷节点左旋
		{
			RBTreeSetColor(RBTreeParent(rbtree), RBBLACK);
			RBTreeSetColor(RBTreeGrand(rbtree), RBRED);
			RBTreeLeftRotate(rbroot, RBTreeGrand(rbtree));
			return rbtree;
		}
	}
	return NULL;
}



KLINE RBTree* RBTreeAdd(RBRoot* root, RBTree* rbtree, RBPathCMP cmper)
{
	RBTree* srcrb;
	uint_t rets;
	if(NULL == root || NULL == rbtree || NULL == cmper)
	{
		return NULL;
	}
	if(NULL == root->Node)
	{
		root->Node = rbtree;
		rbtree->Parent = NULL;
		RBTreeSetColor(rbtree, RBBLACK);
		return rbtree;
	}
	srcrb = root->Node;
	while(NULL != srcrb)
	{
		rets = cmper(srcrb, rbtree);
		if(RBLEFT == rets)
		{
			if(NULL == srcrb->Left)
			{
				srcrb->Left = rbtree;
				rbtree->Parent = srcrb;
				break;
			}
			srcrb = srcrb->Left;
		}
		else if(RBRIGHT == rets)
		{
			if(NULL == srcrb->Right)
			{
				srcrb->Right = rbtree;
				rbtree->Parent = srcrb;
				break;
			}
			srcrb = srcrb->Right;
		}
		else
		{
			return NULL;
		}
	}
	return rbtree;
}

KLINE RBTree* RBTreeSerch(RBRoot* root, u32_t key)
{
	RBTree* srcrb;
	// uint_t rets;
	if(NULL == root)
	{
		return NULL;
	}
	if(NULL == root->Node)
	{
	
		return NULL;
	}
	srcrb = root->Node;
	while(NULL != srcrb)
	{
		// rets = cmper(srcrb, rbtree);
		if(key == srcrb->Flags.Hight)
		{
			return srcrb;
		}
		if(key < srcrb->Flags.Hight)
		{
			srcrb = srcrb->Left;
		}
		else
		{
			srcrb = srcrb->Right;
		}
	}
	return NULL;	
}

KLINE RBTree* RBTreeAddAfterFix(RBRoot* rbroot, RBTree* rbtree)
{
	RBTree* node;
	RBTree* uncle;
	if(NULL == rbroot || NULL == rbtree)
	{
		return NULL;
	}
	node = rbtree;
	while((NULL != node) && (rbroot->Node != node) && (RBTreeColorIsRed(RBTreeParent(node)) == TRUE))
	{
		if(RBTreeIsLeft(RBTreeParent(node)) == TRUE)
		{
			uncle = RBTreeGrand(node)->Right;
			if(RBTreeColorIsRed(uncle) == TRUE)
			{
				RBTreeSetColor(RBTreeParent(node), RBBLACK);
				RBTreeSetColor(uncle, RBBLACK);
				RBTreeSetColor(RBTreeGrand(node), RBRED);
				node = RBTreeGrand(node);
			}
			else
			{
				if(RBTreeIsRight(node) == TRUE)
				{
					node = RBTreeParent(node);
					RBTreeLeftRotate(rbroot, node);
				}
				RBTreeSetColor(RBTreeParent(node), RBBLACK);
				RBTreeSetColor(RBTreeGrand(node), RBRED);
				RBTreeRightRotate(rbroot, RBTreeGrand(node));
			}
		}
		else
		{
			uncle = RBTreeGrand(node)->Left;
			if(RBTreeColorIsRed(uncle) == TRUE)
			{
				RBTreeSetColor(RBTreeParent(node), RBBLACK);
				RBTreeSetColor(uncle, RBBLACK);
				RBTreeSetColor(RBTreeGrand(node), RBRED);
				node = RBTreeGrand(node);
			}
			else
			{
				if(RBTreeIsLeft(node) == TRUE)
				{
					node = RBTreeParent(node);
					RBTreeRightRotate(rbroot, node);
				}
				RBTreeSetColor(RBTreeParent(node), RBBLACK);
				RBTreeSetColor(RBTreeGrand(node), RBRED);
				RBTreeLeftRotate(rbroot, RBTreeGrand(node));
			}
		}
	}
	RBTreeSetColor(rbroot->Node, RBBLACK);
	return rbtree;
}

KLINE RBTree* RBTreeAddAfter(RBRoot* rbroot, RBTree* rbtree)
{
	RBTree* rbt;
	if(NULL == rbtree)
	{
		return NULL;
	}
	rbt = rbtree;
	while(NULL != rbt)
	{
		rbt = RBTreeFixColorReal(rbroot, rbt);
	
		rbt = RBTreeFixRotate(rbroot, rbt); 
	}
	return rbtree;
}

KLINE RBTree* RBTreeInsert(RBRoot* root, RBTree* rbtree, RBPathCMP cmper)
{
	RBTree* rbt;
	rbt = RBTreeAdd(root, rbtree, cmper);
	return RBTreeAddAfterFix(root, rbtree);
}

KLINE RBTree* RBTreeRemoveAfterFix(RBRoot* root, RBTree* rbtree)
{
	RBTree* rsib;
	RBTree* rbnode;
	if(NULL == rbtree || NULL == root)
	{
		return NULL;
	}
	rbnode = rbtree;
	while((rbnode != root->Node) && (RBTreeColorIsBlack(rbnode) == TRUE))
	{
		if(RBTreeIsLeft(rbnode) == TRUE)
		{
			rsib = rbnode->Parent->Right;
			if((RBTreeColorIsRed(rsib) == TRUE))
			{
				RBTreeSetColor(rsib, RBBLACK);
				RBTreeSetColor(rbnode->Parent, RBRED);
				RBTreeLeftRotate(root, rbnode->Parent);
				rsib = rbnode->Parent->Right;
			}

			if((RBTreeColorIsBlack(rsib->Left) == TRUE)&&
					(RBTreeColorIsBlack(rsib->Right) == TRUE))
			{
				RBTreeSetColor(rsib, RBRED);
				rbnode = rbnode->Parent;
			}
			else
			{
				if((RBTreeColorIsBlack(rsib->Right) == TRUE))
				{
					RBTreeSetColor(rsib->Left, RBBLACK);
					RBTreeSetColor(rsib, RBRED);
					RBTreeRightRotate(root, rsib);
					rsib = rbnode->Parent->Right;
				}
				RBTreeSetColor(rsib, rbnode->Parent->Flags.Color);
				RBTreeSetColor(rbnode->Parent, RBBLACK);
				RBTreeSetColor(rsib->Right, RBBLACK);
				RBTreeLeftRotate(root, rbnode->Parent);
				rbnode = root->Node;
			}
		}
		else
		{
			rsib = rbnode->Parent->Left;
			if((RBTreeColorIsRed(rsib) == TRUE))
			{
				RBTreeSetColor(rsib, RBBLACK);
				RBTreeSetColor(rbnode->Parent, RBRED);
				RBTreeRightRotate(root, rbnode->Parent);
				rsib = rbnode->Parent->Left;
			}

			if((RBTreeColorIsBlack(rsib->Right) == TRUE)&&
					(RBTreeColorIsBlack(rsib->Left) == TRUE))
			{
				RBTreeSetColor(rsib, RBRED);
				rbnode = rbnode->Parent;
			}
			else
			{
				if((RBTreeColorIsBlack(rsib->Left) == TRUE))
				{
					RBTreeSetColor(rsib->Right, RBBLACK);
					RBTreeSetColor(rsib, RBRED);
					RBTreeLeftRotate(root, rsib);
					rsib = rbnode->Parent->Left;
				}
				RBTreeSetColor(rsib, rbnode->Parent->Flags.Color);
				RBTreeSetColor(rbnode->Parent, RBBLACK);
				RBTreeSetColor(rsib->Left, RBBLACK);
				RBTreeRightRotate(root, rbnode->Parent);
				rbnode = root->Node;
			}
		}
	}
	RBTreeSetColor(rbnode, RBBLACK);
	return rbnode;
}

KLINE RBTree* RBTreeRemove(RBRoot* root, RBTree* rbtree, RBRePlace reper, RBDelAfter deler)
{
	RBTree* p;
	RBTree* after;
	RBTree* rbtnode;
	RBTree* rbtrepl;
	if(NULL == rbtree || NULL == root || NULL == reper || NULL == deler)
	{
		return NULL;
	}
	rbtnode = rbtree;
	p = rbtnode->Parent;

	if((NULL != rbtnode->Left) && (NULL != rbtnode->Right))
	{
		after = RBTreeFindSuccessor(rbtnode);
		if(NULL == after)
		{
			return NULL;
		}
		reper(rbtnode, after);
		rbtnode = after;
	}

	rbtrepl = rbtnode->Left != NULL ? rbtnode->Left : rbtnode->Right;

	if(NULL != rbtrepl)
	{
		rbtrepl->Parent = rbtnode->Parent;
		if(NULL == rbtrepl->Parent)
		{
			root->Node = rbtrepl;
		}
		else if(RBTreeIsLeft(rbtnode) == TRUE)
		{
			rbtnode->Parent->Left = rbtrepl;
		}
		else
		{
			rbtnode->Parent->Right = rbtrepl;
		}

		rbtnode->Right = rbtnode->Left = rbtnode->Parent = NULL;

		if(RBTreeColorIsBlack(rbtnode) == TRUE)
		{
			RBTreeRemoveAfterFix(root, rbtrepl);
		}
		deler(rbtnode);
	}
	else if(NULL == rbtnode->Parent)
	{
		root->Node = NULL;
		deler(rbtnode);
	}
	else
	{
		if(RBTreeColorIsBlack(rbtnode) == TRUE)
		{
			RBTreeRemoveAfterFix(root, rbtnode);
		}
		if(NULL != rbtnode->Parent)
		{
			if(RBTreeIsLeft(rbtnode) == TRUE)
			{
				rbtnode->Parent->Left = NULL;
			}
			else if(RBTreeIsRight(rbtnode) == TRUE)
			{
				rbtnode->Parent->Right = NULL;
			}
			rbtnode->Parent = NULL;
			deler(rbtnode);
		}
	}
	return NULL;
}

KLINE RBTree* RBTreeDelete(RBRoot* root, RBTree* rbtree, RBRePlace reper, RBDelAfter deler)
{
	RBTree* rbt;
	rbt = RBTreeRemove(root, rbtree, reper, deler);
	return rbt;
}

KLINE RBTree* RBTreeMostLeft(RBRoot* root)
{
	RBTree* rbtree;
	if(NULL == root)
	{
		return NULL;
	}
	rbtree = root->Node;
	if(NULL == rbtree)
	{
		return NULL;
	}
	while(NULL != rbtree)
	{
		if(NULL != rbtree->Left)
		{
			rbtree = rbtree->Left;
		}
		else if(NULL != rbtree->Right)
		{
			rbtree = rbtree->Right;
		}
		else
		{
			return rbtree;
		}
	}
	return NULL;
}

KLINE RBTree* RBTreeMostRight(RBRoot* root)
{
	RBTree* rbtree;
	if(NULL == root)
	{
		return NULL;
	}
	rbtree = root->Node;
	if(NULL == rbtree)
	{
		return NULL;
	}
	while(NULL != rbtree)
	{
		if(NULL != rbtree->Right)
		{
			rbtree = rbtree->Right;
		}
		else if(NULL != rbtree->Left)
		{
			rbtree = rbtree->Left;
		}
		else
		{
			return rbtree;
		}
	}
	return NULL;
}


#define RBTreeEntry(ptr, type, member) \
	((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))

	
#endif
