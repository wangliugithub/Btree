#include <iostream>
using namespace std;

//B+树结构定义

#define M 5
//叶子要存放整棵树的所有值，分支节点的值要放到叶子节点的data[0]位置，所以叶子节点值得数量不同于分支
#define LEAFMAX (M)
#define LEAFMIN (M/2+1)
#define BRCHMAX (M-1)
#define BRCHMIN (M/2)


typedef char KeyType;
typedef struct{}Record;

//标志是分支还是叶子节点
typedef enum{LEAF=0,BRCH=1}NodeType;

struct BNode
{
	NodeType utype;
	int num;
	KeyType data[M+1];
	BNode *parent;
	union
	{
		struct  
		{
			Record *recptr[M+1];
			BNode *prev,*next;
		}; // 叶子节点
		BNode *sub[M+1];//分支节点
	};
};

typedef struct  
{
	BNode *root; //指向根节点
	BNode *first;//指向叶子节点的第一个结点
}BTree;

typedef struct
{
	BNode *pnode;
	int index;
	bool tag;
}Result;

BNode * BuyNode()
{
	BNode *p = (BNode *)malloc(sizeof(BNode));
	if (p == NULL)
		exit(1);
	memset(p,0,sizeof(BNode));
	return p;
}
BNode *BuyLeaf()
{
	BNode *p = BuyNode();
	p->utype = LEAF;
	return p;
}
BNode *BuyBrch()
{
	BNode *p = BuyNode();
	p->utype = BRCH;
	return p;
}
void FreeNode(BNode *p)
{
	free(p);
}
void init_Btree(BTree *ptree)
{
	ptree->root = NULL;
	ptree->first = NULL;
}
//在叶子结点找key值
Result FindFirst(BNode *ptr,KeyType kx)
{
	Result res = {NULL,-1,false};
	while (ptr != NULL && ptr->next != NULL && 
		kx>ptr->data[ptr->num-1])
	{
		ptr = ptr->next;
	}
	if (ptr == NULL) return res;

	//锁定一个结点
	int pos = ptr->num-1;
	while (pos>=0 && kx<ptr->data[pos]) --pos;
	res.pnode = ptr;
	res.index = pos;

	if (pos>=0 && kx == ptr->data[pos])
	{
		res.tag = true;
	}
	if (pos<0 && ptr->prev != NULL)
	{
		res.pnode = ptr->prev;
		res.index = ptr->prev->num-1;//
	}
	return res;

}

void insert_item_to_leaf(BNode *ptr,int pos,KeyType kx,Record *rec)
{
	//理解错误:在pos之后插入
	int i = ptr->num-1;
	for (;i>pos;--i)
	{
		ptr->data[i+1] = ptr->data[i];
		ptr->recptr[i+1] = ptr->recptr[i];
	}

	ptr->data[pos+1] = kx;
	ptr->recptr[pos+1] = rec;

	ptr->num += 1;

}
void insert_item_to_brch(BNode *ptr,int pos,KeyType kx,BNode *right)
{
	if (ptr == NULL)
		return ;
	for (int i=ptr->num;i>pos;--i)
	{
		ptr->data[i+1] = ptr->data[i];
		ptr->sub[i+1] = ptr->sub[i];
	}
	ptr->data[pos+1] = kx;
	ptr->sub[pos+1] = right;
	if (right != NULL)
	{
		right->parent = ptr;
	}
	ptr->num += 1;

}
KeyType move_leaf_item(BNode *p,BNode *s,int pos)
{
	for (int i = pos,j=0;i<p->num;++i,++j)
	{
		s->data[j] = p->data[i];
		s->recptr[j] = p->recptr[i];
	}

	//把p,s串起来
	s->prev = p;
	s->next = p->next;
	p->next = s;
	if(s->next != NULL)
	{
		s->next->prev = s;
	}
	s->parent = p->parent;
	p->num = LEAFMIN;
	s->num = LEAFMIN;

	return s->data[0];
}
KeyType move_brch_item(BNode *ptr,BNode *s,int pos)
{
	for (int i=pos+1,j=0;i<=ptr->num;++i,++j)
	{
		s->data[j] = ptr->data[i];
		s->sub[j] = ptr->sub[i];
		if (s->sub[j] != NULL)
		{
			s->sub[j]->parent = s;
		}
	}
	s->parent = ptr->parent;
	s->num = BRCHMIN;
	ptr->num = BRCHMIN;
	return s->data[0];
}
BNode *Makeroot(BNode* left,KeyType kx,BNode *right)
{
	BNode *root = BuyBrch();
	root->parent = NULL;
	root->data[1] = kx;
	root->num = 1;

	root->sub[0] = left;
	left->parent = root;
	root->sub[1] = right;
	right->parent = root;
	return root;
}
BNode *splice_brch(BNode *p);
//对叶子节点进行调整
//可能产生新根
//可能会使分支节点也要进行分裂
BNode* splice_leaf(BNode *ptr)
{
	//首先要申请一个结点
	//购买结点怎么购买 分支or叶子
	//先进行叶子的分裂
	if (ptr == NULL)
		exit(1);
	if (ptr->utype == LEAF)
	{
		BNode *s = BuyLeaf();
		KeyType kx = move_leaf_item(ptr,s,LEAFMIN);

		if (ptr->parent == NULL)
		{
			return Makeroot(ptr,kx,s);
		}
		ptr = ptr->parent;
		int pos = ptr->num;
		ptr->data[0] = kx;
		while (kx < ptr->data[pos]) --pos;
		insert_item_to_brch(ptr,pos,kx,s); //这个决定了新生成的结点放在parent的哪个sub
		if (ptr->num >BRCHMAX)
		{
			return splice_brch(ptr);
		}
		else
			return NULL;
	}
}	
BNode *splice_brch(BNode *p)
{
	if (p->utype == BRCH)
	{
		BNode *s = BuyBrch();
		KeyType kx = move_brch_item(p,s,BRCHMIN);
		if (p->parent == NULL)
		{
			Makeroot(p,kx,s);
		}
		p = p->parent;
		int pos = p->num;
		p->data[0] = kx;
		while (kx < p->data[pos]) --pos;
		insert_item_to_brch(p,pos,kx,s); 
		if (p->num >BRCHMAX)
		{
			return splice_brch(p);
		}
		else
			return NULL;
	}
}
//B树的插入在叶子进行
bool insert(BTree *ptree,KeyType kx,Record *rec)
{
	//空树
	if (ptree->root == NULL)
	{
		BNode *s = BuyLeaf();
		s->num = 1;
		s->data[0] = kx;//叶子节点从0开始放数据
		s->recptr[0] = rec;
		ptree->first = ptree->root = s;
		return true;
	}
	//不空
	Result res = FindFirst(ptree->first,kx);
	if (res.pnode == NULL || res.tag)
		return false;
	
	BNode *ptr = res.pnode;
	int pos = res.index;
	//在ptr结点的pos位置插入
	insert_item_to_leaf(ptr,pos,kx,rec);
	
	//插入之后对叶子进行调整
	if (ptr->num > LEAFMAX)
	{
		BNode *newroot = splice_leaf(ptr);
		if (newroot != NULL)
		{
			ptree->root = newroot;
		}
	}
	return true;
}
void main()
{
	BTree myt;
	init_Btree(&myt);
	KeyType ar[]={"qwertyuiopasdfghjklzxcvbnm"};
	int n = sizeof(ar)/sizeof(ar[0]) -1;
	Record *rec = NULL;
	for(int i = 0;i<n;++i)
	{
		cout<<insert(&myt,ar[i],rec);
	}
		
}