
#include<iostream>
using namespace std;

///B��:m��B����һ��ƽ���m·��������ƽ��Ķ������

#define M 5 
#define MAX (M-1) //���Ԫ�ظ���
#define MIN (M/2) //��СԪ�ظ���

typedef char KeyType; //key
typedef struct {}Record; //??????

typedef struct //��ֵ��
{
	KeyType key;
	Record *recptr; //??????id ����Ӧ��ֵ
}ElemType; //����Ԫ������


typedef struct BNode //B�����
{
	int num;//��ǰԪ�ظ���
	BNode *parent; 
	ElemType data[M+1]; //data[0]���ڱ�λ��data[5]��������
	BNode *sub[M+1];

}BNode,*BTree;


//��Ϊ��ѯԪ��ʱ��Ž���ģ����а�����Ԫ�ص�ָ��͸�Ԫ�ص�λ����Ϣ���Լ��Ƿ��ѯ���ı�־λ
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
//right��sub[pos+1]ָ��Ľ��
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
	insert_item(p,pos,x,s); //��������������ɵĽ�����parent���ĸ�sub
	if (p->num >MAX)
	{
		return Splice(p);
	}
	else
		return NULL;
}

//B���Ĳ���
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

void RightRotateLeaf(BNode *leftbro, BNode*ptr, BNode *parent, int pos)
{
	ptr->data[0] = parent->data[pos];
	for (int i = ptr->num; i >= 0; i--)
	{
		ptr->data[i + 1] = ptr->data[i];
		ptr->sub[i + 1] = ptr->sub[i];
	}
	ptr->num += 1;

	ptr->sub[0] = leftbro->sub[leftbro->num];
	if (ptr->sub[0] != NULL)//
	{
		ptr->sub[0]->parent = ptr;
	}
	parent->data[pos] = leftbro->data[leftbro->num];
	leftbro->num -= 1;
}

void LeftRotateLeaf(BNode *rightbro,BNode *ptr,BNode *parent,int pos)
{
	ptr ->data[ptr->num+1] = parent->data[pos + 1];

	ptr->sub[ptr->num + 1] = rightbro->sub[0];

	if (ptr->sub[ptr->num+1]!=NULL)
	{
		ptr->sub[ptr->num + 1]->parent = ptr;
	}
	ptr->num += 1;

	parent->data[pos + 1] = rightbro->data[1];

	for (int i =0; i < rightbro->num; i++)
	{
		rightbro->data[i] = rightbro->data[i + 1];
		rightbro->sub[i] = rightbro->sub[i + 1];
	}
	rightbro->num -= 1;

}
void LeftMerge(BNode*leftbro, BNode*ptr, BNode*parent, int pos)
{
	ptr->data[0] = parent->data[pos];

	for (int i = 0,j=leftbro->num+1; i <= ptr->num; i++,j++)
	{
		leftbro->data[j] = ptr->data[i];
		leftbro->sub[j] = ptr->sub[i];
		if (leftbro->sub[j] != NULL)
		{
			leftbro->sub[j]->parent = leftbro;
		}
	}
	leftbro->num = leftbro->num + ptr->num + 1;
	free(ptr);
	Del_Leaf_Item(parent, pos);

}
void RightMerge(BNode *ptr, BNode *rightbro, BNode *parent, int pos)
{
	LeftMerge(ptr, rightbro, parent, pos+1);
}
BNode *AdjusLeaf(BNode*ptr)
{
	BNode*parent = ptr->parent;
	int pos = 0;
	while (parent->sub[pos] != ptr) ++pos;

	BNode*leftbro = pos-1<0?NULL:parent->sub[pos-1];
	BNode*rightbro = pos+1>=MAX?NULL:parent->sub[pos+1];

	if (leftbro!=NULL&&leftbro->num>MIN)
	{
		RightRotateLeaf(leftbro,ptr,parent,pos);
	}
	else if (rightbro!=NULL&&rightbro->num>MIN)
	{
		LeftRotateLeaf(rightbro, ptr,parent, pos);
	}

	else if(leftbro!=NULL)
	{
		LeftMerge(leftbro, ptr, parent, pos);
		ptr = leftbro;
	}
	else if (rightbro != NULL)
	{
		RightMerge(ptr, rightbro, parent, pos);
		// ptr = rightbro;
	}
	if (parent->parent != NULL&&parent->num < MIN)
	{
		return AdjusLeaf(parent);
	}
	if (parent->parent == NULL&&parent->num <= 0)
	{
		free(parent);
		ptr->parent = NULL;
		return ptr;
	}
	return NULL;

}
/*
ɾ��:������Ҷ�ӽڵ���߷�֧�ڵ�
��֧�ڵ�:��Ѱ��ǰ�����ߺ�̽ڵ㣬(ǰ�����ȣ�����ƶ�Ԫ�ؽ϶�)�滻���Ϊɾ��Ҷ�ӽڵ��е�Ԫ��
//ɾ��Ҷ��Ԫ�غ����Ҷ��Ԫ�ص���ĿС����СԪ��ֵ��Ҫ���е���
*/
bool ReMove(BNode*&root, KeyType e)
{
	if (root == NULL)
		return false;

	Result res = FindValue(root, e);
	if (res.pnode == NULL || res.tag==false) return false; 
 
	BNode *ptr = res.pnode;
	int pos = res.index;

	BNode*pre = FindPre(ptr->sub[pos-1]);
	BNode*next = FindNext(ptr->sub[pos]);

	if (pre != NULL&&pre->num > MIN)
	{
		ptr->data[pos] = pre->data[pre->num];
		ptr = pre;
		pos = pre->num;
	}
	else if (next != NULL&&next->num > MIN)
	{
		ptr->data[pos] = next->data[1];
		ptr = next;
		pos = 1;
	}
	else if (pre != NULL)
	{
		ptr->data[pos] = pre->data[pre->num];
		ptr = pre;
		pos = pre->num;
	}
	else if (next != NULL)
	{
		ptr->data[pos] = next->data[1];
		ptr = next;
		pos = 1;
	}
	Del_Leaf_Item(ptr, pos);

	if (ptr->parent != NULL&&ptr->num < MIN)
	{
		BNode*newroot = AdjusLeaf(ptr);
		if (newroot != NULL)
		{
			root = newroot;
		}
	}
	else if (ptr->parent == NULL&&ptr->num <= 0)
	{
		free(root);
		root = NULL;
	}
	return true;
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

	KeyType kx;
	while (cin>>kx,kx != '#')
	{
		cout<<ReMove(root,kx)<<":";
		InOrder(root);
		cout<<endl;
	}
	
}





