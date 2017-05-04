
#include<iostream>
using namespace std;

///B树:m阶B树是一颗平衡的m路搜索树，平衡的多叉树。

#define M 5 
#define MAX (M-1) //最大元素个数
#define MIN (M/2) //最小元素个数

typedef char KeyType;
typedef struct {}Record; //??????

typedef struct //键值对
{
	KeyType key;
	Record *recptr; //??????id 所对应的值
}ElemType; //结点的元素类型




typedef struct BNode //B树结点
{
	int num;//当前元素个数
	BNode *parent; 
	ElemType data[M+1]; //data[0]做哨兵位，data[5]做分裂用
	BNode *sub[M+1];

}BNode,*BTree;

//作为查询元素时存放结果的，当中包含该元素的指针和该元素的位置信息，以及是否查询到的标志位
typedef struct  
{
	BNode *pnode;
	int index;
	bool tag;
}Result;

BNode *BuyNode()
{
	BNode *p = (BNode *)malloc(sizeof(BNode));
	if (p == NULL)
		exit(1);
	memset(p,0,sizeof(BNode));	
	return p;
}

void FreeNode(BNode *p)
{
	free(p);
}

Result FindValue(BNode *ptr,KeyType kx)
{
	Result res = { NULL,-1,false};
	while(ptr != NULL)
	{
		int i = ptr->num;
		ptr->data[0].key = kx;
		while(kx < ptr->data[i].key) --i;
		res.pnode = ptr;
		res.index = i;
		if(i > 0 && ptr->data[i].key == kx)
		{
			res.tag = true;
			break;
		}
		ptr = ptr->sub[i];
	}
	return res;
}

BNode* MakeNode(ElemType kx,BNode *left,BNode *right)
 {
	BNode *p = BuyNode();
	if (p == NULL)
		exit(-1);
	p->data[1] = kx;
	p->num=1;
	p->sub[0]=left;
	if (left != NULL)
	{
		left->parent=p;
	}
	p->sub[1]=right;
	if (right != NULL)
	{
		right->parent=p;
	}
	return p;
}

void insert_item(BNode *p,int pos,ElemType kx,BNode *right)
{
	if (p == NULL)
		return;
	for (int i = p->num;i>pos;--i)
	{
		p->data[i+1] = p->data[i];
		p->sub[i+1] = p->sub[i];
	}
	p->data[pos+1] = kx;
	p->sub[pos+1] = right;
	if (right != NULL)
	{
		right->parent = p;
	}
	p->num +=1;
}

ElemType move_item(BNode *p,BNode *s,int pos)
{
	int i = pos+1;
	int j = 0;
	for (;i<=p->num;++i,++j)
	{
		s->data[j] = p->data[i];
		s->sub[j] = p->sub[i];
		if (s->sub[j] != NULL)
		{
			s->sub[j]->parent = s;
		}
	}
	s->parent = p->parent;
	p->num = MIN;
	s->num = MIN;
	return s->data[0];
}

BNode *Splice(BNode *p)
{
	if (p == NULL)
		exit(-1);
	BNode *s = BuyNode();
	if (s == NULL)
		exit(-1);
	ElemType x = move_item(p,s,MIN);
	if (p->parent == NULL)
	{
		return MakeNode(x,p,s);
	}

	p = p->parent;
	int pos = p->num;
	p->data[0] = x;
	while (x.key < p->data[pos].key) --pos;
	insert_item(p,pos,x,s);
	if (p->num >MAX)
	{
		return Splice(p);
	}
	else
		return NULL;
}

//B树的插入
bool insert(BTree *ptr,ElemType kx)
{
	if (ptr == NULL)
		return false;
	if (*ptr == NULL)
	{
		*ptr = MakeNode(kx,NULL,NULL);
		return true;
	}
	Result res={NULL,-1,false};
	res = FindValue(*ptr,kx.key);

	if (res.pnode == NULL || res.tag)
		return false;

	BNode *p = res.pnode;
	int pos = res.index;
	insert_item(p,pos,kx,NULL);

	if (p->num > MAX)
	{
		BNode *newroot = Splice(p);
		if (newroot != NULL)
		{
			*ptr = newroot;
		}
	}
	return true;
}

void InOrder(BNode *ptr)
{
	if (ptr!= NULL)
	{
		InOrder(ptr->sub[0]);
		for (int i = 1;i<=ptr->num;++i)
		{
			cout<<ptr->data[i].key;
			InOrder(ptr->sub[i]);
		}
	}
}

BNode *FindPre(BNode *ptr)
{
	while (ptr != NULL && ptr->sub[ptr->num] != NULL)
	{
		ptr = ptr->sub[ptr->num];
	}
	return ptr;
}

BNode *FindNext(BNode *ptr)
{
	while (ptr != NULL && ptr->sub[0] != NULL)
	{
		ptr = ptr->sub[0];
	}
	return ptr;
}

void Del_Leaf_Item(BNode *p,int pos)
{
	for (int i=pos;i<p->num;++i)
	{
		p->data[i] = p->data[i+1];
		p->sub[i] = p->sub[i+1];
	}
	p->num -= 1;
}


void main()
{
	KeyType ar[]={"qw6e7rty9uiopa0sdfg1hj2kl3zx4cv5bn8m"};
	
	int n = sizeof(ar)/sizeof(ar[0]) -1;
	BTree root = NULL;
	ElemType item;
	for(int i = 0;i<n;++i)
	{
		item.key = ar[i];
		item.recptr = NULL;
		cout<<insert(&root,item);
	}
	cout<<endl;
	InOrder(root);
	cout<<endl;
}





