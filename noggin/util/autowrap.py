
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
    generate_header(skel)
    generate_wrapper(skel)
    generate_form(skel)

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

    type_dict = dict((name, o) for name, o in objs.iteritems()
                            if type(o) is types.TypeType)
    skel.types = type_dict.keys()
    rtype_dict = dict((val, key) for key, val in type_dict.iteritems())

    type_objs = type_dict.values()
    instances = dict((name, rtype_dict[type(o)])
                        for name, o in objs.iteritems()
                         if name not in type_dict and type(o) in type_objs)
    skel.instances = instances

    constants = dict((name, type(o))
                        for name, o in objs.iteritems()
                         if name not in type_dict and name not in instances)
    skel.constants = constants

    return skel


def generate_header(skel):
    '''
    Generate a C++ header file wrapping the given skeleton definition.
    File is output to [skel.dirpath]/_[skel.name]module.h
    '''
    f = file(skel.header_name(), 'w')
    write_warning(f)
    write_copyright(f)
    write_types(f, skel)

def write_warning(f):
    '''Write a warning to a file indicating it was generated automatically'''

def write_copyright(f):
    '''Write a GPLv2 copyright notice to a file.'''

def write_types(f, skel):
    '''
    Write documentation and definitions for the types in a Skeleton to
    a file.
    '''

def generate_wrapper(skel):
    '''
    Generate a C++ source file wrapping the given skeleton definition.
    File is output to [skel.dirpath]/_[skel.name]module_autogen.cpp
    '''

def generate_form(skel):
    '''
    Generate a C++ source file form for human hands to fill method
    implementations, based on the given skeleton definition.
    File is output to [skel.dirpath]/_[skel.name]module.cpp
    '''


if __name__ == '__main__':
    import sys
    if len(sys.argv) != 2:
        print >>sys.stderr, "Usage:  %s MODULE_FILE" % sys.argv[0]
        sys.exit(1)
    wrap_module(sys.argv[1])
