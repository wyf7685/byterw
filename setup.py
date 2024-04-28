from setuptools import Extension, setup


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
    ),
]

setup(
    name="byterw",
    version="1.0",
    description="byterw module by wyf7685",
    author="wyf7685",
    author_email="69091901+wyf7685@users.noreply.github.com",
    ext_modules=extensions,
)
