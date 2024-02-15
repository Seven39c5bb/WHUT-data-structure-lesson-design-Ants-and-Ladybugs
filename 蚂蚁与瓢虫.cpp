#include<stdio.h>
#include <malloc.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include <cassert>
#include "蚂蚁与瓢虫.h"
const int N = 110,INF = 0x7FFFFFFF;
typedef struct QueueNode
{
	int data;
	struct QueueNode* next;
}QNode;

//记录头指针和尾指针
typedef struct Queue
{
	QNode* head;
	QNode* tail;
}Queue;
//创建一个邻接表对有向图进行存储
struct treeEdge {
	int data;
	struct treeEdge* next;
	int ant = 0;
};
struct tree {
	int numNodes;
	struct treeEdge** adjList;//邻接表数组
};
struct Ant {
	int dist;//离瓢虫的距离
	int id;
	int ve;//当前所在结点
	int hitCnt=0;
};
struct treeNode
{
	int id, ant;
};
struct path {
	struct path* from;
	int p;
};
//设置全局变量初始时间点为0
int timePoint = 0;
//设置全局变量结点数
int nodes;
//蚂蚁的数量
int antCnt;
Ant ants[N];
//随机生成瓢虫的位置
int randLadybugs() {
	int a;
	printf("请输入瓢虫的位置：");
	scanf("%d", &a);
	return a;// rand() % (nodes + 1);
}
//记录蚂蚁到瓢虫的路线
int verCnt[N], pathal[N][N];
//树上的结点
treeNode tn[N];
//记录蚂蚁的顺序
int order[N];
#pragma region 队列ADT
//队列的初始化
void QueueInit(Queue* pq)
{
	assert(pq);
	pq->head = pq->tail = NULL;
}

//队列的销毁
void QueueDestroy(Queue* pq)
{
	QNode* cur = pq->head;
	while (cur)
	{
		QNode* next = cur->next;
		free(cur);
		cur = next;
	}
	pq->head = pq->tail = NULL;
}

//入队
void QueuePush(Queue* pq, int x)
{
	assert(pq);
	QNode* newnode = (QNode*)malloc(sizeof(QNode));
	if (newnode == NULL)
	{
		printf("%s\n", strerror(errno));
		exit(-1);
	}
	newnode->data = x;
	newnode->next = NULL;

	//尾插 - 用了尾指针就不用找尾了
	if (pq->tail == NULL)
	{
		assert(pq->head == NULL);
		pq->head = pq->tail = newnode;
	}
	else
	{
		pq->tail->next = newnode;
		pq->tail = newnode;
	}
}

//出队
void QueuePop(Queue* pq)
{
	assert(pq);
	assert(pq->head && pq->tail);

	//头删 - 只有一种个结点的情况时tail会成野指针
	if (pq->head->next == NULL)
	{
		free(pq->head);
		pq->head = pq->tail = NULL;
	}
	else
	{
		QNode* next = pq->head->next;
		free(pq->head);
		pq->head = next;
	}

}

//判断队列是否为空
bool QueueEmpty(Queue* pq)
{
	assert(pq);

	return pq->head == NULL && pq->tail == NULL;
}

//队列中数据的个数
size_t QueueSize(Queue* pq)
{
	assert(pq);
	QNode* cur = pq->head;
	size_t size = 0;
	while (cur)
	{
		size++;
		cur = cur->next;
	}

	return size;
}

//队头的数据
int QueueFront(Queue* pq)
{
	assert(pq);
	assert(pq->head);

	return pq->head->data;
}

//队尾的数据
int QueueBack(Queue* pq)
{
	assert(pq);
	assert(pq->tail);

	return pq->tail->data;
}

