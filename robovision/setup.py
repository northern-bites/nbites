
from distutils.core import Extension, setup
import platform

# Default options
AIBO_INCLUDE_DIRS = [
        '.',
        'dog/common/include',
        'dog/Vision',
        ]
NAO_INCLUDE_DIRS = [
        '.',
        'nao',
        'nao_robocup/include',
        'dog/common/include',
        'dog/Vision',
        'nao/man',
        ]
LIBRARY_DIRS = []
AIBO_SOURCES = [
        'AiboVision.i',
        'dog/cortex/Vision.cc',
        'dog/Vision/Ball.cc',
	'dog/Vision/FieldLines.cc',
	'dog/Vision/FieldObjects.cc',
	'dog/Vision/ObjectFragments.cc',
	'dog/Vision/Pose.cc',
	'dog/Vision/PoseMatrix.cc',
	'dog/Vision/Threshold.cc',
        ]
NAO_SOURCES = [
        'nao/PyVision.cpp',
        'nao/Vision.cpp',
        'dog/Vision/Ball.cc',
	'dog/Vision/FieldLines.cc',
	'dog/Vision/FieldObjects.cc',
	'dog/Vision/ObjectFragments.cc',
	'dog/Vision/Pose.cc',
	'dog/Vision/PoseMatrix.cc',
	'dog/Vision/Threshold.cc',
        ]

AIBO_DEFINES = [
    ('OFFLINE', 1),
    ('NO_ZLIB', 1),
    ('ROBOT_TYPE', 'AIBO_ERS7'),
    ]

NAO_DEFINES = [
    ('OFFLINE', 1),
    ('NO_ZLIB', 1),
    ('ROBOT_TYPE', 'NAO_RL'),
    ]

EXTRA_COMPILE_ARGS = [
    '-Wno-write-strings',
    ]

# Windows-specific options
if platform.uname()[0] == 'Windows':
    AIBO_INCLUDE_DIRS.append('include.win')
    NAO_INCLUDE_DIRS.append('include.win')
    AIBO_SOURCES[0] = 'AiboVision_wrap.cpp'


AiboModule = Extension(
    'aibovision',
    sources = AIBO_SOURCES,
    include_dirs = AIBO_INCLUDE_DIRS,
    language = 'c++',
    define_macros = AIBO_DEFINES,
    extra_compile_args = EXTRA_COMPILE_ARGS,
    )

NaoModule = Extension(
    'naovision',
    sources = NAO_SOURCES,
    include_dirs = NAO_INCLUDE_DIRS,
    language = 'c++',
    define_macros = NAO_DEFINES,
    extra_compile_args = EXTRA_COMPILE_ARGS,
    )

setup(
    name = 'robovision',
    py_modules = [
        'robovision',
        #'aibovision',
        #'naovision',
        ],
    ext_modules = [
        #AiboModule,
        NaoModule,
        ],
    )
