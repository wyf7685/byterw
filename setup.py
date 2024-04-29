import platform

from setuptools import Extension, setup

if platform.system() == "Windows":
    cxx_std_arg = "/std:c++20"
else:
    cxx_std_arg = "-std=c++20"

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
        include_dirs=["./C/"],
        extra_compile_args=[cxx_std_arg],
    ),
]

setup(
    name="byterw",
    version="0.2.2",
    description="byterw module by wyf7685",
    author="wyf7685",
    author_email="69091901+wyf7685@users.noreply.github.com",
    ext_modules=extensions,
)
