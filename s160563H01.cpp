#include "Graph_adj_list.h"
#include <queue>

extern void errorExit(const char *s);

SLList2 pool;
unsigned long UsedMemoryForArray = 0;

void Read_Graph_adj_list(int Vnum, vertex *V, int Enum, edge *E) {
	// ���Ͽ��� �׷��� ������ �о� V�� E �迭�� �����Ѵ�.
	// V[ ].name�� E[ ].name�� ������ �ʿ䰡 ������ ���ǻ� �� �迭������
	//   index�� �����Ѵ�.
	// V[ ].S�� adjacent list header�� ���� ����ε� ���ȴ�.
	// ���� edge (i,j)�� ������ V[i].S�� edge index k�� push�ϰ�
	//  ���������� V[j].S�� k�� push�Ѵ�.
	// V[].flag�� E[].flag�� �ݵ�� false�� �����ؾ� �Ѵ�.

	// ***�� �Լ��� �ۼ��ߴ�
	SLL *p;

	for (int i = 0; i < Vnum; i++) {
		V[i].name = i;
		V[i].flag = false;
	}

	for (int i = 0; i < Enum; i++) {
		scanf_s("%d %d %d", &E[i].vf, &E[i].vr, &E[i].cost);
		E[i].name = i;
		E[i].flag = false;
		for (int j = 0, k = E[i].vf; j != 2; j++, k = E[i].vr) {
			p = pool.allocSLL();
			p->i = i;
			V[k].S.push(p);
		}
	}
}

void Free_Graph_adj_list(int Vnum, vertex *V) {
	// V �迭�� adjacency list�� ���Ҹ� pop()�� ����Ͽ�
	// ���  pool�� �ǵ��� �ش�
	int k;

	// ***�� �Լ��� �ۼ��ߴ�
	for (int i = 0; i < Vnum; i++) {
		while (!V[i].S.empty()) {
			pool.freeSLL(V[i].S.pop());
		}
	}
}

int BFS_Tree_adj_list(
	int     src,   // source node index
	int     Vnum,  // number of vertices
	vertex *V,     // vertex structure array (starting index 0)
	int     Enum,  // number of edges
	edge   *E     // edge structure array (starting index 0)
) {
	// BFS�� ����Ͽ� BFS tree�� ���ϴ� ������ flag�� true�� �����Ѵ�.
	// BFS ���� vertex�� �Է� ���Ͽ��� �����ȴ�(src).
	// V[]�� adjacent list Ž���� .top() ��� �Լ��� ����Ͽ�
	//  adj list ����� ��� �̸� ���󰡸� ���� vertex�� Ž���Ѵ�.
	// BFS tree�� ���� ������ cost ���� return �Ѵ�
	//   (not unique but ��� �� edge cost�� 1�̸� unique)
	// -- �Լ��� ��� parameter�� ���� �ʿ�� ����.
	// -- BFS�� ���� queue�� �ʿ��ϸ� STL�� queue�� ����ص� ����

	// ***�� �Լ��� �ۼ��ߴ�
	std::queue<int> q;
	int curr, next, TotalCost=0;
	SLL *p;

	V[src].flag = true;
	q.push(src);
	while (!q.empty()) {
		curr = q.front();
		q.pop();
		for (p = V[curr].S.top(); p; p=p->p) {
			next = (E[p->i].vf != curr) ? E[p->i].vf : E[p->i].vr;
			if (V[next].flag) continue;
			V[next].flag = true;
			E[p->i].flag = true;
			TotalCost += E[p->i].cost;
			q.push(next);
		}
	}
	return TotalCost;
}

// SLL management functions
// �Ʒ� �Լ����� 02_04_CSE3081_CPrg&DataStructure_�߰��ڷ�.pdf��
//   �����Ͽ� �� �Լ����� ä�� ����.
// ���ø� �ʿ��ϹǷ� ť�� ��� �Լ��� �Ʒ� �Լ� ����Ʈ�� ����

// stack member fuctions
void sllStack2::push(SLL *p) {
	// insert p to the top of stack ST
	p->p = ST;
	ST = p;
}

SLL *sllStack2::pop(void) {
	// remove and return p at the top of ST

	// ***�� �Լ��� �ۼ��ߴ�
	SLL *p = ST;
	ST = ST->p;
	return p;
}

SLL *sllStack2::top(void) {
	// return p at the top of ST
	return ST;
}

bool sllStack2::empty(void) {
	// return true if empty
	if (ST == NULL)
		return true;
	else
		return false;
}

// SLList2 member functions
SLL *SLList2::allocSLL(void) {		// allocate an SLL element
	SLL *p;

	// ***�� �Լ��� �ۼ��ߴ�
	if (!SLL_pool) {
		p = (SLL *)malloc(sizeof(SLL));
		if (!p) errorExit("Error in Alloc_SLL");
		UsedMemoryForSLLs += sizeof(SLL);
		p->i = NONE;
	}
	else {
		p = SLL_pool;
		SLL_pool = p->p;
	}
	p->p = NULL;
	SLL_cnt++;

	return(p);
}

void SLList2::freeSLL(SLL *p) {		// free *p (move it to SLL_pool
	if (p->i == NONE)
		errorExit("This is already freed(freeSLL).");

	// ***�� �Լ��� �ۼ��ߴ�
	p->i = NONE;
	p->p = SLL_pool;
	SLL_pool = p;

	SLL_cnt--;
}

void SLList2::freeSLL_pool(void) {	// clear SLL_pool
	SLL *p = SLL_pool;

	// ***�� �Լ��� �ۼ��ߴ�
	while (p) {
		SLL_pool = p->p;
		free(p);
		UsedMemoryForSLLs -= sizeof(SLL);
		p = SLL_pool;
	}

	if (SLL_cnt)
		errorExit("Non-zero SLL_cnt after cleanup.");
}