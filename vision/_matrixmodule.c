
#include "_matrixmodule.h"

static char * ModuleName = "_matrix";

static PyObject *MatrixError;  // forward reference

typedef struct MatrixObject_t {
  PyObject_HEAD
  //struct MatrixObject_t *parent;
  Matrix *matrix;
} MatrixObject;

inline PyObject * Matrix_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
inline int Matrix_init(MatrixObject *self, PyObject *args);
inline void Matrix_dealloc(MatrixObject *self);

inline PyObject * Matrix_add(MatrixObject *self, PyObject *args);
inline PyObject * Matrix_get(MatrixObject *self, PyObject *args);
inline PyObject * Matrix_invert(MatrixObject *self, PyObject *args);
inline PyObject * Matrix_multiply(MatrixObject *self, PyObject *args);
inline PyObject * Matrix_ncols(MatrixObject *self, PyObject *args);
inline PyObject * Matrix_negate(MatrixObject *self, PyObject *args);
inline PyObject * Matrix_nrows(MatrixObject *self, PyObject *args);
inline PyObject * Matrix_power(MatrixObject *self, PyObject *args);
inline PyObject * Matrix_scale(MatrixObject *self, PyObject *args);
inline PyObject * Matrix_set(MatrixObject *self, PyObject *args);
inline PyObject * Matrix_solve(MatrixObject *self, PyObject *args);
inline PyObject * Matrix_submatrix(MatrixObject *self, PyObject *args);
inline PyObject * Matrix_subtract(MatrixObject *self, PyObject *args);
inline PyObject * Matrix_transpose(MatrixObject *self, PyObject *args);
inline PyObject * Matrix_tostring(MatrixObject *self, PyObject *args);

inline PyObject * Matrix___add__(PyObject *arg1, PyObject *arg2);
inline PyObject * Matrix___mul__(PyObject *arg1, PyObject *arg2);
inline PyObject * Matrix___sub__(PyObject *arg1, PyObject *arg2);
inline PyObject * Matrix___str__(MatrixObject *self);

static PyMethodDef Matrix_methods[] = {
  /*
   * Matrix methods
   */
  {"add", (PyCFunction)Matrix_add, METH_VARARGS,
    "Return the result of adding the given Matrix to this one"},
  {"get", (PyCFunction)Matrix_get, METH_VARARGS,
    "Return the value at the given row and column coordinates"},
  {"invert", (PyCFunction)Matrix_invert, METH_NOARGS,
    "Return the multiplicative inverse of thie Matrix"},
  {"multiply", (PyCFunction)Matrix_multiply, METH_VARARGS, 
    "Return the result of multiplying this Matrix by the given one"},
  {"ncols", (PyCFunction)Matrix_ncols, METH_NOARGS, 
    "Return the number of columns in this Matrix"},
  {"negate", (PyCFunction)Matrix_negate, METH_NOARGS,
    "Return the additive inverse of this Matrix"},
  {"nrows", (PyCFunction)Matrix_nrows, METH_NOARGS,
    "Return the number of rows in thie Matrix"},
  {"scale", (PyCFunction)Matrix_scale, METH_VARARGS,
    "Return the result of scaling this MAtrix by the given coefficient"},
  {"set", (PyCFunction)Matrix_set, METH_VARARGS,
    "Set the given row and column member to the given value"},
  {"solve", (PyCFunction)Matrix_solve, METH_VARARGS,
    "Return the Matrix m such that m multiplied by this Matrix is equal to the given Matrix"},
  //{"submatrix", (PyCFunction)Matrix_submatrix, METH_VARARGS,
  //  "Return a new matrix object backed on the given protion of this Matrix.  Changes to this matrix will be reflected in the submatrix, and vice-versa.  To avoid altering the original (this) Matrix, use Matrix(m,a,b,c,d), where 'm' is a Matrix instance, to create a new Matrix copied from the given submatrix positions."},
  {"subtract", (PyCFunction)Matrix_subtract, METH_VARARGS,
    "Retuern the result of subtracting the given Matrix from this one"},
  {"tostring", (PyCFunction)Matrix_tostring, METH_NOARGS,
    "Return a string representation of this Matrix object"},
  {"transpose", (PyCFunction)Matrix_transpose, METH_NOARGS,
    "Return the transpose of this Matrix"},
  /*
   * Generic
   */
  {NULL} /* Sentinel */
};

