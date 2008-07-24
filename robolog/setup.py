
from distutils.core import Extension, setup
import platform

# Default options
INCLUDE_DIRS = ['../../../trunk/dog/cortex', '../../../trunk/dog/Vision']
LIBRARY_DIRS = []
LIBRARIES = ['jpeg']
EXTRA_LINK_ARGS = []
SOURCES = [
        'crobolog.i',
        'Logging.cc',
        'JPEG.cc',
        ]

# Windows-specific options
if platform.uname()[0] == 'Windows':
    INCLUDE_DIRS.append('include.win')
    del LIBRARIES[0]
    SOURCES[0] = 'crobolog_wrap.cpp'
    EXTRA_LINK_ARGS.append('lib.win/libjpeg.dll')
# Mac-specific options
elif platform.uname()[0] == 'Darwin':
    INCLUDE_DIRS.append('/sw/include')
    LIBRARY_DIRS.append('/sw/lib')

module1 = Extension(
    '_crobolog',
    sources = SOURCES,
    include_dirs = INCLUDE_DIRS,
    library_dirs = LIBRARY_DIRS,
    libraries = LIBRARIES,
    extra_link_args = EXTRA_LINK_ARGS,
    language = 'c++',
    define_macros = [
        ('OFFLINE',1),
        ],
    )

setup(
    name = 'robolog',
    py_modules = ['robolog', 'crobolog'],
    ext_modules = [module1],
    )
