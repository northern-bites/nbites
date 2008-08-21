
import imp
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
        return os.path.join(self.dirpath, '_' + self.name +
                            'module_autogen.h')

    def form_name(self):
        return os.path.join(self.dirpath, '_' + self.name + 'module.cpp')



def wrap_module(path):
    module = import_module(path)
    skel = process_module(module)
    generate_header_form(skel)
    generate_wrapper_form(skel)

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
                         if o not in type_objs and name not in instances)
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
    write_includes(f, skel)
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

def write_includes(f, skel):
    '''Write C++ header includes for a Skeleton definition to a file.'''
    f.write('''\
#include <Python.h>
#include "%s.h"

''' % skel.name)

def write_types(f, skel):
    '''
    Write documentation and definitions for the types in a Skeleton to
    a file.
    '''
    for t in skel.types:
        write_type(f, t)

def write_type(f, t):
    '''Write documentation and definitions for a type to a file.'''

    funcs = dict((attr, val) for attr, val in t.__dict__.iteritems()
                    if not attr.startswith('__') and
                       type(val) is types.FunctionType)
    attrs = [attr for attr, val in t.__dict__.iteritems()
                    if not attr.startswith('__') and
                       attr not in funcs]
    f.write('''\
//
// Py%s definitions
//

''' % t.__name__)

    write_type_struct(f, t, funcs, attrs)
    write_type_methods(f, t, funcs, attrs)
    write_type_attrs(f, t, funcs, attrs)
    write_type_object(f, t, funcs, attrs)

def write_type_struct(f, t, funcs, attrs):
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
    f.write('}\n\n')

def write_type_methods(f, t, funcs, args):
    f.write('''\
// backend methods
extern PyObject* Py%(type)s_new (PyTypeObject* type, PyObject* args,
    PyObject* kwds);
extern int Py%(type)s_init (PyObject* self, PyObject* arg, PyObject* kwds);
extern void Py%(type)s_dealloc (PyObject* self);
// C++ - accessible interface
extern Py%(type)s *Py%(type)s_new (ARGS);
// Python - accesible interface
''' % {'type':t.__name__})

    for func in sorted(funcs.keys()):
        f.write('extern PyObject* Py%(type)s_%(func)s (PyObject* self, '
                'PyObject* args);\n' % {'type':t.__name__, 'func':func})

    f.write('''\

// backend method list
static PyMethodDef Py%(type)s_methods[] = {

''' % {'type':t.__name__})

    for func in sorted(funcs.keys()):
        f.write('''\
    {"%(func)s", reinterpret_case<PyCFunction>(Py%(type)s_%(func)s),
      METH_VARARGS,
      "%(doc)s"},

''' % {'type':t.__name__, 'func':func,
       'doc':funcs[func].__doc__.replace('\n', '\\n')})

    f.write('''\
    {NULL} // Sentinel
};

''')


def write_type_attrs(f, t, funcs, attrs):
    pass

def write_type_object(f, t, funcs, attrs):
    pass

def generate_wrapper_form(skel):
    '''
    Generate a C++ source file wrapping the given skeleton definition.
    File is output to [skel.dirpath]/_[skel.name]module.cpp
    '''


if __name__ == '__main__':
    import sys
    if len(sys.argv) != 2:
        print >>sys.stderr, "Usage:  %s MODULE_FILE" % sys.argv[0]
        sys.exit(1)
    wrap_module(sys.argv[1])
