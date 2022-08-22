import os
import platform
import subprocess
import sys
from pprint import pprint
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext

c_module_name = '_rtklib'

cmake_cmd_args = []
for f in sys.argv:
    if f.startswith('-D'):
        cmake_cmd_args.append(f)

for f in cmake_cmd_args:
    sys.argv.remove(f)


def _get_env_variable(name, default='OFF'):
    if name not in os.environ.keys():
        return default
    return os.environ[name]


class CMakeExtension(Extension):
    def __init__(self, name, cmake_lists_dir='.', sources=[], **kwargs):
        Extension.__init__(self, name, sources=sources, **kwargs)
        self.cmake_lists_dir = os.path.abspath(cmake_lists_dir)


class CMakeBuild(build_ext):
    def build_extensions(self):
        try:
            out = subprocess.check_output(['cmake', '--version'])
            pprint(out)
        except OSError:
            raise RuntimeError('Cannot find CMake executable')

        for ext in self.extensions:

            extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
            pprint("extdir: " + extdir)
            cfg = 'Debug' if _get_env_variable('RTKLIB_DEBUG') == 'ON' else 'Release'

            cmake_args = [
                '-DCMAKE_BUILD_TYPE=%s' % cfg,
                '-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{}={}'.format(cfg.upper(), extdir),
                '-DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_{}={}'.format(cfg.upper(), self.build_temp),
                '-DPYTHON_EXECUTABLE={}'.format(sys.executable)
            ]

            if platform.system() == 'Windows':
                plat = ('x64' if platform.architecture()[0] == '64bit' else 'Win32')
                cmake_args += [
                    '-DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=TRUE',
                    '-DCMAKE_RUNTIME_OUTPUT_DIRECTORY_{}={}'.format(cfg.upper(), extdir),
                ]
                if self.compiler.compiler_type == 'msvc':
                    cmake_args += [
                        '-DCMAKE_GENERATOR_PLATFORM=%s' % plat,
                    ]
                else:
                    cmake_args += [
                        '-G', 'MinGW Makefiles',
                    ]

            cmake_args += cmake_cmd_args

            pprint(cmake_args)

            if not os.path.exists(self.build_temp):
                os.makedirs(self.build_temp)

            subprocess.check_call(['cmake', ext.cmake_lists_dir] + cmake_args,
                                  cwd=self.build_temp)
            subprocess.check_call(['cmake', '--build', '.', '--config', cfg],
                                  cwd=self.build_temp)


version = '0.1.0'

setup(name='rtklib',
      packages=[],
      version=version,
      description='RTKLIB Python bindings',
      author='seagull@3secondz.com',
      author_email='seagull@3secondz.com',
      ext_modules=[CMakeExtension(c_module_name)],
      cmdclass={'build_ext': CMakeBuild},
      zip_safe=False
      )
