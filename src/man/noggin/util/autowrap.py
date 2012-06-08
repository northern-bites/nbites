import imp
import pydoc
import os
import types

class Skeleton(object):
    def __init__(self, name, dirpath=''):
        self.name = name
        self.dirpath = dirpath
        self.types = []
        self.constants = {}
        self.instances = {}

    def header_name(self):
        return os.path.join(self.dirpath, '_' + self.name + 'module.h')

    def wrapper_name(self):
        return os.path.join(self.dirpath, '_' + self.name + 'module.cpp')

    def module_name(self):
        return os.path.join(self.dirpath, self.name + '.py')


def wrap_module(path):
    module = import_module(path)
    skel = process_module(module)
    generate_header_form(skel)
    generate_wrapper_form(skel)
    generate_module_form(skel)

def import_module(path):
    dirpath, mod_file = os.path.split(path)
    mod_name = os.path.splitext(mod_file)[0]
    full_name = os.path.splitext(path)[0]
    #if os.path.exists(os.path.join(dirpath, '__init__.py')):
    #    dirpath = os.path.join(dirpath, '__init__.py')

    if dirpath:
        info = imp.find_module(mod_name, [dirpath])
    else:
        info = imp.find_module(mod_name)
    return imp.load_module(full_name, *info)

def process_module(module):
    '''
    Process a module and its declared types and instance variables to
    generate a Skeleton wrapper definition.
    '''
    name = os.path.basename(module.__name__)
    dirpath = os.path.dirname(module.__file__)
    skel = Skeleton(name, dirpath)
    objs = dict((attr, getattr(module, attr)) for attr in dir(module))

    type_objs = [t for t in objs.itervalues() if type(t) is types.TypeType]
    skel.types = type_objs

    instances = dict((name, o) for name, o in objs.iteritems()
                                if type(o) in type_objs)
    skel.instances = instances

    constants = dict((name, o) for name, o in objs.iteritems()
                         if o not in type_objs and
                            name not in instances and
                            not name.startswith('__'))

    skel.constants = constants

    return skel


def generate_header_form(skel):
    '''
    Generate a C++ header file wrapping the given skeleton definition.
    File is output to [skel.dirpath]/_[skel.name]module.h
    '''
    f = file(skel.header_name(), 'w')
    f.write('\n')
    write_warning(f)
    write_copyright(f)
    write_header_includes(f, skel)
    write_types(f, skel)

def write_warning(f):
    '''Write a warning to a file indicating it was generated automatically'''
    f.write('''\
//  WARNING, this file was automatically generated using the Northern
//  Bites' Python wrapper extension module generator.  Subsequent
//  form-completion was done by hand.


''')

def write_copyright(f):
    '''Write a GPLv3 copyright notice to a file.'''
    f.write('''\
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//   along with this program.  If not, see <http://www.gnu.org/licenses/>.

''')

def write_header_includes(f, skel):
    '''Write C++ header includes for a Skeleton definition to a file.'''
    f.write('''\
#include <Python.h>
#include <structmember.h>
#include "%s.h"

''' % skel.name)

def write_types(f, skel):
    '''
    Write documentation and definitions for the types in a Skeleton to
    a file.
    '''
    for t in skel.types:
        write_type(f, skel, t)

def write_type(f, skel, t):
    '''Write documentation and definitions for a type to a file.'''

    funcs = dict((attr, val) for attr, val in t.__dict__.iteritems()
                    if not attr.startswith('__') and
                       type(val) is types.FunctionType)
    attrs = dict((attr, getattr(t, attr))
                    for attr, val in t.__dict__.iteritems()
                    if not attr.startswith('__') and attr not in funcs)
    f.write('''\
//
// Py%s definitions
//

''' % t.__name__)

    write_type_struct(f, t, funcs, attrs)
    write_type_methods(f, t, funcs, attrs)
    write_type_members(f, t, funcs, attrs)
    write_type_attrs(f, t, funcs, attrs)
    write_type_object(f, skel, t, funcs, attrs)

