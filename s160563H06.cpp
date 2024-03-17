#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <queue>
#include <math.h>

#define NONE -1

typedef struct elm_edge {
	int  vf, vr;  // e = (vf, vr) where vf, vr are vertex array indices
	int  cost;    // edge cost
	bool flag;    // true if the edge is in the SP spanning tree, false otherwise 
	int  fp, rp;  // adj list ptr of forward and reverse (-1 if none)
} edge;

typedef struct elm_vertex {
	int  f_hd, r_hd;	// adj list header(-1 if none)
	int  distance;		// distance from src to this vertex
	int  heap_idx;		// minHeap index
	bool inS;			// true if SP length found(true when move from V-S to S) 
} vertex;

void Error_Exit (const char *s);

void swap(int* a, int* b) { // swap function
	int tmp;
	tmp = *a;
	*a = *b;
	*b = tmp;
}

void bubble_up(int *minHeap, vertex* V, int idx) { // heap node swap
	if (idx > 1 && V[minHeap[idx / 2]].distance > V[minHeap[idx]].distance) { // swap
		swap(&(V[minHeap[idx / 2]].heap_idx), &(V[minHeap[idx]].heap_idx));
		swap(&minHeap[idx / 2], &minHeap[idx]);
		bubble_up(minHeap, V, idx / 2);
	}
}

void bubble_down(int *minHeap, int size, vertex* V, int idx) { // heap node swap
	int child = idx * 2;
	if (child < size && V[minHeap[child]].distance > V[minHeap[child+1]].distance) // find child to swap
		child++;
	if (child <= size && V[minHeap[idx]].distance > V[minHeap[child]].distance) { // swap
		swap(&V[minHeap[child]].heap_idx, &V[minHeap[idx]].heap_idx);
		swap(&minHeap[child], &minHeap[idx]);
		bubble_down(minHeap, size, V, child);
	}
}

void insert_heap(int *minHeap, int *size, vertex *V, int idx) {
	// insert to the end of the heap
	V[idx].heap_idx = ++(*size); 
	minHeap[*size] = idx;
	bubble_up(minHeap, V, V[idx].heap_idx);
}

int delete_heap(int *minHeap, int *size, vertex* V) {
	// delete from the top of the heap
	int ret = -1;
	if (!(*size)) return ret;
	else {
		ret = minHeap[1];
		V[minHeap[1]].heap_idx = -1;
		V[minHeap[*size]].heap_idx = 1;
		minHeap[1] = minHeap[*size];
		(*size)--;
		bubble_down(minHeap, *size, V, 1);
	}

	return ret;
}

int SPT_Dijkstra(
	int src,	// source vertex index
	// graph structure array
	// 1. the adjacency list structure is the same as PHW02
	// 2. additional fields are added for Dijkstra's algorithm(see .h file)
	int Vnum, vertex *V,	// Vertex array size and the array
	int Enum, edge *E,		// Edge array size and the array

	int *minHeap	// array for min heap (array size = Vnum+1)
		// heap index range is 1 ~ (Vnum - 1) note: src must not in the initial heap
		// just arry is passed. must construct min-heap in this function

	// OUTPUT
	// 1. V[].distance : shortest path length from src to this vertex
	// 2. shortest path spanning tree : set E[].flag = true if the edge is in the tree
	// 3. return the sum of edge costs in the shortest path spanning tree.
	//    should be small as possible (think a way to make it small)
) {
	int treeCost = 0;
	// *** �� �Լ��� �ۼ��ߴ� ***
	// �ݵ�� min-heap�� ����Ͽ� O((n+m)logn) �˰����� �����ؾ� �Ѵ�(�ƴϸ� trivial�� ���α׷���)
	// heap ���� �� �ʿ��� �Լ��� �����Ӱ� �ۼ��Ͽ� �߰��Ѵ�.
	// �׷��� global ����, dynamic array ���� �߰��� ������� �ʴ´�(������ �ʿ� ����)

	// init src vertex
	V[src].inS = true;
	V[src].distance = 0;

	// init vertices which is adjacent to src vertex
	for (int i = 0; i < Enum; i++) {
		if (E[i].vf == src || E[i].vr == src) {
			if (E[i].vf == src) V[E[i].vr].distance = E[i].cost;
			else V[E[i].vf].distance = E[i].cost;
		}
	}

	// insert heap
	int size = 0;
	for (int i = 0; i < Vnum; i++) {
		if (src != i) insert_heap(minHeap, &size, V, i);
	}

	// delete from the heap
	int e, u, w;
	while ((u = delete_heap(minHeap, &size, V))  != -1) {
		V[u].inS = true;

		// every vertex w adjacent to u : front
		e = V[u].f_hd;
		while (e != NONE) {
			w = E[e].vr;
			if (!V[w].inS && V[u].distance + E[e].cost < V[w].distance) {
				V[w].distance = V[u].distance + E[e].cost;
				// adjust heap
				bubble_up(minHeap, V, V[w].heap_idx);
			}
			e = E[e].fp;
		}

		// every vertex w adjacent to u : rear
		e = V[u].r_hd;
		while (e != NONE) {
			w = E[e].vf;
			if (!V[w].inS && V[u].distance + E[e].cost < V[w].distance) {
				V[w].distance = V[u].distance + E[e].cost;
				// adjust heap
				bubble_up(minHeap, V, V[w].heap_idx);
			}
			e = E[e].rp;
		}
	}
	
	// backtracing
	for (u = 0; u < Vnum; u++) {
		if (src == u) continue;
		bool flag = false;

		// every vertex w adjacent to u : front
		e = V[u].f_hd;
		while (e != NONE) {
			w = E[e].vr;
			if (V[u].distance == E[e].cost + V[w].distance) {
				E[e].flag = true;
				treeCost += E[e].cost;
				flag = true;
				break;
			}
			e = E[e].fp;
		}
		if (flag) continue;

		// every vertex w adjacent to u : rear
		e = V[u].r_hd;
		while (e != NONE) {
			w = E[e].vf;
			if (V[u].distance == E[e].cost + V[w].distance) {
				E[e].flag = true;
				treeCost += E[e].cost;
				flag = true;
				break;
			}
			e = E[e].rp;
		}
	}

	return treeCost;
}

