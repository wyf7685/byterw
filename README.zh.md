# byterw

![build](https://github.com/wyf7685/byterw/actions/workflows/build.yml/badge.svg)
[![codecov](https://codecov.io/gh/wyf7685/byterw/graph/badge.svg?token=3PGRN98Y25)](https://codecov.io/gh/wyf7685/byterw)
[![license](https://img.shields.io/github/license/wyf7685/byterw.svg)](https://github.com/wyf7685/byterw/blob/main/LICENSE)

[`English`](https://github.com/wyf7685/byterw/blob/main/README.md) | [`简体中文`](https://github.com/wyf7685/byterw/blob/main/README.zh.md)

## 简介

[wyf7685](https://github.com/wyf7685) _自用_ 编码库

将一些常用 Python 对象编码为 `bytes`

~~代码乱七八糟，不建议下载使用，仅供娱乐~~

## 安装

由于本项目主要为 _自用_ , 故未发布至 `PyPI`.

可通过以下方式安装模块 `byterw`

- 从预编译的 wheel 包安装

  1. 从 [`Release`](https://github.com/wyf7685/byterw/releases) 或 [`Actions`](https://github.com/wyf7685/byterw/actions) 下载环境对应系统的预编译 wheel 包。

  2. 执行 `pip` 命令安装 wheel 包

```sh
pip install byterw-*.whl
```

- 从源码编译安装

> 注意: 该方式需要在你的系统上配置 C++ 编译器

```sh
pip install git+https://github.com/wyf7685/byterw.git@main
```

## 使用

`byterw` 模块提供两个主要类型: `ByteReader` 和 `ByteWriter`

支持共计 13 种类型(及嵌套)的读写: `None`, `int`, `float`, `bool`, `str`, `bytes`, `dict`, `list`, `set`, `tuple`, `datetime`, `pathlib.Path`, `pydantic.BaseModel`

使用示例：

```python
from datetime import datetime
from pathlib import Path
from pydantic import BaseModel

from byterw import ByteReader, ByteWriter

# 用于简单加密的KEY
KEY = "SOME_KEY"

# 定义用于测试的 Model
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

写入示例

```python
# 使用 KEY 创建一个 ByteWriter
writer = ByteWriter(key=KEY)

writer.write_int(114514)
writer.write_float(1 / 3, 6)  # 写入 1/3 并保留 6 位小数
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

# 将写入的所有数据编码输出
data = writer.get()
```

读取示例

```python
# 使用 KEY 创建 ByteReader 并解码数据
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
print(reader.read_int())  # ValueError: 没有可供读取的内容
```

如果可以确定正在存取的数据类型, 也可以使用更简短的写法:

```python
writer = ByteWriter(KEY)
for i in range(10):
    writer.write(i)
data = writer.get()

reader = ByteReader(data, KEY)
while reader.any():
    print(reader.read(), end=' ')  # 0 1 2 ...
```

对于 `ByteWriter`, 还可以使用如下所示的链式操作:

```python
Path("somedata").write_bytes(
    ByteWriter().write("value").write(114514).get()
)
```

## 构建

如果需要自行构建 `byterw`, 可以参考如下步骤

> 注: 由于项目使用 C++ 实现部分逻辑, 构建模块需要在本地安装 C++ 编译器

1. 安装 `Python` 并配置环境变量

2. 安装 包管理工具 `pdm`

```sh
pip install -U pdm
```

3. 从 Github 克隆此仓库

```sh
git clone https://github.com/wyf7685/byterw.git
cd byterw
```

4. 构建 wheel 包

```sh
pdm build
```

等待片刻后, `dist` 目录下生成平台对应的 wheel 包

## 贡献者

![Contributors](https://contrib.rocks/image?repo=wyf7685/byterw)