def write_type_struct(f, t, funcs, attrs):
    '''Write the 'PyFoo_t' struct definition for the 'PyFoo' type.'''
    f.write('''\
typedef struct Py%s_t
{
    PyObject_HEAD
    %s* _%s;
''' % (t.__name__, t.__name__, t.__name__.lower()))

    if attrs:
        f.write('\n')
    for attr in attrs:
        f.write('    PyObject* %s;\n' % attr)
    f.write('''\
} Py%(type)s;


''' % {'type':t.__name__})

def write_type_methods(f, t, funcs, attrs):
    '''Write the PyMethodDef array for a type to a file.'''
    f.write('''\
// backend methods
extern PyObject* Py%(type)s_new (PyTypeObject* type, PyObject* args,
    PyObject* kwds);
extern int Py%(type)s_init (PyObject* self, PyObject* arg, PyObject* kwds);
extern void Py%(type)s_dealloc (PyObject* self);
// C++ - accessible interface
extern PyObject* Py%(type)s_new (%(type)s* _%(lower)s);
// Python - accesible interface
''' % {'type':t.__name__, 'lower':t.__name__.lower()})

    for func in sorted(funcs.keys()):
        f.write('extern PyObject* Py%(type)s_%(func)s (PyObject* self, '
                'PyObject* args);\n' % {'type':t.__name__, 'func':func})

    f.write('''\

// backend method list
static PyMethodDef Py%(type)s_methods[] = {

''' % {'type':t.__name__})

    for func in sorted(funcs.keys()):
        f.write('''\
    {"%(func)s", reinterpret_cast<PyCFunction>(Py%(type)s_%(func)s),
      METH_VARARGS,
      "%(doc)s"},

''' % {'type':t.__name__, 'func':func,
       'doc':funcs[func].__doc__.replace('\n', '\\n')})

    f.write('''\
    {NULL} // Sentinel
};

''')

def write_type_members(f, t, funcs, attrs):
    '''Write the PyMemberDef array for a type to a file.'''
    f.write('''\
// backend member list
static PyMemberDef Py%(type)s_members[] = {

''' % {'type':t.__name__})

    for attr in sorted(attrs):
        f.write('''\
    {"%(attr)s", T_OBJECT_EX, offsetof(Py%(type)s, %(attr)s), READONLY,
      "%(doc)s"},

''' % {'type':t.__name__, 'attr':attr, 'doc':attrs[attr].replace('\n', '\\n')})

    f.write('''\
    {NULL} // Sentinel
};

''')

def write_type_attrs(f, t, funcs, attrs):
    pass

def write_type_object(f, skel, t, funcs, attrs):
    '''Write the PyTypeObject for a type to a file.'''
    f.write('''\
// Py%(type)s type definition
static PyTypeObject Py%(type)sType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "_%(module)s.%(type)s",    /*tp_name*/
    sizeof(Py%(type)s),        /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Py%(type)s_dealloc,/*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "%(doc)s",                 /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    Py%(type)s_methods,        /* tp_methods */
    Py%(type)s_members,        /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)Py%(type)s_init, /* tp_init */
    0,                         /* tp_alloc */
    (newfunc)Py%(type)s_new,   /* tp_new */
};

''' % {'type':t.__name__, 'module':skel.name,
       'doc':t.__doc__.replace('\n', '\\n')})

def generate_wrapper_form(skel):
    '''
    Generate a C++ source file wrapping the given skeleton definition.
    File is output to [skel.dirpath]/_[skel.name]module.cpp
    '''

    f = file(skel.wrapper_name(), 'w')
    f.write('\n')
    write_warning(f)
    write_copyright(f)
    write_wrapper_includes(f, skel)
    write_wrapper_methods(f, skel)
    write_wrapper_init(f, skel)

def write_wrapper_includes(f, skel):
    '''Write C++ wrapper includes for a Skeleton definition to a file.'''
    f.write('#include "_%(module)smodule.h"\n' % {'module':skel.name})
    f.write('\n\n')