void Read_Graph(int Vnum, vertex *V, int Enum, edge *E) {
	// Graph �ڷᱸ���� ����� �Լ�
	// *** �� �Լ��� �߰��ߴ� ***
	// PHW02�� Read_Graph_adj_array()�� �� ������ �ڷᱸ���� ���� ��¦ �����Ͽ� ����Ѵ�
	// ��, Read_Graph_adj_array()���� ���ʿ��� ������ ������ �� ���.
	
	for (int i = 0; i < Vnum; i++) { // initialize V
		V[i].f_hd = V[i].r_hd = NONE;
		V[i].distance = INT_MAX;
		V[i].inS = false;
	}

	for (int i = 0; i < Enum; i++) { // initialize E
		E[i].flag = false;
		E[i].fp = E[i].rp = NONE;
	}	
	
	for (int i = 0; i < Enum; i++) { // initialize E, V
		scanf_s("%d %d %d", &E[i].vf, &E[i].vr, &E[i].cost);
		if (V[E[i].vf].f_hd != NONE) E[i].fp = V[E[i].vf].f_hd;
		V[E[i].vf].f_hd = i;
		if (V[E[i].vr].r_hd != NONE) E[i].rp = V[E[i].vr].r_hd;
		V[E[i].vr].r_hd = i;
	}
}

// the following functions are for testing if the submitted program is correct.
int  Tree_Check(int Vnum, vertex *V, int Enum, edge *E, int *visited);
bool SPT_test(int src, int Vnum, vertex *V, int Enum, edge *E, int *minHeap);
void Error_Exit(const char *s);

int main ( void ) {
	int		src;
	vertex *V;		int Vnum;
	edge   *E;		int Enum;
	int    *minHeap;
	int    Tree_cost;
	int    Tnum; 		// # of test cases
	clock_t start, finish;
	double cmpt;

	scanf_s("%d", &Tnum);		// read # of tests

	for (int t = 0; t < Tnum; t++ ) {
		scanf_s("%d %d %d", &Vnum, &Enum, &src);
		V = new vertex [Vnum];
		E = new edge [Enum];
		minHeap = new int[Vnum + 1];	// heap array allocation
		if ( V == NULL || E == NULL || minHeap == NULL ) {
			Error_Exit("Memory Allocation Error");
		}
		Read_Graph(Vnum, V, Enum, E);

		/**/start = clock();	// start timer

		Tree_cost = SPT_Dijkstra(src, Vnum, V, Enum, E, minHeap);	// code by students

		/**/finish = clock();	// stop timer
		cmpt = ((double)(finish - start)) / (double)CLK_TCK;

		// �Ʒ� Tree_Check�� SPT_test �Լ��� ÷���� SPT_test.obj ���Ͽ� �ִ�.
		// �� �׽�Ʈ���� ������ ���Ͽ� ���α׷��� �����ϸ� ���� �߸��� ���̴�(�����ص� 0��)
        if (Tree_Check(Vnum, V, Enum, E, minHeap) == 0) {
			Error_Exit("   ERROR The result is not a spanning tree");
		}
		if (SPT_test(src, Vnum, V, Enum, E, minHeap) == false) {
			Error_Exit("** Something wrong in applying Dijkstra's");
		}
		if ( t != 0 ) 
			printf("\n");

		printf("**T%d (Dijkstra) (V = %d, E = %d, time = %.3f sec) Tree Cost = %d\n", 
			t+1, Vnum, Enum, cmpt, Tree_cost);

		delete [] minHeap; delete [] V; delete [] E;
	}
	return 0;
}

void Error_Exit (const char *s ) {
  printf("%s\n", s);
  exit(-1);
}