static PyNumberMethods MatrixNumberMethods = {
    Matrix___add__,              /* nb_add */
    Matrix___sub__,              /* nb_subtract */
    Matrix___mul__,              /* nb_multiply */
    0,                           /* nb_divide */
    0,                           /* nb_remainder */
    0,                           /* nb_divmod */
    0,                           /* nb_power */
    (unaryfunc)Matrix_negate,    /* nb_negative */
    0,                           /* nb_positive */
    0,                           /* nb_absolute */
    0,                           /* nb_nonzero */
    (unaryfunc)Matrix_invert,    /* nb_invert */
    0,                           /* nb_lshift */
    0,                           /* nb_rshift */
    0,                           /* nb_and */
    0,                           /* nb_xor */
    0,                           /* nb_or */
    0,                           /* nb_coerce */
    0,                           /* nb_int */
    0,                           /* nb_long */
    0,                           /* nb_float */
    0,                           /* nb_oct */
    0,                           /* nb_hex */
    /* inplace operators */
//    0,                           /* nb_inplace_add */
//    0,                           /* nb_inplace_subtract */
//    0,                           /* nb_inplace_multiply */
//    0,                           /* nb_inplace_divide */
//    0,                           /* nb_inplace_remainder */
//    0,                           /* nb_inplace_power */
//    0,                           /* nb_inplace_lshift */
//    0,                           /* nb_inplace_rshift */
//    0,                           /* nb_inplace_and */
//    0,                           /* nb_inplace_xor */
//    0,                           /* nb_inplace_or */
};

static PyTypeObject MatrixType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "matrix.Matrix",           /*tp_name*/
    sizeof(MatrixObject),      /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Matrix_dealloc,/*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    (unaryfunc)Matrix___str__, /*tp_repr*/
    &MatrixNumberMethods,      /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    (unaryfunc)Matrix___str__, /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_CHECKTYPES, /*tp_flags*/
    "",                        /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    Matrix_methods,            /* tp_methods */
    0,                         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)Matrix_init,     /* tp_init */
    0,                         /* tp_alloc */
    Matrix_new,                /* tp_new */
};

inline void
Matrix_dealloc(MatrixObject* self)
{
    if (self->matrix != NULL)
        fmfree(self->matrix);
    self->ob_type->tp_free((PyObject*)self);
}

inline PyObject *
Matrix_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    MatrixObject *self;

    self = (MatrixObject *)type->tp_alloc(type, 0);
    if (self) {
      self->matrix = NULL;
      //self->parent = NULL;
    }
    return (PyObject *)self;
}

/*
inline int
Matrix_init(MatrixObject *self, MatrixObject *other) {
  Matrix *m = new Matrix(other->matrix);
  if (m != NULL) {
    self->matrix = m;
    return 0;
  }else
    return -1;
}

inline int
Matrix_init(MatrixObject *self, Matrix *m, MatrixObject *parent) {
  self->matrix = m;
  self->parent = parent;
  Py_INCREF(self->parent);
  return 0;
}

inline int
Matrix_init(MatrixObject *self, MatrixObject *other, int r1, int r2,
    int c1, int c2) {
  self->matrix = fmatrix4(other->matrix, r1, r2, c1, c2);
  //self->parent = NULL;
  return 0;
}
*/


inline int
Matrix_init(MatrixObject *self, PyObject *args)
{
    PyObject *o;
    MatrixObject *other;
    int rows, cols;
    float value = 0;

    if (PyTuple_Size(args) == 1) {
      o = PyTuple_GetItem(args, 0);

      if (PyObject_TypeCheck(o, &MatrixType)) {
        self->matrix = fmatrix3(((MatrixObject*)o)->matrix);
        return self->matrix == NULL ? -1 : 0;

      }else {
        char err_msg[100];
        sprintf(err_msg, "__init__() got 1 argument, expected Matrix not %s",
            o->ob_type->tp_name);
        PyErr_SetString(PyExc_TypeError, err_msg);
        return -1;

      }
    }else if (PyTuple_Size(args) == 5) {
      int r1, r2, c1, c2;
      if (PyArg_ParseTuple(args, "Oiiii:__init__", 
            &other, &r1, &r2, &c1, &c2)) {
        self->matrix = fmatrix4(other->matrix, r1, r2, c1, c2);
        return self->matrix == NULL ? -1 : 0;
        //return Matrix_init(self, other, r1, r2, c1, c2);
      }else
        return -1;
    }
    
    if (!PyArg_ParseTuple(args, "ii|f:__init__", &rows, &cols, &value))
      return -1;
    
    if (PyTuple_Size(args) == 2)
      self->matrix = fmatrix(rows, cols);
    else
      self->matrix = fmatrix2(rows, cols, value);
    
    return 0;
}

