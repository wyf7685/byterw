from setuptools import Extension, setup


extension = Extension(
    "byterw._crypt",
    sources=["C/_crypt.c"],
)

setup(
    name="byterw",
    version="1.0",
    description="byterw module by wyf7685",
    author="wyf7685",
    author_email="69091901+wyf7685@users.noreply.github.com",
    ext_modules=[extension],
)
