# byterw

![build](https://github.com/wyf7685/byterw/actions/workflows/build.yml/badge.svg)
[![codecov](https://codecov.io/gh/wyf7685/byterw/graph/badge.svg?token=3PGRN98Y25)](https://codecov.io/gh/wyf7685/byterw)
[![license](https://img.shields.io/github/license/wyf7685/byterw.svg)](https://github.com/wyf7685/byterw/blob/main/LICENSE)

[`English`](https://github.com/wyf7685/byterw/blob/main/README.md) | [`简体中文`](https://github.com/wyf7685/byterw/blob/main/README.zh.md)

## Introduction

[wyf7685](https://github.com/wyf7685)'s _personal_ encoding library.

Encodes some common Python objects into bytes.

~~The code is messy, not recommended for downloading and usage, for entertainment purposes only~~

## Installation

As this project is mainly for _personal_ use, it has not been published to PyPI.

You can install the module byterw in the following ways:

- Install from precompiled wheel package

1. Download the precompiled wheel package for your system from Release or Actions.

2. Execute the pip command to install the wheel package

```sh
pip install byterw-*.whl
```

- Install from source code compilation

> Note: This method requires configuring a C++ compiler on your system

```sh
pip install git+https://github.com/wyf7685/byterw.git@main
```

## Usage

The `byterw` module provides two main types: `ByteReader` and `ByteWriter`.

Supports reading and writing of a total of 13 types (including nested types): `None`, `int`, `float`, `bool`, `str`, `bytes`, `dict`, `list`, `set`, `tuple`, `datetime`, `pathlib.Path`, `pydantic.BaseModel`

Examples:

```python
from datetime import datetime
from pathlib import Path
from pydantic import BaseModel

from byterw import ByteReader, ByteWriter

# KEY for simple encryption
KEY = "SOME_KEY"

# Define a model for testing
class InnerModel(BaseModel):
    mInt: int
    mBool: bool
    mStr: str

class Model(BaseModel):
    mModel: InnerModel
    mDict: dict
    mList: list
    mSet: set
    mDatetime: datetime
    mPath: Path
```

Example of writing:

```python
# Create a ByteWriter using KEY
writer = ByteWriter(key=KEY)

writer.write_int(114514)
writer.write_float(1 / 3, 6)  # write 1/3 with precision=6
writer.write_bool(True)
writer.write_string("Python")
writer.write_bytes(b"byterw")
writer.write_dict({"key": "value"})
writer.write_list([114514, 1919810, False, "wyf7685"])
writer.write_set(set(range(5)))
writer.write_tuple(tuple("byterw"))
writer.write_datetime(datetime(2024, 1, 1))
writer.write_path(Path("somefile.txt"))

writer.write_model(
    Model(
        mModel=InnerModel(
            mInt=114514,
            mBool=True,
            mStr="byterw",
        ),
        mDict={"key": "value"},
        mList=[11, 22, 33],
        mSet={4, 5, 6},
        mDatetime=datetime(2024, 1, 1),
        mPath=Path("somefile.txt"),
    )
)

# Encode all written data and output
data = writer.get()
```

Example of reading:

```python
# Create ByteReader using KEY and decode data
reader = ByteReader(data, KEY)

print(reader.read_int())  # 114514
print(reader.read_float())  # 0.333333
print(reader.read_bool())  # True
print(reader.read_string())  # Python
print(reader.read_bytes())  # b'byterw'
print(reader.read_dict())  # {'key': 'value'}
print(reader.read_list())  # [114514, 1919810, False, 'wyf7685']
print(reader.read_set())  # {0, 1, 2, 3, 4}
print(reader.read_tuple())  # ('b', 'y', 't', 'e', 'r', 'w')
print(reader.read_datetime())  # datetime.datetime(2024, 1, 1, 0, 0)
print(reader.read_path())  # PosixPath('somefile.txt')
print(reader.read_model())  # ...
print(reader.read_int())  # ValueError: No more data to read
```

If you can determine the data type being accessed, you can also use a shorter syntax:

```python
writer = ByteWriter(KEY)
for i in range(10):
    writer.write(i)
data = writer.get()

reader = ByteReader(data, KEY)
while reader.any():
    print(reader.read(), end=' ')  # 0 1 2 ...
```

For `ByteWriter`, you can also achieve chain operations as follows:

```python
Path("somedata").write_bytes(
    ByteWriter().write("value").write(114514).get()
)
```

## Building

If you need to build `byterw` yourself, you can follow these steps

> Note: As the project uses C++ to implement some logic, building the module requires installing a C++ compiler locally

1. Install Python and configure environment variables

2. Install package manager `pdm`

```sh
pip install -U pdm
```

3. Clone this repository from Github

```sh
git clone https://github.com/wyf7685/byterw.git
cd byterw
```

4. Build the wheel package

```sh
pdm build
```

After a while, the wheel package corresponding to the platform will be generated in the `dist` directory

## Contributors

![Contributors](https://contrib.rocks/image?repo=wyf7685/byterw)
