from typing import List, Union
import pytest
from pydantic import BaseModel

from byterw import ByteReader, ByteWriter


def test_not_readable():
    with pytest.raises(ValueError):
        ByteReader(b"").read()

    with pytest.raises(ValueError):
        ByteReader(b"").read_int()


def test_read_err_vt():
    data = ByteWriter().write("123").get()
    with pytest.raises(TypeError):
        ByteReader(data).read_int()


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
    mError: tuple


def test_error_vt():
    model = ModelB(mError=tuple("byterw"))
    with pytest.raises(TypeError):
        ByteWriter().write(model).get()