inline PyObject *
Matrix_add(MatrixObject *self, PyObject *args) {
  MatrixObject *other, *result = NULL;
  Matrix *m;

  if (PyArg_ParseTuple(args, "O!:add", &MatrixType, &other)){
    m = fmadd(self->matrix, other->matrix);

    if (m != NULL) {
      result = (MatrixObject*)Matrix_new(&MatrixType, NULL, NULL);
      if (result != NULL)
        result->matrix = m;
      else
        fmfree(m);
    }else
      PyErr_SetString(MatrixError, "add(): illegal row or column count for addition");
  }
  
  return (PyObject*)result;
}

inline PyObject *
Matrix_get(MatrixObject *self, PyObject *args) {
  int row, col;
  PyObject *result = NULL;

  if (PyArg_ParseTuple(args, "ii:get", &row, &col)) {
    const float *value = fmget(self->matrix, row, col);
    if (value == NULL)
      PyErr_SetFromErrno(MatrixError);
    else
      result = Py_BuildValue("f", *value);
  }

  return result;
}

inline PyObject *
Matrix_invert(MatrixObject *self, PyObject * args) {
  MatrixObject *result = NULL;
  Matrix *inverse;

  inverse = fminvert(self->matrix);

  if (inverse != NULL) {
    result = (MatrixObject*)Matrix_new(&MatrixType, NULL, NULL);

    if (result != NULL)
      result->matrix = inverse;
    else
      fmfree(inverse);
  }else
    PyErr_SetFromErrno(MatrixError);

  return (PyObject*)result;
}


inline PyObject *
Matrix_multiply(MatrixObject *self, PyObject *args) {
  MatrixObject *other, *result;
  Matrix *m;

  if (PyArg_ParseTuple(args, "O!:multiply", &MatrixType, &other)){
    m = fmmul(self->matrix, other->matrix);

    if (m != NULL) {
      result = (MatrixObject*)Matrix_new(&MatrixType, NULL, NULL);
      if (result != NULL)
        result->matrix = m;
      else
        fmfree(m);
    }else
      PyErr_SetString(MatrixError, "multiply(): illegal row or column count for multiplication");
  }

  return (PyObject*)result;
}

inline PyObject *
Matrix_ncols(MatrixObject *self, PyObject * args) {
  return PyInt_FromLong(self->matrix->cols);
}

inline PyObject *
Matrix_negate(MatrixObject *self, PyObject * args) {
  MatrixObject *result = NULL;
  Matrix *m;

  m = fmnegate(self->matrix);

  if (m != NULL) {
    result = (MatrixObject*)Matrix_new(&MatrixType, NULL, NULL);
    result->matrix = m;
  }else
    PyErr_SetString(MatrixError, "negate(): error occurred in negation");

  return (PyObject*)result;
}

inline PyObject *
Matrix_nrows(MatrixObject *self, PyObject * args) {
  return PyInt_FromLong(self->matrix->rows);
}

inline PyObject *
Matrix_scale(MatrixObject *self, PyObject *args) {
  float scalar;
  MatrixObject *result = NULL;
  Matrix *m;

  if (PyArg_ParseTuple(args, "f:scale", &scalar)) {
    m = fmscale(self->matrix, scalar);

    if (m != NULL) {
      result = (MatrixObject*)Matrix_new(&MatrixType, NULL, NULL);
      if (result != NULL)
        result->matrix = m;
      else
        fmfree(m);
    }else
      PyErr_SetString(MatrixError, "scale(): error occurred in scaling");
  }

  return (PyObject*)result;
}

inline PyObject *
Matrix_set(MatrixObject *self, PyObject *args) {
  int row, col, success;
  float value;
  PyObject *result = NULL;

  if (PyArg_ParseTuple(args, "iif:set", &row, &col, &value)) {
    success = fmset(self->matrix, row, col, value);

    if (success == 0) {
      Py_INCREF(Py_None);
      result = Py_None;
    }else
      PyErr_SetFromErrno(MatrixError);
  }

  return result;
}

