#ifndef MATRIX
#define MATRIX 1

#ifdef __cplusplus
extern "C" {
#endif 

#include "errno.h"

//typedef void * matrix_data_t;
//typedef int matrix_size_t;

typedef struct Matrix_t {
  //matrix_data_t data;
  //matrix_size_t data_size;
  float *data;
  int rows;
  int cols;
  //int r1;
  //int r2;
  //int c1;
  //int c2;
} Matrix;

inline Matrix * fmatrix(int nrows, int ncols);
inline Matrix * fmatrix2(int nrows, int ncols, float value);
inline Matrix * fmatrix3(const Matrix *other);
inline Matrix * fmatrix4(const Matrix *other, int r1, int r2, int c1, int c2);
inline void     fmfree(Matrix *m);

inline Matrix *     fmadd      (const Matrix *m1, const Matrix *m2);
inline int          fmaddeq    (Matrix *m1, const Matrix *m2);
inline const float* fmget      (const Matrix *m1, int r, int c);
inline Matrix *     fmgetmatrix(const Matrix *m, const int *rows, int nrows, 
                                int c1, int c2);
inline Matrix *     fminvert   (const Matrix *m);
inline Matrix *     fminvert   (const Matrix *m);
inline Matrix *     fmmul      (const Matrix *m1, const Matrix *m2);
inline int          fmmuleq2   (const Matrix *m1, Matrix *m2);
inline int          fmmuleq    (Matrix *m1, const Matrix *m2);
inline int          fmmulstore (const Matrix *m1, const Matrix *m2,
                                Matrix *store);
//inline Matrix *     fmncols    (Matrix *m);
inline Matrix *     fmnegate   (const Matrix *m);
inline void         fmnegateeq (Matrix *m);
//inline Matrix *     fmnrows    (Matrix *m);
inline Matrix *     fmpower    (const Matrix *m, int p);
inline int          fmpowereq  (Matrix *m, int p);
inline Matrix *     fmscale    (const Matrix *m, float s);
inline void         fmscaleeq  (Matrix *m, float s);
inline int          fmset      (Matrix *m, int r, int c, float e);
inline Matrix *     fmsolve    (const Matrix *m1, const Matrix *m2);
//inline Matrix *     fmsubmatrix(Matrix *m, int r1, int r2, int c1, int c2);
inline Matrix *     fmsub      (const Matrix *m1, const Matrix *m2);
inline int          fmsubeq    (Matrix *m1, const Matrix *m2);
inline char *       fmtostring (const Matrix *m);
inline Matrix *     fmtranspose(const Matrix *m);

typedef struct LUDecomposition_t {
  Matrix *LU;
  int *piv;
} LUDecomposition;

inline LUDecomposition makeLUD(const Matrix *A);
inline Matrix *        solveLU(const LUDecomposition lud, const Matrix *B);
inline void            freeLUD(LUDecomposition lud);


inline Matrix* fmidentity(int nrows);

#ifdef __cplusplus
}
#endif

#endif  /* MATRIX */
