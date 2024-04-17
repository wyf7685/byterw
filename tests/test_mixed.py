from random import choice
from typing import cast

from byterw import ByteReader, ByteWriter

from .dataset import CASE_NUM, TEST_CASE, VT, Model


def test_mixed():
    test_case = [
        [
            choice(TEST_CASE[VT.Int]),
            choice(TEST_CASE[VT.Float]),
            choice(TEST_CASE[VT.Bool]),
            choice(TEST_CASE[VT.Str]),
            choice(TEST_CASE[VT.Bytes]),
            choice(TEST_CASE[VT.Dict]),
            choice(TEST_CASE[VT.List]),
            choice(TEST_CASE[VT.Set]),
            choice(TEST_CASE[VT.Datetime]),
            choice(TEST_CASE[VT.Path]),
            choice(TEST_CASE[VT.Model]),
        ]
        for _ in range(CASE_NUM // 2)
    ]
    key = "7685_TEST_MIXED"

    writer = ByteWriter(key)
    for case in test_case:
        writer.write_list(case)
    data = writer.get()

    reader = ByteReader(data, key)
    while reader.any():
        for i, j in zip(reader.read_list(), test_case.pop(0)):
            if isinstance(j, float):
                assert abs(i - j) < 1e-6
            elif isinstance(j, Model):
                i = cast(Model, i)
                assert i.mModel.model_dump() == j.mModel.model_dump()
                i, j = i.model_copy(deep=True), j.model_copy(deep=True)
                del i.mModel, j.mModel
                assert i.model_dump() == j.model_dump()
            else:
                assert i == j

    assert not test_case
