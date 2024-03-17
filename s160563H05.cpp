#include <stdio.h>
#include <string.h> 

enum Where{case_ins=0, case_del, case_sub, case_match};

void Back_Track_Str(char **STR, int size, char c) {
	char *TMP = new char[size];
	if (*STR) strcpy(TMP + 1, *STR);
	else (TMP[1] = '\0');
	TMP[0] = c;
	if (*STR) delete[] * STR;
	*STR = TMP;
}

void Edit_Distance (
	// inputs
	char *SS,		// Source string array
	char *TS,		// Edited string array
	int ins_cost, int del_cost, int sub_cost,
	// insertion, deletion, substitution cost
	// outputs
	int **Table,    // Cost Table Calculated (2-d array (|SS|+1) x (|TS|+1) will be set)
	char **SR,		// Source string with insertion to T marks '*'
	char **OP,		// Operation sequence('i', 'd', 's', or '.' (nop))
	char **TR,       // Edited string with deletion form S mark '*'
					 // NOTE: ST, OP, TR must be allocated inside the function as needed(minimum as possible)
	int  *Mem_Allocated
) {
	//initializing
	int lenS = strlen(SS);
	int lenT = strlen(TS);
	int i, j;
	for (i = 0; i <= lenS; i++) // src
		Table[i][0] = del_cost * i;
	for (i = 0; i <= lenT; i++) // edited
		Table[0][i] = ins_cost * i;

	//calculation
	for (i = 0; i < lenS; i++) {
		for (j = 0; j < lenT; j++) {
			if (SS[i] == TS[j]) {
				Table[i+1][j+1] = Table[i][j];
			}
			else {
				int ins = Table[i + 1][j] + ins_cost; // current insertion cost
				int del = Table[i][j + 1] + del_cost; // current deletion cost
				int sub = Table[i][j] + sub_cost; // current substitution cost
				Table[i + 1][j + 1] = (ins < del ? ins : del) < sub ? (ins < del ? ins : del) : sub; // take minimum cost
			}
		}
	}

	//print
	/*for (i = 0; i <= lenS; i++) {
		for (j = 0; j <= lenT; j++) {
			printf("%d ", Table[i][j]);
		}
		printf("\n");
	}*/

	//backtracking
	i = lenS; j = lenT;
	while (1) {
		int ins = (i >= 0 && j - 1 >= 0) ? Table[i][j - 1] + ins_cost : -1;
		int del = (i - 1 >= 0 && j >= 0) ? Table[i - 1][j] + del_cost : -1;
		if (ins == -1 && del == -1) break;
		int sub = (i - 1 >= 0 && j - 1 >= 0) ? (SS[i-1] == TS[j-1] ? 0 : Table[i - 1][j - 1] + sub_cost) : -1;

		// ins vs del (both can't be -1 at the same time)
		Where _min_dir;
		if (ins == -1) _min_dir = case_del;
		else if (del == -1) _min_dir = case_ins;
		else {
			_min_dir = (ins < del || (ins == del && ins_cost <= del_cost)) ? case_ins : case_del;
		}
			
		if (sub != -1 && sub < ((_min_dir == case_ins) ? ins : del)) // min vs sub
			_min_dir = sub ? case_sub : case_match;

		//printf("\n%d\n", _min_dir);

		switch (_min_dir) {
		case case_ins:
			j--;
			Back_Track_Str(SR, *SR ? strlen(*SR) + 2 : 2, '*'); // SR = '*'+SR
			Back_Track_Str(TR, *TR ? strlen(*TR) + 2 : 2, TS[j]); // TR = '*'+TR
			Back_Track_Str(OP, *OP ? strlen(*OP) + 2 : 2, 'i'); // OP = 'i'+OP
			break;
		case case_del:
			i--;
			Back_Track_Str(SR, *SR ? strlen(*SR) + 2 : 2, SS[i]); // SR = '*'+SR
			Back_Track_Str(TR, *TR ? strlen(*TR) + 2 : 2, '*'); // TR = '*'+TR
			Back_Track_Str(OP, *OP ? strlen(*OP) + 2 : 2, 'd'); // OP = 'd'+OP
			break;
		case case_sub:
			i--; j--;
			Back_Track_Str(SR, *SR ? strlen(*SR) + 2 : 2, SS[i]); // SR = '*'+SR
			Back_Track_Str(TR, *TR ? strlen(*TR) + 2 : 2, TS[j]); // TR = '*'+TR
			Back_Track_Str(OP, *OP ? strlen(*OP) + 2 : 2, 's'); // OP = 's'+OP
			break;
		case case_match:
			i--; j--;
			Back_Track_Str(SR, *SR ? strlen(*SR) + 2 : 2, SS[i]); // SR = '*'+SR
			Back_Track_Str(TR, *TR ? strlen(*TR) + 2 : 2, TS[j]); // TR = '*'+TR
			Back_Track_Str(OP, *OP ? strlen(*OP) + 2 : 2, '.'); // OP = '.'+OP
			break;
		default:
			break;
		}
	}
	*Mem_Allocated = strlen(*SR) + strlen(*TR) + strlen(*OP) + 3;
}