from byterw import ByteReader, ByteWriter
from typing import cast

from .dataset import VT, Model, get_test_case


def test_model():
    test_case = get_test_case(VT.Model)
    key = "7685_TEST_MODEL"

    writer = ByteWriter(key)
    for i in test_case:
        writer.write_model(i)
    data = writer.get()

    reader = ByteReader(data, key)
    while reader.any():
        m1 = reader.read_model(Model)
        m2 = cast(Model, test_case.pop(0))

        # 嵌套 Model 时, 访问正常, 但验证异常
        assert m1.mModel.model_dump() == m2.mModel.model_dump()
        m1, m2 = m1.model_copy(deep=True), m2.model_copy(deep=True)
        del m1.mModel, m2.mModel
        assert m1.model_dump() == m2.model_dump()

    assert not test_case