inline PyObject *
Matrix_solve(MatrixObject *self, PyObject *args) {
  MatrixObject *other, *result = NULL;
  Matrix *solution;
  
  if (PyArg_ParseTuple(args, "O!:solve", &MatrixType, &other)) {
    solution = fmsolve(self->matrix, other->matrix);

    if (solution != NULL) {
      result = (MatrixObject*)Matrix_new(&MatrixType, NULL, NULL);
      
      if (result != NULL)
        result->matrix = solution;
      else
        fmfree(solution);
    }else
      PyErr_SetFromErrno(MatrixError);
  }

  return (PyObject*)result;
}

/*
inline PyObject *
Matrix_submatrix(MatrixObject *self, PyObject *args) {
  int r1, r2, c1, c2;
  PyObject *result = NULL;
  Matrix *m;

  if (PyArg_ParseTuple(args, "iiii:submatrix", &r1, &r2, &c1, &c2)) {
    m = self->matrix->submatrix(r1, r2, c1, c2);

    if (m != NULL) {
      MatrixObject *mo = (MatrixObject*)Matrix_new(&MatrixType, NULL, NULL);
      if (Matrix_init(mo, m, self) == 0)
        result = (PyObject*)mo;
      else { 
        Matrix_dealloc(mo);
        PyErr_SetString(MatrixError, "submatrix(): error occured in initialization");
      }
    }else
      PyErr_SetFromErrno(MatrixError);
  }

  return result;
}
*/

inline PyObject *
Matrix_subtract(MatrixObject *self, PyObject *args) {
  MatrixObject *other, *result = NULL;
  Matrix *m;

  if (PyArg_ParseTuple(args, "O!:subtract", &MatrixType, &other)) {
    m = fmsub(self->matrix, other->matrix);

    if (m != NULL) {
      result = (MatrixObject*)Matrix_new(&MatrixType, NULL, NULL);
      if (result != NULL)
        result->matrix = m;
      else
        fmfree(m);
    }else
      PyErr_SetString(MatrixError, "substract(): illegal row or column count for subtraction");
  }

  return (PyObject*)result;
}

inline PyObject * 
Matrix_tostring(MatrixObject *self, PyObject * args) {
  const char *str = fmtostring(self->matrix);

  return PyString_FromString(str);
}

inline PyObject *
Matrix_transpose(MatrixObject *self, PyObject * args) {
  MatrixObject *result = NULL;
  Matrix *m;

  m = fmtranspose(self->matrix);

  if (m != NULL) {
    result = (MatrixObject*)Matrix_new(&MatrixType, NULL, NULL);
    if (result != NULL)
      result->matrix = m;
    else
      fmfree(m);
  }else
    PyErr_SetString(MatrixError, "transpose(): error occurred in transpose");

  return (PyObject*)result;
}

inline PyObject *
Matrix___add__(PyObject *arg1, PyObject *arg2) {
  char err_msg[100];
  PyObject *value, *result = NULL;

  if (!PyObject_TypeCheck(arg1, &MatrixType) || 
      !PyObject_TypeCheck(arg2, &MatrixType)) {
    sprintf(err_msg, "unsupported operand type(s) for +: '%s' and '%s'",
        arg1->ob_type->tp_name, arg2->ob_type->tp_name);
    PyErr_SetString(PyExc_TypeError, err_msg);
    return NULL;
  }

  value = Py_BuildValue("(O)", arg2);
  if (value != NULL) {
    result = Matrix_add((MatrixObject*)arg1,value);
    Py_DECREF(value);
  }

  return result;
}

inline PyObject *
Matrix___mul__(PyObject *arg1, PyObject *arg2) {
  MatrixObject *result = NULL, *self, *other;
  PyObject *float_obj;
  Matrix *m;
  char err_msg[100];

  if (PyObject_TypeCheck(arg1, &MatrixType) && 
      PyObject_TypeCheck(arg2, &MatrixType)) {
    self = (MatrixObject*)arg1;
    other = (MatrixObject*)arg2;
    m = fmmul(self->matrix, other->matrix);

    if (m != NULL) {
      result = (MatrixObject*)Matrix_new(&MatrixType, NULL, NULL);
      if (result != NULL)
        result->matrix = m;
      else
        fmfree(m);
    }else
      PyErr_SetString(MatrixError, "__mul__(): illegal row or column count for multiplication");
  }else if (PyNumber_Check(arg1) || PyNumber_Check(arg2)) {
    float scalar;
    if (PyObject_TypeCheck(arg1, &MatrixType)) {
      self = (MatrixObject*)arg1;
      float_obj = PyNumber_Float(arg2);
      scalar = (float)PyFloat_AsDouble(float_obj);
      Py_DECREF(float_obj);
    }else {
      float_obj = PyNumber_Float(arg1);
      scalar = (float)PyFloat_AsDouble(float_obj);
      self = (MatrixObject*)arg2;
      Py_DECREF(float_obj);
    }
    
    m = fmscale(self->matrix, scalar);
    
    if (m != NULL) {
      result = (MatrixObject*)Matrix_new(&MatrixType, NULL, NULL);
      if (result != NULL)
        result->matrix = m;
      else
        fmfree(m);
    }else
      PyErr_SetString(MatrixError, "__mul__(): error occurred in scaling");
  }else {
    sprintf(err_msg, "unsupported operand type(s) for *: '%s' and '%s'",
      arg1->ob_type->tp_name, arg2->ob_type->tp_name);
    PyErr_SetString(PyExc_TypeError, err_msg);
  }

  return (PyObject*)result;
}

