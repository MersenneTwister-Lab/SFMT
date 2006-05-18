#include <iostream>
#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>
#include <NTL/mat_GF2.h>

NTL_CLIENT

int32_t gauss_plus(mat_GF2& mat);
bool dependent_rows(bool result[], mat_GF2& mat);

int main(void) {
    mat_GF2 mat;
    bool result;

    cout << "Input Matrix" << endl;
    cin >> mat;
    cout << mat << endl;
    bool dependent[mat.NumCols()];
    cout << "-----------------" << endl;
    gauss_plus(mat);
    cout << mat << endl;
    cout << "-----------------" << endl;
    result = dependent_rows(dependent, mat);
    cout << "result = " << result << endl;
    cout << "dependent = [";
    for (int i = 0; i < mat.NumCols(); i++) {
	cout << dependent[i] << " ";
    }
    cout << "]" << endl;
    return 0;
}

int32_t gauss_plus(mat_GF2& mat) {
    int32_t rank;
    int32_t rows;
    int32_t cols;
    int32_t i, j, pos;
    
    rank = gauss(mat);
    rows = mat.NumRows();
    cols = mat.NumCols();
    pos = 0;
    for (i = 0; i < rows; i++) {
	while ((i + pos < cols) && IsZero(mat.get(i, i + pos))) {
	    pos++;
	}
	if (i + pos >= cols) {
	    break;
	}
	cout << "mat[i] = " << mat[i] << endl;
	for (j = 0; j < i; j++) {
	    cout << "mat[j] = " << mat[j] << endl;
	    if (IsOne(mat.get(j, i + pos))) {
		mat[j] += mat[i];
		cout << "new mat[j] = " << mat[j] << endl;
	    }
	}
    }
    return rank;
}

bool dependent_rows(bool result[], mat_GF2& mat) {
    int32_t rows;
    int32_t cols;
    int i, j, k, pos;
    bool found = false;

    rows = mat.NumRows();
    cols = mat.NumCols();
    for (i = 0; i < cols; i++) {
	result[i] = false;
    }
    pos = 0;
    for (i = 0; (i < cols - pos) && (i < rows); i++) {
	if (IsOne(mat.get(i, i + pos))) {
	    continue;
	}
	for (j = 0; j < i; j++) {
	    if (IsOne(mat.get(j, i + pos))) {
		for (k = 0; k < i + pos; k++) {
		    if (IsOne(mat.get(j, k))) {
			found = true;
			result[k] = true;
			break;
		    }
		}
	    }
	}
	if (found) {
	    result[i + pos] = true;
	    break;
	} else {
	    pos++;
	    i--;
	}
    }
    if (!found) {
#if 1
	cout << "not found" << endl;
	cout << mat << endl;
	//exit(1);
#endif
    }
    return found;
}
