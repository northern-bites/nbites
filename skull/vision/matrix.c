
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>


#include "matrix.h"

#ifdef __cplusplus
extern "C" {
#endif

////////////
// Matrix //
////////////

inline Matrix *
fmatrix (int nrows, int ncols) {
  Matrix *m;
  float *data;
  
  m = (Matrix*) malloc(sizeof(Matrix));
  if (m == NULL)
    return NULL;
  data = (float*)malloc(nrows * ncols * sizeof(float));
  if (data == NULL)
    return NULL;

  m->data = data;
  m->rows = nrows;
  m->cols = ncols;
  //m->r1 = 0;
  //m->r2 = nrows;
  //m->c1 = 0;
  //m->c2 = ncols;

  return m;
}

inline Matrix *
fmatrix2 (int nrows, int ncols, float value) {
  Matrix *m;
  float *ptr;
  int length;

  m = fmatrix(nrows, ncols);
  if (m == NULL)
    return NULL;
  ptr = m->data;
  length = m->rows * m->cols;

  while (ptr < m->data + length)
    *ptr++ = value;
  
  return m;
}

inline Matrix *
fmatrix3 (const Matrix *other) {
  Matrix *m;

  m = fmatrix(other->rows, other->cols);
  if (m == NULL)
    return NULL;
  memcpy(m->data, other->data, m->rows * m->cols * sizeof(float));

  return m;
}

inline Matrix *
fmatrix4 (const Matrix *other, int r1, int r2, int c1, int c2) {
  Matrix *m;
  int r;

  //m = fmatrix(gr2 - r1, gc2 - gc1);
  m = fmatrix(r2 - r1, c2 - c1);
  if (m == NULL)
    return NULL;

  r = 0;
  while (r < m->rows) {
    memcpy(m->data + r * m->cols,
           other->data + (r1 + r) * other->cols + c1,
           m->cols * sizeof(float));
    r++;
  }

  return m;
}

inline void
fmfree (Matrix *m) {
  free(m->data);
  free(m);
}

inline const float *
fmget (const Matrix *m, int r, int c) {
  //if (r < 0 || r > (m->r2 - m->r1) || c < 0 || c > (m->c2 - m->c1)) {
  if (r < 0 || r >= m->rows || c < 0 || c > m->cols) {
    errno = EINVAL;
    return NULL;
  }
  return m->data + r * m->cols + c;
}

inline int
fmset (Matrix *m, int r, int c, float f) {
  //if (r < 0 || r > (m->r2 - m->r1) || c < 0 || c > (m->c2 - m->c1)) {
  if (r < 0 || r > m->rows || c < 0 || c > m->cols) {
    errno = EINVAL;
    return -1;
  }
  *(m->data + r * m->cols + c) = f;
  return 0;
}

inline Matrix *
fmadd (const Matrix *m1, const Matrix *m2) {
  Matrix *m;
  int r, c;

  if (m1->rows != m2->rows || m1->cols != m2->cols) {
    errno = EINVAL;
    return NULL;
  }

  m = fmatrix(m1->rows, m1->cols);
  if (m == NULL)
    return NULL;

  r = 0;
  while (r < m1->rows) {
    c = 0;
    while (c < m1->cols) {
      fmset(m, r, c, *fmget(m1,r,c) + *fmget(m2,r,c));
      c++;
    }
    r++;
  }

  return m;
}
  
inline int
fmaddeq (Matrix *m1, const Matrix *m2) {
  int r, c;

  if (m1->rows != m2->rows || m1->cols != m2->cols) {
    errno = EINVAL;
    return -1;
  }

  r = 0;
  while (r < m1->rows) {
    c = 0;
    while (c < m1->cols) {
      fmset(m1, r, c, *fmget(m1,r,c) + *fmget(m2,r,c));
      c++;
    }
    r++;
  }

  return 0;
}

inline Matrix *
fmgetmatrix (const Matrix *other, const int *rows, int nrows, int c1, int c2) {
  Matrix *m;
  int r, c;

  m = fmatrix(nrows, c2 - c1);
  if (m == NULL)
    return NULL;

  r = 0;
  while (r < m->rows) {
    c = 0;
    while (c < m->cols) {
      fmset(m, r, c, *fmget(other, rows[r], c1 + c));
      c++;
    }
    r++;
  }

  return m;
}

inline Matrix *
fminvert (const Matrix *other) {
  Matrix *result;
  float det;
  
  if (other->rows == 2 && other->cols == 2) {
    result = fmatrix(2,2);
    fmset(result,0,0, *fmget(other,1,1));
    fmset(result,0,1, *fmget(other,0,1) * -1);
    fmset(result,1,0, *fmget(other,1,0) * -1);
    fmset(result,1,1, *fmget(other,0,0));

    det = (*fmget(other,0,0) * *fmget(other,1,1)) -
          (*fmget(other,0,1) * *fmget(other,1,0));
    if (det != 0)
      fmscaleeq(result, 1.0 / det);
    else {
      fmfree(result);
      result = NULL;
      errno = EDOM;
    }
  }else {
    Matrix *id = fmidentity(other->rows);
    result = fmsolve(other, id);
    fmfree(id);
  }

  return result;
}

inline Matrix *
fmmul (const Matrix *m1, const Matrix *m2) {
  Matrix *m;
  int r, c, n;
  float sum;
  
  if (m1->cols != m2->rows) {
    errno = EINVAL;
    return NULL;
  }
  
  m = fmatrix(m1->rows, m2->cols);
  if (m == NULL)
    return NULL;
  
  r = 0;
  while (r < m1->rows) {
    c = 0;
    while (c < m2->cols) {
      sum = 0;
      n = 0;
      while (n < m1->cols) {
        sum += *fmget(m1, r, n) * *fmget(m2, n, c);
        n++;
      }
      fmset(m, r, c, sum);
      c++;
    }
    r++;
  }

  return m;
}

inline int
fmmuleq (Matrix *m1, const Matrix *m2) {
  Matrix *m;

  m = fmmul(m1, m2);
  if (m == NULL)
    return -1;

  // free old matrix data
  free(m1->data);
  // copy row,col info and data pointer into m2 matrix
  *m1 = *m;
  // free actual matrix struct (data pointer is in m1 now)
  free(m);

  return 0;
}

inline int
fmmuleq2 (const Matrix *m1, Matrix *m2) {
  Matrix *m;

  m = fmmul(m1, m2);
  if (m == NULL)
    return -1;

  // free old matrix data
  free(m2->data);
  // copy row,col info and data pointer into m2 matrix
  *m2 = *m;
  // free actual matrix struct (data pointer is in m2 now)
  free(m);
  return 0;
}

inline int
fmmulstore (const Matrix *m1, const Matrix *m2, Matrix *store) {
  int r, c, n;
  float sum, *tmp;
  
  if (m1->rows != m2->cols) {
    errno = EINVAL;
    return -1;
  }
  if (m1->rows != store->rows || m2->cols != store->cols) {
    store->rows = m1->rows;
    store->cols = m2->cols;
    tmp = (float*)realloc(store->data,
                          store->rows * store->cols * sizeof(float));
    if (tmp == NULL)
      return -1;
    store->data = tmp;
  }

  r = 0;
  while (r < store->rows) {
    c = 0;
    while (c < store->cols) {
      sum = 0;
      n = 0;
      while (n < m1->cols) {
        sum += *fmget(m1, r, n) * *fmget(m2, n, c);
        n++;
      }
      fmset(store, r, c, sum);
      c++;
    }
    r++;
  }

  return 0;
}

inline Matrix *
fmnegate (const Matrix *other) {
  Matrix *m;
  int r, c;

  m = fmatrix(other->rows, other->cols);
  if (m == NULL)
    return NULL;

  r = 0;
  while (r < m->rows) {
    c = 0;
    while (c < m->cols) {
      fmset(m, r, c, -(*fmget(other, r, c)));
      c++;
    }
    r++;
  }

  return m;
}

inline void
fmnegateeq (Matrix *m) {
  int r, c;

  r = 0;
  while (r < m->rows) {
    c = 0;
    while (c < m->cols) {
      fmset(m, r, c, -(*fmget(m, r, c)));
      c++;
    }
    r++;
  }
}

inline Matrix *
fmpower (const Matrix *other, int pow) {
  Matrix *m;
  int i;

  if (pow > 1) {
    m = fmatrix3(other);
    if (m == NULL)
      return NULL;
    i = 2;
  }else {
    errno = EINVAL;
    return NULL;
  }

  if (fmpowereq(m, pow) != 0)
    return NULL;
  return m;
}

inline int
fmpowereq (Matrix *m, int pow) {
  Matrix *tmp, *tmp2, *tmp3;
  int i;

  if (pow > 1) {
    tmp = fmmul(m, m);
    if (tmp == NULL)
      return -1;
    if (pow != 2) {  
      tmp2 = fmatrix(m->rows, m->cols);
      if (tmp2 == NULL) {
        fmfree(tmp);
        return -1;
      }
    }else
      tmp2 = tmp;
    i = 2;
  }else {
    errno = EINVAL;
    return -1;
  }

  while (i < pow) {
    if (fmmulstore(m, tmp, tmp2) != 0) {
      fmfree(tmp);
      fmfree(tmp2);
      return -1;
    }
    tmp3 = tmp;
    tmp = tmp2;
    tmp2 = tmp3;
    i++;
  }

  free(m->data);        // Clear old data
  *m = *tmp2;           // Set m to the power result
  free(tmp2);           // Free the tmp2 Matrix (but not its data!)
  if (tmp != tmp2)      // Free the tmp1 Matrix fully, if not the same
    fmfree(tmp);        //   (which would happen when pow == 2) 
                 
  return 0;
}

inline Matrix *
fmscale (const Matrix *other, float scalar) {
  Matrix *m;
  int r, c;

  m = fmatrix(other->rows, other->cols);
  if (m == NULL)
    return NULL;

  r = 0;
  while (r < m->rows) {
    c = 0;
    while (c < m->cols) {
      fmset(m, r, c, scalar * *fmget(other, r, c));
      c++;
    }
    r++;
  }

  return m;
}

inline void
fmscaleeq (Matrix *m, float scalar) {
  int r, c;

  r = 0;
  while (r < m->rows) {
    c = 0;
    while (c < m->cols) {
      fmset(m, r, c, scalar * *fmget(m, r, c));
      c++;
    }
    r++;
  }
}

inline Matrix *
fmsolve (const Matrix *m1, const Matrix *m2) {
  LUDecomposition lud;
  Matrix *result;

  lud = makeLUD(m1);
  result = solveLU(lud, m2);

  freeLUD(lud);
  return result;
}

inline Matrix *
fmsub (const Matrix *m1, const Matrix *m2) {
  Matrix *m;
  int r, c;

  if (m1->rows != m2->rows || m1->cols != m2->cols) {
    errno = EINVAL;
    return NULL;
  }

  m = fmatrix(m1->rows, m1->cols);
  if (m == NULL)
    return NULL;

  r = 0;
  while (r < m->rows) {
    c = 0;
    while (c < m->cols) {
      fmset(m, r, c, *fmget(m1,r,c) - *fmget(m2,r,c));
      c++;
    }
    r++;
  }

  return m;
}

inline int
fmsubeq (Matrix *m1, const Matrix *m2) {
  int r, c;

  if (m1->rows != m2->rows || m1->cols != m2->cols) {
    errno = EINVAL;
    return -1;
  }

  r = 0;
  while (r < m1->rows) {
    c = 0;
    while (c < m1->rows) {
      fmset(m1, r, c, *fmget(m1,r,c) - *fmget(m2,r,c));
      c++;
    }
    r++;
  }

  return 0;
}

inline char *
fmtostring (const Matrix *m) {
  char *str;
  int length, r, c, i;
  
  //    '['  "0.0 1.2 3.4\n"
  length = (1 + 8 * m->cols) * m->rows;
  if (length == 0)
    length = 2;

  str = (char*) malloc((length + 1) * sizeof(char));

  i = 0;
  r = 0;
  while (r < m->rows) {
    str[i] = ' ';
    i++;
    c = 0;
    while (c < m->cols) {
      i += sprintf(str + i, "%7.2G ", *fmget(m, r, c));
      c++;
    }
    str[i - 1] = '\n';
    r++;
  }
  
  str[0] = '[';
  str[i - 1] = ']';
  str[i] = '\0';

  return str;
}

inline Matrix *
fmtranspose (const Matrix *other) {
  Matrix *m;
  int r, c;

  m = fmatrix(other->cols, other->rows);
  if (m == NULL)
    return NULL;

  r = 0;
  while (r < m->rows) {
    c = 0;
    while (c < m->cols) {
      fmset(m, r, c, *fmget(other,c,r));
      c++;
    }
    r++;
  }

  return m;
}


/////////////////////
// LUDecomposition //
/////////////////////

inline LUDecomposition 
makeLUD (const Matrix *A) {
  LUDecomposition lud;
  int m, n, pivsign, i, j, k, p;
  float tmp;

  lud.LU = fmatrix3(A);
  m = A->rows;
  n = A->cols;

  lud.piv = (int*) malloc(m * sizeof(int));
  i = 0;
  while (i < m) {
    lud.piv[i] = i;
    i++;
  }

  pivsign = 1;
  
  k = 0;
  while (k < n) {
    p = k;
    i = k + 1;
    while (i < m) {
      if (fabs(*fmget(lud.LU,i,k)) > fabs(*fmget(lud.LU,p,k)))
        p = i;
      i++;
    }
    
    if (p != k) {
      j = 0;
      while (j < n) {
        tmp = *fmget(lud.LU,p,j);
        fmset(lud.LU, p, j, *fmget(lud.LU,k,j));
        fmset(lud.LU, k, j, tmp);
        j++;
      }
      i = lud.piv[p];
      lud.piv[p] = lud.piv[k];
      lud.piv[k] = i;
      pivsign = -pivsign;
    }
    if (*fmget(lud.LU,k,k) != 0) {
      i = k + 1;
      while (i < m) {
        fmset(lud.LU, i, k, *fmget(lud.LU,i,k) / *fmget(lud.LU,k,k));
        j = k + 1;
        while (j < n) {
          fmset(lud.LU, i, j, 
                (*fmget(lud.LU,i,j) - *fmget(lud.LU,i,k) * *fmget(lud.LU,k,j))
                );
          j++;
        }
        i++;
      }
    }

    k++;
  }

  return lud;
}

inline void
freeLUD (LUDecomposition lud) {
  fmfree(lud.LU);
  free(lud.piv);
}

inline Matrix *
solveLU (const LUDecomposition lud, const Matrix *B) {
  Matrix *X;
  int m, n, nx, i, j, k;

  nx = B->cols;
  m = lud.LU->rows;
  n = lud.LU->cols;

  X = fmgetmatrix(B, lud.piv, lud.LU->rows, 0, nx);
  assert(X);
  
  k = 0;
  while (k < n) {
    i = k + 1;
    while (i < n) {
      j = 0;
      while (j < nx) {
        fmset(X, i, j, 
              (*fmget(X,i,j) - *fmget(X,k,j) * *fmget(lud.LU,i,k)));
        j++;
      }
      i++;
    }
    k++;
  }

  k = n - 1;
  while (k >= 0) {
    j = 0;
    while (j < nx) {
      fmset(X, k, j, *fmget(X,k,j) / *fmget(lud.LU,k,k));
      j++;
    }
    i = 0;
    while (i < k) {
      j = 0;
      while (j < nx) {
        fmset(X, i, j, 
              (*fmget(X,i,j) - *fmget(X,k,j) * *fmget(lud.LU,i,k)));
        j++;
      }
      i++;
    }
    k--;
  }
  
  return X;
}

/////////////////////
// General methods //
/////////////////////

inline Matrix *
fmidentity (int nrows) {
  Matrix *m;
  int i;

  m = fmatrix2(nrows, nrows, 0);
  i = 0;
  while (i < nrows) {
    fmset(m, i, i, 1);
    i++;
  }

  return m;
}


#ifdef __cplusplus
}
#endif