def write_wrapper_methods(f, skel):
    for t in skel.types:
        funcs = dict((attr, val) for attr, val in t.__dict__.iteritems()
                    if not attr.startswith('__') and
                        type(val) is types.FunctionType)
        attrs = [attr for attr, val in t.__dict__.iteritems()
                    if not attr.startswith('__') and
                        attr not in funcs]
        write_wrapper_type_methods(f, skel, t, funcs, attrs)

def write_wrapper_type_methods(f, skel, t, funcs, attrs):
    f.write('''\
/**
Allocate a new Py%(type)s object.
**/
PyObject* Py%(type)s_new (PyTypeObject* type, PyObject* args,
    PyObject* kwds)
{
    PyObject* self = type->tp_alloc(type, 0);
    Py%(type)s* %(lower)s = reinterpret_cast<Py%(type)s*>(self);

    if (self != NULL) {
        %(lower)s->_%(lower)s = new %(type)s();
        if (%(lower)s->_%(lower)s == NULL) {
            Py_DECREF(self);
            PyErr_SetFromErrno(PyExc_SystemError);
            return NULL;
        }
''' % {'type':t.__name__, 'lower':t.__name__.lower()})

    for attr in attrs:
        f.write('        %(lower)s->%(attr)s = NULL;\n' %
                {'lower':t.__name__.lower(), 'attr':attr})

    f.write('''\
    }

    return self;
}

/**
Initialize a Py%(type)s object.
**/
int Py%(type)s_init (PyObject* self, PyObject* args,
    PyObject* kwds)
{
    Py%(type)s* %(lower)s = reinterpret_cast<Py%(type)s*>(self);
    return 0;
}

/**
Deallocate memory for a deleted Py%(type)s object.
**/
void Py%(type)s_dealloc (PyObject* self)
{
    Py%(type)s* %(lower)s = reinterpret_cast<Py%(type)s*>(self);
    delete %(lower)s->_%(lower)s;
    self->ob_type->tp_free(self);
}

/**
Create a new Py%(type)s object from a C++ %(type)s object, from C++.
**/
PyObject* Py%(type)s_new (%(type)s* _%(lower)s)
{
    PyObject* self = Py%(type)sType.tp_alloc(&Py%(type)sType, 0);
    Py%(type)s* %(lower)s = reinterpret_cast<Py%(type)s*>(self);

    if (self != NULL) {
        %(lower)s->_%(lower)s = _%(lower)s;
''' % {'type':t.__name__, 'lower':t.__name__.lower()})

    for attr in attrs:
        f.write('        %(lower)s->%(attr)s = NULL;\n' %
                {'lower':t.__name__.lower(), 'attr':attr})

    f.write('''\
    }

    return self;
}

''')

    for func in sorted(funcs.keys()):
        f.write('''\
/**
%(doc)s
**/
PyObject* Py%(type)s_%(func)s (PyObject* self, PyObject* args)
{
    Py%(type)s* %(lower)s = reinterpret_cast<Py%(type)s*>(self);
    Py_BEGIN_ALLOW_THREADS;

    Py_END_ALLOW_THREADS;
    Py_RETURN_NONE;
}

''' % {'type':t.__name__, 'func':func, 'lower':t.__name__.lower(),
       'doc':pydoc.getdoc(funcs[func])})

def write_wrapper_init(f, skel):
    '''Write the module initialization code for a skeleton to a file.'''
    write_wrapper_init_methoddef(f, skel)
    write_wrapper_init_methods(f, skel)

def write_wrapper_init_methoddef(f, skel):
    '''Write the PyMethodDef for the methods in a skeleton to a file.'''
    f.write('''\
/**
Module-level methods for _%(skel)s
**/
static PyMethodDef _%(skel)s_methods[] = {

    {NULL} // Sentinel
};

''' % {'skel':skel.name})

