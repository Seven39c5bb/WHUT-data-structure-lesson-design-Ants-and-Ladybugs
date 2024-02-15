#include<stdio.h>
#include <malloc.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include <cassert>
#include "������ư��.h"
const int N = 110,INF = 0x7FFFFFFF;
typedef struct QueueNode
{
	int data;
	struct QueueNode* next;
}QNode;

//��¼ͷָ���βָ��
typedef struct Queue
{
	QNode* head;
	QNode* tail;
}Queue;
//����һ���ڽӱ������ͼ���д洢
struct treeEdge {
	int data;
	struct treeEdge* next;
	int ant = 0;
};
struct tree {
	int numNodes;
	struct treeEdge** adjList;//�ڽӱ�����
};
struct Ant {
	int dist;//��ư��ľ���
	int id;
	int ve;//��ǰ���ڽ��
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
//����ȫ�ֱ�����ʼʱ���Ϊ0
int timePoint = 0;
//����ȫ�ֱ��������
int nodes;
//���ϵ�����
int antCnt;
Ant ants[N];
//�������ư���λ��
int randLadybugs() {
	int a;
	printf("������ư���λ�ã�");
	scanf("%d", &a);
	return a;// rand() % (nodes + 1);
}
//��¼���ϵ�ư���·��
int verCnt[N], pathal[N][N];
//���ϵĽ��
treeNode tn[N];
//��¼���ϵ�˳��
int order[N];
#pragma region ����ADT
//���еĳ�ʼ��
void QueueInit(Queue* pq)
{
	assert(pq);
	pq->head = pq->tail = NULL;
}

//���е�����
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

//���
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

	//β�� - ����βָ��Ͳ�����β��
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

//����
void QueuePop(Queue* pq)
{
	assert(pq);
	assert(pq->head && pq->tail);

	//ͷɾ - ֻ��һ�ָ��������ʱtail���Ұָ��
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

//�ж϶����Ƿ�Ϊ��
bool QueueEmpty(Queue* pq)
{
	assert(pq);

	return pq->head == NULL && pq->tail == NULL;
}

//���������ݵĸ���
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

//��ͷ������
int QueueFront(Queue* pq)
{
	assert(pq);
	assert(pq->head);

	return pq->head->data;
}

//��β������
int QueueBack(Queue* pq)
{
	assert(pq);
	assert(pq->tail);

	return pq->tail->data;
}

#pragma endregion
//��ӱߵ���
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
//��ʼ����
struct tree* initTree() {
	struct tree* sTree = (struct tree*)malloc(sizeof(struct tree));
	sTree->numNodes = nodes;
	sTree->adjList = (struct treeEdge**)malloc(sTree->numNodes * sizeof(struct treeEdge*));
	for (int i = 1; i <= sTree->numNodes; i++)sTree->adjList[i] = NULL;
	return sTree;
}
//�������ϳ�ʼ��λ��
void antsOriPos(tree* stree) {
	for (int i = 1; i <= antCnt; i++) {
		printf("�������%dֻ�������ڽ�㣺", i);
		int pos=0;
		scanf("%d", &pos);
		tn[pos].ant = i;
		ants[i].id = i;
		ants[i].ve = pos;
		order[i] = i;
	}
}
//��ʼ������
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
					int a = tn[v.p].ant;//��õ�ǰλ�������ϵı��
					path* b = &v;
					verCnt[a] = 0;
					p = pathal[a];
					printf("��%d���ϵ�����%d��ư���·��Ϊ��\n", ants[v.p].ve,a);
					while (b->from)
					{
						printf("%d -> ", b->p);
						p[++verCnt[a]] = b->p;
						b = b->from;
					}
					printf("%d\n", lbPos);
					p[++verCnt[a]] = lbPos;
					ants[a].dist = verCnt[a] - 1;
					printf("����%d�ľ���ư��ľ����ǣ�%d\n", a, ants[a].dist);
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
	//���õ������ƶ���ư��ĵ���
	ants[i].ve = lbPos;
	tn[lbPos].ant = i;
}
//��ʼ�ĳ�ʼ��
void InitAll()
{
	printf("���������ϵ�������");
	scanf("%d", &antCnt);
	printf("���������н���������");
	scanf("%d", &nodes);
	while (nodes < 2 * antCnt) {
		printf("�������������ڵ�������������������������������������");
		scanf("%d", &nodes);
	}
	//������
	tree* stree = initTree();
	//��������ӱ�
	for (int i = 1; i <= stree->numNodes - 1; i++) {
		int a, b;
		printf("����������ֵ����ʾ��a��b��·����:\n");
		scanf("%d%d", &a, &b);
		addEdge(stree, a, b);
		addEdge(stree, b, a);
	}
	//
	for (int i = 1; i <= nodes; i++) {
		tn[i].id = i;
	}

	//ʱ���Ϊ0ʱ��չʾ���Ľṹ,�����ڽӱ�
	printf("ʱ���Ϊ%dʱ,չʾ�����Ľṹ��", timePoint++);
	for (int i = 1; i <= nodes; i++) {
		treeEdge* cur = stree->adjList[i];
		if (cur == NULL)continue;
		while (cur->next)
		{
			printf("��%d��%d��һ��·\n", i, cur->next->data);
			cur = cur->next;
		}
	}
	//�����ϵĳ�ʼλ�ý��и�ֵ
	antsOriPos(stree);
	//��ư�潵�������ϵ�ʱ�����ģ��
	printf("������Ҫ����ư��Ĵ�����");
	int lbNum;
	scanf("%d", &lbNum);
	for (int i = 1; i <= lbNum; i++) {
		int lbPos = randLadybugs();
		printf("��%d������ư���λ���ڣ�%d\n", i, lbPos);
		//�����ǰ���λ�������ϣ�ֱ�Ӹ��߲��ǹ�һ��
		if (tn[lbPos].ant != 0) {
			ants[tn[lbPos].ant].hitCnt++;
		}
		else {
		//�Ƚ��г�ʼ��,���������ϳ�ʼ��ư���λ������Ϊ���޴�
		clear();
		//��lbPos��ʼ����bfs���ҵ�ÿ�����Ͼ����ư���λ��,�������Ͻ�������
		bfs(stree->adjList, lbPos);
		//��ÿ���������ư���λ��˳���������
		qsort(order + 1, antCnt, sizeof(order[0]), cmp);
		move(lbPos);
		}
		
	}
}
int main() {
	InitAll();
	for (int i = 1; i <= antCnt; i++) {
		printf("��%dֻ���ϸ���ư�������Ϊ��%d\n", i,ants[i].hitCnt);
	}
}


