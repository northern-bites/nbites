
'''
08/13/2008 - Disregard the following for now.  It is correct in
    reference to imported attributes, but NOT for imported modules.
    With the change to a new and more robust robot platform, however,
    we will probably not need this at all anymore.

    @jfishman

 ****

 A note from Jeremy...

   The work in this module is to in a way replace the default module
 management for Python.  Normally, as you may know, Python modules are on
 the fence between managed and sloppy, in that the interpreter's sys.modules
 list holds the reference to the last loaded version of a module, so after
 reloads future imports load the latest version of the module.  However,
 when cross-dependencies get involved, it is often tricky to successfully
 reload modified portions of code across modules.

   For example, say Module A imports Module B, and Module B imports Module
 A.  Then when reloading both A and B, we call reload(A), which will import
 the original version of B, followed by the call to reload(B), which will
 import the latest version of A.  Thus, B has a valid reference, but A
 still has a dangling reference to the original, unmodified B.  This can
 result in strange behavior, especially when exceptions thrown in B
 originate from code lines that have been changed, or moved, in the new B,
 thus resulting in confusing traceback logs.

   The goal here is to proide managed module loading and reloading
 facilities, so that when a module and its attributes are reloaded all
 references to them are updated.  The drawback of the current design (which
 I have not confirmed, nor spent much time pondering a solution for) is that
 repeated reloading of modules may build up numerous old references in this
 module's record keeping.  I believe the problem to be minor (very little
 memory wastage will occurs in any normal situation), and that a complex
 solution (better than simply reload(module_helper)) could be found without
 changing the overall structure, but is not worth the time.

 And it seems to be working pretty darn well, if I do say so myself :-)

 Author:
  jfishman@bowdoin.edu

'''


import imp
import sys
import os

# A mapping from module name to the respective module
name_map = {}
# A mapping from module to a set of namespaces
ns_map = {}
# A mapping from module to a set attributes (dependencies)
attr_map = {}

def locate_module(name):
    ''' -> ReloadableModuleReference.  Create a reference to the specified
    module.

    Uses the imp module's utilities to locate and import the module
    specified by name.  Returns a new ReloadableModuleReference object
    constructed on the loaded module and puts the ReloadableModuleReference
    into module_helper's store of loaded modules. '''

    path = None
    raw_module = __import__(name)
    for sub_mod_name in name.split('.')[1:]:
        raw_module = getattr(raw_module, sub_mod_name)
    #module = ReloadableModuleReference(raw_module)
    # we know don't do the Reloadable... stuff, due to the getattr() overhead
    return raw_module


def register_module(name, namespace=None):
    ''' -> ReloadableModuleReference.  Load the given module, storing the
    reference and name in the global dictionaries, or return the pre-loaded
    module.
    '''
    # retrieve module
    module = name_map.setdefault(name, locate_module(name))
    # register namespace
    if namespace is not None:
        ns_map.setdefault(module, list()).append(namespace)
    # since we don't do the Reloadable... stuff, add a reload() function
    set_reload_function(module, name)

    return module


def set_reload_function(module, name):
    ''' -> None.  Define a new function to reload the given module.

    Sets the attribute 'reload' on the given module (object) to a new
    function in this namespace, with no arguments, that calls
    reload_module(name) to reload the module.  Meant to replace the
    ReloadableModuleReference.reload() method.
    '''
    def f_reload():
        reload_module(name)
    module.reload = f_reload


def insert_global(name, alias=None, attrs=None):
    ''' -> ReloadedableModuleReference.  Locates and loads the given module
    into the current global scope.

    Utilises locate_module to load and store the module reference.  Then
    uses insert_namespace() to insert the reference into the globals of the
    calling stack frame, at the given alias.  If no alias is supplied, the
    base name of the module path is utilized.
    '''

    # insert the module reference into the previous frame's globals, if
    # requested
    if alias is None and attrs is None:
        alias = name.split('.')[-1]
    last_frame = sys._getframe(1)
    globals = last_frame.f_globals
    insert_namespace(name, globals, alias=alias, attrs=attrs)