#pragma endregion
//添加边到树
void addEdge(struct tree* stree,int a,int b) {
		treeEdge* newNode = (struct treeEdge*)malloc(sizeof(treeEdge));
		treeEdge* transNode = (struct treeEdge*)malloc(sizeof(treeEdge));
		newNode->data = b;
		newNode->next = NULL;

		if (stree->adjList[a] == NULL) {
			stree->adjList[a] = transNode;
			stree->adjList[a]->data = a;
			stree->adjList[a]->next = newNode;
		}
		else {
			treeEdge* curNode = stree->adjList[a]->next;
			stree->adjList[a]->next = newNode;
			newNode->next = curNode;
		}
}
//初始化树
struct tree* initTree() {
	struct tree* sTree = (struct tree*)malloc(sizeof(struct tree));
	sTree->numNodes = nodes;
	sTree->adjList = (struct treeEdge**)malloc(sTree->numNodes * sizeof(struct treeEdge*));
	for (int i = 1; i <= sTree->numNodes; i++)sTree->adjList[i] = NULL;
	return sTree;
}
//输入蚂蚁初始的位置
void antsOriPos(tree* stree) {
	for (int i = 1; i <= antCnt; i++) {
		printf("请输入第%d只蚂蚁所在结点：", i);
		int pos=0;
		scanf("%d", &pos);
		tn[pos].ant = i;
		ants[i].id = i;
		ants[i].ve = pos;
		order[i] = i;
	}
}
//初始化蚂蚁
void clear() {
	for (int i = 1; i <= antCnt; i++) {
		ants[i].dist = INF;
	}
}
//bfs
void bfs(treeEdge** adjLis,int lbPos) {
	path q[N]; int head = 0, tail = 0;
	int* p;
	q[0].from = NULL;
	q[0].p = lbPos;
	int visited[N] = { 0 };
	visited[lbPos] = 1;
	path u,v;
	while (head<=tail) {
		v.from = &q[head];
		printf("v.from:%d\n", v.from->p);
		u = q[head++];
		for (treeEdge* cur = adjLis[u.p]; cur; cur = cur->next) {
			v.p = cur->data;
			if (!visited[v.p]) {
				printf("cur->data:%d\n", cur->data);
				visited[v.p] = 1;
				if (tn[v.p].ant != 0) {
					int a = tn[v.p].ant;//获得当前位置上蚂蚁的编号
					path* b = &v;
					verCnt[a] = 0;
					p = pathal[a];
					printf("在%d点上的蚂蚁%d到瓢虫的路线为：\n", ants[v.p].ve,a);
					while (b->from)
					{
						printf("%d -> ", b->p);
						p[++verCnt[a]] = b->p;
						b = b->from;
					}
					printf("%d\n", lbPos);
					p[++verCnt[a]] = lbPos;
					ants[a].dist = verCnt[a] - 1;
					printf("蚂蚁%d的距离瓢虫的距离是：%d\n", a, ants[a].dist);
				}
				else {
					q[++tail] = v;
				}
			}
		}
	}
}
//cmp
int cmp(const void* a, const void* b) {
	int A = *(int*)a, B = *(int*)b;
	if (ants[A].dist < ants[B].dist)return -1;
	if (ants[A].dist > ants[B].dist)return 1;
	if (ants[A].id < ants[B].id)return -1;
	return 1;
}
void move(int lbPos) {
	int i = order[1];
	ants[i].hitCnt++;
	//将该点蚂蚁移动到瓢虫的点上
	ants[i].ve = lbPos;
	tn[lbPos].ant = i;
}
//开始的初始化
void InitAll()
{
	printf("请输入蚂蚁的数量：");
	scanf("%d", &antCnt);
	printf("请输入树中结点的数量：");
	scanf("%d", &nodes);
	while (nodes < 2 * antCnt) {
		printf("结点数量必须大于等于两倍的蚂蚁数量，请重新输入结点数量：");
		scanf("%d", &nodes);
	}
	//生成树
	tree* stree = initTree();
	//向树中添加边
	for (int i = 1; i <= stree->numNodes - 1; i++) {
		int a, b;
		printf("请输入两个值，表示从a到b有路可走:\n");
		scanf("%d%d", &a, &b);
		addEdge(stree, a, b);
		addEdge(stree, b, a);
	}
	//
	for (int i = 1; i <= nodes; i++) {
		tn[i].id = i;
	}

	//时间点为0时，展示树的结构,遍历邻接表
	printf("时间点为%d时,展示该树的结构：", timePoint++);
	for (int i = 1; i <= nodes; i++) {
		treeEdge* cur = stree->adjList[i];
		if (cur == NULL)continue;
		while (cur->next)
		{
			printf("从%d到%d有一条路\n", i, cur->next->data);
			cur = cur->next;
		}
	}
	//对蚂蚁的初始位置进行赋值
	antsOriPos(stree);
	//对瓢虫降落在树上的时间进行模拟
	printf("请输入要生成瓢虫的次数：");
	int lbNum;
	scanf("%d", &lbNum);
	for (int i = 1; i <= lbNum; i++) {
		int lbPos = randLadybugs();
		printf("第%d次生成瓢虫的位置在：%d\n", i, lbPos);
		//如果当前结点位置有蚂蚁，直接赶走并记功一次
		if (tn[lbPos].ant != 0) {
			ants[tn[lbPos].ant].hitCnt++;
		}
		else {
		//先进行初始化,将所有蚂蚁初始离瓢虫的位置设置为无限大
		clear();
		//从lbPos开始进行bfs，找到每个蚂蚁距离该瓢虫的位置,并对蚂蚁进行排序
		bfs(stree->adjList, lbPos);
		//对每个蚂蚁相对瓢虫的位置顺序进行排序
		qsort(order + 1, antCnt, sizeof(order[0]), cmp);
		move(lbPos);
		}
		
	}
}
int main() {
	InitAll();
	for (int i = 1; i <= antCnt; i++) {
		printf("第%d只蚂蚁赶跑瓢虫的数量为：%d\n", i,ants[i].hitCnt);
	}
}


