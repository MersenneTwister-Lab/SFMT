/* 
 * Search Parity Check Vector
 */
#include <stdio.h>
#include <iostream>

#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>
#include <NTL/mat_GF2.h>
#include "dependent_proc.h"

using namespace std;
using namespace NTL;

void convert(mat_GF2& mat, vec_GF2 array[], int size);
void concat(mat_GF2& r, mat_GF2& a, mat_GF2& b);
void separate(mat_GF2& a, mat_GF2& b, mat_GF2& M);
void get_vec(vec_GF2& vec, mat_GF2& M, int pos);
int first_one(vec_GF2& vec);
int get_dependent(vec_GF2& dependent, vec_GF2 array[], int size);

DependentProc::DependentProc(vec_GF2 array[], int size) {
    convert(mat, array, size);
    ident(P, mat.NumRows());
}

void DependentProc::get_dependent(vec_GF2& vec) {
    mat_GF2 M;
    int w;
    int rank;

    w = mat.NumCols();
    concat(M, mat, P); 
    rank = gauss(M, w);
    separate(mat, P, M);
    if (rank >= M.NumRows()) {
	vec.SetLength(0);
	return;
    }
    get_vec(vec, P, rank);
}

void concat(mat_GF2& r, mat_GF2& a, mat_GF2& b) {
    int i, j, k;
    int rows, cols;

    assert(a.NumRows() == b.NumRows());
    rows = a.NumRows();
    cols = a.NumCols() + b.NumCols();
    r.SetDims(rows, cols);
    for (i = 0; i < rows; i++) {
	for (j = 0; j < a.NumCols(); j++) {
	    r.put(i, j, a.get(i, j));
	}
	for (k = 0; k < b.NumCols(); j++, k++) {
	    r.put(i, j, b.get(i, k));
	}
    }
}

/* separate M to a and b, b is square */
void separate(mat_GF2& a, mat_GF2& b, mat_GF2& M) {
    int i, j, k;
    int rows, cols;

    rows = M.NumRows();
    cols = M.NumCols();
    a.SetDims(rows, cols - rows);
    b.SetDims(rows, rows);
    for (i = 0; i < rows; i++) {
	for (j = 0; j < cols - rows; j++) {
	    a.put(i, j, M.get(i, j));
	}
	for (k = 0; k < rows; j++, k++) {
	    b.put(i, k, M.get(i, j));
	}
    }
}

void convert(mat_GF2& mat, vec_GF2 array[], int size) {
    int i, j, len;

    assert(size > 0);

    len = array[0].length();
    mat.SetDims(len, size);
    for (i = 0; i < size; i++) {
	for (j = 0; j < len; j++) {
	    mat.put(i, j, array[i].get(j));
	}
    }
}

void get_vec(vec_GF2& vec, mat_GF2& M, int pos) {
    int i;

    assert(pos < M.NumRows());

    vec.SetLength(M.NumCols());
    for (i = 0; i < M.NumCols(); i++) {
	vec.put(i, M.get(pos, i));
    }
}

int first_one(vec_GF2& vec) {
    int i;

    for (i = 0; i < vec.length(); i++) {
	if (IsOne(vec.get(i))) {
	    return i;
	}
    }
    assert(false);
    return -1;
}

int get_dependent(vec_GF2& dependent, vec_GF2 array[], int size) {
    mat_GF2 A;
    mat_GF2 E;
    mat_GF2 M;
    int rank;

    ident(E, size);
    convert(A, array, size);
    concat(M, A, E);
    rank = gauss(M);
    if (rank == size) {
	return -1;
    }
    separate(A, E, M);
    get_vec(dependent, E, rank);
    return first_one(dependent);
}