def insert_local(name, alias=None, attrs=None):
    ''' -> ReloadedableModuleReference.  Locates and loads the given module
    into the current local scope.

    Utilises locate_module to load and store the module reference.  Then
    uses insert_namespace() to insert the reference into the locals of the
    calling stack frame, at the given alias.  If no alias is supplied, the
    base name of the module path is utilized. '''

    # insert the module reference into the previous frame's locals, if
    # requested
    if alias is None and attrs is None:
        alias = name.split('.')[-1]
    last_frame = sys._getframe(1)
    locals = last_frame.f_locals
    insert_namespace(module, locals, alias=alias, attrs=attrs)


def insert_namespace(name, namespace, alias=None, attrs=None):
    ''' -> None.  Insert the module, or the given attributes of the module,
    into the given namespace dictionary. '''

    global attr_map

    # locate and create a reference to the module specified
    module = register_module(name, namespace)

    # insert module reference
    if alias is not None:
        namespace[alias] = module

    # determine requested attributes references
    if attrs == '*':
        # find or build '__all__' attribute list
        if hasattr(module, '__all__'):
            # use specified list
            attrs = module.__all__
        else:
            # build the attribute list
            attrs = [attr for attr in dir(module.module)
                            if not attr.startswith('_')]

    # insert references to the requested attributes
    if attrs is None:
        attrs = []
        namespace = None
    attrs = set(attrs)
    for attr in attrs:
        namespace[attr] = getattr(module, attr)

    # store attribute dependencies in the global map
    attr_map[module] = attrs


def reload_module(name, delete_pycs=False, ignore_case=False):
    ''' -> None.  Reload ALL the references for the given module, given the
    name of the module.

    If delete_pycs evaluates as True, will first remove all associated .pyc
    files for the given module.  Case is automatically ignored for the file
    extension, but not for the file name.  Specify ignore_case as True to
    remove all .pyc files whose basenames match alphanumerically in lower
    case. '''

    global name_map, ns_map, attr_map

    module = name_map.get(name)
    if module is None:
        # Load it now, I guess, since it hasn't been loaded already
        module = register_module(name)

    if hasattr(module, '__file__') and delete_pycs:
        fpath = module.__file__
        fname = os.path.splitext(os.path.basename(fpath))[0]
        dirname = os.path.dirname(fpath)
        if not dirname:
            # Locate module's directory
            path = imp.find_module(name)[1]
            dirname = os.path.dirname(path)
            if not dirname:
                dirname = os.path.curdir

        # Delete pyc file/s
        pyc_file = os.path.extsep.join((fname, 'pyc'))
        # List all pyc files in the directory
        if ignore_case:
            # Check the lower case of all filenames against [...].pyc
            file_list = [path for path in os.listdir(dirname)
                                if path.lower() == pyc_file.lower()]
        else:
            # Check the exact case of all filenames against [...], and the
            # lower case of the extension against .pyc
            file_list = [path for path in os.listdir(dirname)
                                if os.path.splitext(path)[0] == fname and
                                   os.path.splitext(path)[1].lower() == '.pyc']
        # Remove matching files
        for path in file_list:
            print "Removing", os.path.join(dirname, path)
            os.remove(os.path.join(dirname, path))

    # Reload the module
    reload(module)
    # Update all attribute dependencies
    namespaces = ns_map.setdefault(module, set())
    attrs = attr_map.setdefault(module, set())
    for namespace in namespaces:
        for attr in attrs:
            namespace[attr] = getattr(module, attr)


class ReloadableModuleReference(object):
    ''' A transparent wrapper-class around an underlying module object.

    The ReloadableModuleReference is intended to supplant the use of the
    raw module and overloads the __getattr__() method to provide direct
    access to the underlying module's data, mathods, and other attributes.

    The only attributes specific to the wrapper class are:
        'module' - a direct reference to the underlying module, in case of
                   need
        'reload' - a useful method to access module_helper.reload_module()
                   transparently.
    '''

    def __init__(self, module):
        self.module = module
        self.__doc__ = module.__doc__
        self.__name__ = module.__name__

    def reload(self, **kwds):
        reload_module(self.module.__name__, **kwds)

    def __getattr__(self, name):
        return getattr(self.module, name)


