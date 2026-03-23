import re
import os
import platform
from pathlib import Path

import setuptools
from setuptools.command.build_ext import build_ext


def is_windows():
    return platform.system() == "Windows"


def is_macos():
    return platform.system() == "Darwin"


def is_linux():
    return platform.system() == "Linux"


def get_package_version() -> str:
    with open("CMakeLists.txt", "r") as f:
        content = f.read()
        
    match = re.search(r"set\(PYBIND11_EXAMPLE_VERSION (.*)\)", content)
    latest_version = match.group(1).strip('"')
    
    return latest_version


def cmake_extension(name: str, *args, **kwargs) -> setuptools.Extension:
    kwargs["language"] = "c++"
    sources = []
    return setuptools.Extension(name, sources, *args, **kwargs)


class BuildExtension(build_ext):
    def build_extension(self, ext):
        # build/temp.linux-x86_64-cpython-<python_version>
        # such as: build/lib.linux-x86_64-cpython-313
        print(f"build/temp: {self.build_temp}")
        os.makedirs(self.build_temp, exist_ok=True)
        
        # build/lib.linux-x86_64-cpython-<python_version>
        # such as: build/lib.linux-x86_64-cpython-313
        print(f"build/lib: {self.build_lib}")
        os.makedirs(self.build_lib, exist_ok=True)
        
        install_dir = Path(self.build_lib).resolve() / "pymodule"
        project_dir = Path(__file__).parent.resolve()
        
        cmake_args = " -DENABLE_PYTHON=ON"
        cmake_args += f" -DCMAKE_INSTALL_PREFIX={install_dir}"
        
        if is_windows() or is_macos():
            raise NotImplementedError("Windows and Macos is currently not supported")
        
        build_cmd = f"""
            cd {self.build_temp}
            cmake {cmake_args} {project_dir}
            cmake --build .
            cmake --install .
        """
        
        print(f"build command:\n{build_cmd}")
        ret = os.system(build_cmd)
        if ret != 0:
            raise Exception("<pip install .> build failed")
        

setuptools.setup(
    version=get_package_version(),
    ext_modules=[cmake_extension("_pymodule")],
    cmdclass={"build_ext": BuildExtension}
)