def write_wrapper_init_methods(f, skel):
    '''
    Write the module initialization methods for Python dynamic loading
    and for C++ bakend initialization.
    '''
    f.write('''\
/**
Initialize the _%(skel)s module as a dynamic library.
**/
PyMODINIT_FUNC init_%(skel)s (void)
{
    // Initialize the type objects
''' % {'skel':skel.name})

    for t in skel.types:
        f.write('''\
    if (PyType_Ready(&Py%(type)sType) < 0)
        return; // error handled by Python
''' % {'type':t.__name__})

    f.write('''\

    // Initialize module
    PyObject *module = Py_InitModule3("_%(skel)s",
        _%(skel)s_methods,
        "Python wrapped access to %(skel)s");
    if (module == NULL)
        return; // error handled by Python

    // Add type object references to the module
''' % {'skel':skel.name})

    for t in skel.types:
        f.write('''\
    Py_INCREF(&Py%(type)sType);
    PyModule_AddObject(module, "%(type)s",
        reinterpret_cast<PyObject*>(&Py%(type)sType));
''' % {'type':t.__name__})

    f.write('''\

}

/**
Initialize the _%(skel)s module from C++ as a backend extension.
**/
int c_init_%(skel)s (void)
{
    // Initialize the type objects
''' % {'skel':skel.name})

    for t in skel.types:
        f.write('''\
    if (PyType_Ready(&Py%(type)sType) < 0)
        return -1;
''' % {'type':t.__name__})

    f.write('''\

    // Initialize module
    PyObject *module = Py_InitModule3("_%(skel)s",
        _%(skel)s_methods,
        "Python wrapped access to %(skel)s");
    if (module == NULL)
        return -1;

    // Add type object references to the module
''' % {'skel':skel.name})

    for t in skel.types:
        f.write('''\
    Py_INCREF(&Py%(type)sType);
    PyModule_AddObject(module, "%(type)s",
        reinterpret_cast<PyObject*>(&Py%(type)sType));
''' % {'type':t.__name__})

    f.write('''\

    return 0;
}


''')


def generate_module_form(skel):
    '''
    Generate a Python module file wrapping the given skeleton definition.
    File is output to [skel.dirpath]/[skel.name].py
    '''
    f = file(skel.module_name(), 'w')
    f.write('\n')
    write_python_warning(f)
    write_python_copyright(f)
    write_imports(f, skel)

def write_python_warning(f):
    '''Write a warning to a file indicating it was generated automatically'''
    f.write('''\
##  WARNING, this file was automatically generated using the Northern
##  Bites' Python wrapper extension module generator.  Subsequent
##  form-completion was done by hand.


''')

def write_python_copyright(f):
    '''Write a GPLv3 copyright notice to a file.'''
    f.write('''\
##  This program is free software: you can redistribute it and/or modify
##  it under the terms of the GNU General Public License as published by
##  the Free Software Foundation, either version 3 of the License, or
##  (at your option) any later version.
##
##  This program is distributed in the hope that it will be useful,
##  but WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##  GNU General Public License for more details.
##
##  You should have received a copy of the GNU General Public License
##   along with this program.  If not, see <http://www.gnu.org/licenses/>.

''')

def write_imports(f, skel):
    '''Write the import statements for the types defined in the backend.'''

    # build a list of all the module attributes for import
    attrs = []
    for t in sorted(skel.types):
        attrs.append(t.__name__)
    attrs.extend(sorted(skel.instances.keys()))
    attrs.extend(sorted(skel.constants.keys()))

    f.write('\nfrom ._%(module)s import (' % {'module':skel.name})

    assert len(attrs) > 0, 'module needs at least one attribute'
    f.write(attrs[0] + ',\n')  # no indent

    if len(attrs) > 1:
        # indent remaining list entries 16 + len(skel.name) spaces
        indent = ' ' * (16 + len(skel.name))
        for i, attr in enumerate(attrs[1:]):
            f.write(indent + attr + ',\n')
        f.write(indent)

    f.write(')\n\n')


if __name__ == '__main__':
    import sys
    if len(sys.argv) != 2:
        print >>sys.stderr, "Usage:  %s MODULE_FILE" % sys.argv[0]
        sys.exit(1)
    wrap_module(sys.argv[1])
