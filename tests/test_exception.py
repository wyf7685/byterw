from typing import List, Union
import pytest
from pydantic import BaseModel

from byterw import ByteReader, ByteWriter


def test_not_readable():
    # C/pyreader.cpp:54

    with pytest.raises(ValueError) as exc_info:
        ByteReader(b"").read()
    assert exc_info.type is ValueError

    with pytest.raises(ValueError) as exc_info:
        ByteReader(b"").read_int()
    assert exc_info.type is ValueError


def test_read_err_vt():
    # C/pyreader.cpp:60

    data = ByteWriter().write("123").get()
    with pytest.raises(TypeError) as exc_info:
        ByteReader(data).read_int()
    assert exc_info.type is TypeError


class ModelA(BaseModel):
    mGeneric: list[str]
    m_Generic: List[str]
    mUnionType: int | float
    mUnion: Union[str, int]


def test_special_vt():
    model = ModelA(
        mGeneric=["aaa"],
        m_Generic=["aaa"],
        mUnionType=114514,
        mUnion="byterw",
    )
    data = ByteWriter().write(model).get()
    m = ByteReader(data).read_model(ModelA)
    assert model.model_dump() == m.model_dump()


class ModelB(BaseModel):
    mError: object


def test_error_vt():
    model = ModelB(mError=object())
    with pytest.raises(TypeError) as exc_info:
        ByteWriter().write(model).get()

    assert exc_info.type is TypeError
