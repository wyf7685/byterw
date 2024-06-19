import platform
from pathlib import Path
from setuptools import Extension, setup

VERSION = "0.2.3"

if platform.system() == "Windows":
    cxx_std_arg = "/std:c++20"  # MSVC
else:
    cxx_std_arg = "-std=c++20"  # GCC

extensions = [
    Extension(
        "byterw._byterw",
        sources=[
            "C/buffer.cpp",
            "C/module.cpp",
            "C/pyobj.cpp",
            "C/crypt.cpp",
            "C/writer.cpp",
            "C/pywriter.cpp",
            "C/reader.cpp",
            "C/pyreader.cpp",
        ],
        include_dirs=[str(Path(__file__).resolve().parent / "C")],
        define_macros=[("BYTERW_VERSION", f'"{VERSION}"')],
        extra_compile_args=[cxx_std_arg],
        language="c++",
    ),
]

setup(
    name="byterw",
    version=VERSION,
    description="byterw module by wyf7685",
    author="wyf7685",
    author_email="69091901+wyf7685@users.noreply.github.com",
    ext_modules=extensions,
)