inline PyObject *
Matrix___sub__(PyObject *arg1, PyObject *arg2) {
  char err_msg[100];
  PyObject *value, *result = NULL;

  if (!PyObject_TypeCheck(arg1, &MatrixType) || 
      !PyObject_TypeCheck(arg2, &MatrixType)) {
    sprintf(err_msg, "unsupported operand type(s) for -: '%s' and '%s)",
        arg1->ob_type->tp_name, arg2->ob_type->tp_name);
    PyErr_SetString(PyExc_TypeError, err_msg);
    return NULL;
  }

  value = Py_BuildValue("(O)", arg2);
  if(value != NULL) {
    result = Matrix_subtract((MatrixObject*)arg1, value);
    Py_DECREF(value);
  }

  return result;
}

inline PyObject *
Matrix___str__(MatrixObject *self) {
  return Matrix_tostring(self, NULL);
}

inline PyObject *
matrix_identity(PyObject * self, PyObject *args) {
  int size;
  MatrixObject *result = NULL;
  Matrix *m;

  if (PyArg_ParseTuple(args, "i:identity", &size)) {
    m = fmidentity(size);

    if (m != NULL) {
      result = (MatrixObject*)Matrix_new(&MatrixType, NULL, NULL);
      if (result != NULL)
        result->matrix = m;
      else
        fmfree(m);
    }else
      PyErr_SetString(MatrixError, "identity: error occurred in identity");
  }

  return (PyObject*)result;
}

inline PyObject *
matrix_withvalues(PyObject * self, PyObject *args) {
  int row, col, i;
  int size1, size2;
  PyObject *seq, *fl, *cur;
  MatrixObject *result = NULL;
  Matrix *m;

  if (PyArg_ParseTuple(args, "iiO:withvalues", &row, &col, &seq)) {
    if (PySequence_Check(seq)) {
      m = fmatrix(row, col);

      if (m != NULL) {
        size1 = PySequence_Size(seq);
        size2 = row*col;
        i = 0;
        while (i < size1 && i < size2) {
          cur = PySequence_GetItem(seq, i);
          fl = PyNumber_Float(cur);
          if (fl != NULL)
            fmset(m, i/col, i%col, (float)PyFloat_AsDouble(fl));
          else
            return NULL;
          Py_DECREF(fl);
          Py_DECREF(cur);
          i++;
        }

        result = (MatrixObject*)Matrix_new(&MatrixType, NULL, NULL);
        if (result != NULL)
          result->matrix = m;
        else
          fmfree(m);
      }else
        PyErr_SetString(PyExc_TypeError, "iteration over non-sequence");
    }
  }
    
  return (PyObject*)result;
}

static PyMethodDef module_methods[] = {
  /*
   * Generic
   */
  {"identity", matrix_identity, METH_VARARGS,
    "Return an identity Matrix of the given size"},
  {"withvalues", matrix_withvalues, METH_VARARGS,
    "Create and return a new Matrix with the given row and column width/height, filling the matrix with the given list of values"},
  {NULL}  /* Sentinel */
};

PyMODINIT_FUNC
init_matrix(void)
{
    if (!Py_IsInitialized())
      Py_Initialize();

    PyObject* m;
    m = Py_InitModule3(ModuleName, module_methods,
                       "Module that creates an extension type.");

    MatrixError = PyErr_NewException("matrix.error", NULL, NULL);
    Py_INCREF(MatrixError);
    PyModule_AddObject(m, "error", MatrixError);
    
    MatrixType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&MatrixType) < 0)
        return;

    Py_INCREF(&MatrixType);
    PyModule_AddObject(m, "Matrix", (PyObject *)&MatrixType);
}
