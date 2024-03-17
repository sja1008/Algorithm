#include <math.h>

void sortXid (double *X, unsigned *Xid, unsigned *TMP, unsigned N) {
	//merge sort
	if (N <= 1) return;
	unsigned h = N / 2;
	unsigned m = N - h;
	sortXid(X, Xid, TMP, h);
	sortXid(X, Xid + h, TMP, m);
	
	// merge
	unsigned i = 0, j = h, k = 0;
	while (i < h && j < N) {
		if (X[Xid[i]] < X[Xid[j]]) {
			TMP[k] = Xid[i];
			i++;
		}
		else {
			TMP[k] = Xid[j];
			j++;
		}
		k++;
	}
	//if some elements remained
	if (i >= h) { // right sides
		while (j < N) {
			TMP[k] = Xid[j];
			k++; j++;
		}
	}
	else { // left sides
		while (i < h) {
			TMP[k] = Xid[i];
			k++; i++;
		}
	}

	//merge end; copy TMP to Xid
	for (i = 0; i < N; i++) {
		Xid[i] = TMP[i];
	}
}


double closestPairDC(
	unsigned L, unsigned R,	  // current leftmost and rightmost indices
	unsigned *pt1, unsigned *pt2, // closest pair points indices
	double *X, double *Y,         // (x,y) positions array(input)
	unsigned *Xid,  // point index array(sorted by x coordinates)
	unsigned *Yid,  // point index array(gradually sorted by y coordinates)
	unsigned *TMP,  // temporal index array
	unsigned THR	// threshold value
) {
	unsigned i, j, k;
	double curr;
	double dist1, dist2, dist3;

	//less than equal to threashold
	if (R - L + 1 <= THR) {
		// copy Xid to Yid
		//if (L == R) return -1;
		for (i = L; i <= R; i++) {
			Yid[i] = Xid[i];
		}
		// Yid sort (nondecreasing order)
		for (i = L; i <= R; i++) {
			unsigned _min = i; // argmin
			for (j = i + 1; j <= R; j++) {
				if (Y[Yid[j]] < Y[Yid[_min]]) _min = j;
			}
			//swap
			unsigned tmp = Yid[i];
			Yid[i] = Yid[_min];
			Yid[_min] = tmp;
		}

		//find return value (minimum dist)
		//dist = pow(Y[L] - Y[R], 2) + pow(X[L] - X[R], 2);
		dist3 = -1;
		for (i = L; i <= R; i++) {
			for (j = i + 1; j <= R; j++) {
				curr = (Y[i] - Y[j])*(Y[i] - Y[j]) + (X[i] - X[j])*(X[i] - X[j]);
				if (dist3 == -1 || curr < dist3) {
					dist3 = curr;
				}
			}
		}
		return sqrt(dist3);
	}

	else {
		//bigger than
		unsigned M = (L + R) / 2; // middle
		dist1 = closestPairDC(L, M, pt1, pt2, X, Y, Xid, Yid, TMP, THR);
		dist2 = closestPairDC(M + 1, R, pt1, pt2, X, Y, Xid, Yid, TMP, THR);
		dist3 = dist1 < dist2 ? dist1 : dist2;

		//merge
		i = L; j = M + 1; k = L;
		while (i <= M && j <= R) {
			if (Y[Yid[i]] < Y[Yid[j]]) {
				TMP[k] = Yid[i];
				i++;
			}
			else {
				TMP[k] = Yid[j];
				j++;
			}
			k++;
		}
		if (i > M) { // right sides remained
			while (j <= R) {
				TMP[k] = Yid[j];
				k++; j++;
			}
		}
		else { // left sides remained
			while (i <= M) {
				TMP[k] = Yid[i];
				k++; i++;
			}
		}
		//merge end; copy TMP to Yid
		for (i = L; i <= R; i++) {
			Yid[i] = TMP[i];
		}

		//combine
		double Xmid = (X[Xid[M]] + X[Xid[M + 1]]) / 2;
		unsigned cnt = 0;

		for (i = L; i <= R; i++) {
			if (X[Yid[i]] <= Xmid + dist3 && X[Yid[i]] >= Xmid - dist3) {
				TMP[i-L] = Yid[i];
				//cnt++;
			}
		}

		//find closest pair
		double dist_min = -1;
		for (i = 0; i <= R-L-1; i++) {
			for (j = i + 1; j < R-L-1; j++) {
				if (Y[TMP[j]] - Y[TMP[i]] >= dist3) break;
				curr = (Y[TMP[i]] - Y[TMP[j]])*(Y[TMP[i]] - Y[TMP[j]]) + (X[TMP[i]] - X[TMP[j]])*(X[TMP[i]] - X[TMP[j]]);
				if (dist_min == -1 || curr < dist_min) {
					*pt1 = TMP[i]; *pt2 = TMP[j];
					dist_min = curr;
				}
			}
		}

		return sqrt(dist_min);
	}
}