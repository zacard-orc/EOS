//双向链表

#include <iostream>
#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>

int x=0;
#define Malloc(type,n) (type *)malloc((n)*sizeof(type))

typedef struct slist
{
	int data;
	int flag;
	struct slist *before;
	struct slist *next;
} *prlist;


//创建空双向链表
void initx(prlist &L)
{
	L=Malloc(slist,1);
	if(L)
	{  
	   L->data=999;
	   L->flag=10;	
	   L->before=L->next=L;
	}
}

//增加空链
void addlist(slist *L,int val)
{
	int a=0;
	slist *N;
	N=Malloc(slist,1);
	N->data=val;
	if(L->next==L)  //0->1
	{
		L->before=L->next=N;		
		N->before=N->next=L;
	}
	else		   //N->N+1
	{
		N->data=val;
		//建立N与前后的关系
		N->next=L->next;
		N->before=L;
		//拆开L与L->next的关系
		L->next->before=N;
		L->next=N;
	
	}
}

//拆除内存块
void dellist(slist *L)
{
	if(L)
	{
		L->before->next=L->next;
		L->next->before=L->before;
		free(L);
	}
}


//顺向遍历
void showlist(slist *L)
{
	printf("----\n");
	int a=0;
	while(a<10)
	{
		printf("%d:%d\n",a,L->data);
		L=L->next;
		a++;
	}
}

//逆向遍历
void showlist2(slist *L)
{
	printf("----\n");
	int a=0;
	while(a<10)
	{
		printf("%d:%d\n",a,L->data);
		L=L->before;
		a++;
	}
}


int main()
{
	slist *T;
	printf("ADDRESS:0x%x\n",&T);
	initx(T);
	int i=0;
	while(i<10)
	{
		addlist(T,999-++i);
		T=T->next;
	}
	showlist(T->next);
	showlist2(T->next);
	
	dellist(T);

	showlist(T);
	showlist2(T);

	return 0;
}
