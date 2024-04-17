from .dataset import VT, get_test_case, ModelB

from byterw import ByteReader, ByteWriter


def test_model():
    test_case = get_test_case(VT.Model)
    key = "7685_TEST_MODEL"

    writer = ByteWriter(key)
    for i in test_case:
        writer.write_model(i)
    data = writer.get()

    reader = ByteReader(data, key)
    while reader.any():
        m1 = reader.read_model(ModelB)
        m2 = test_case.pop(0)  # type: ModelB

        # 嵌套 Model 时, 访问正常, 但验证异常
        assert m1.modela.model_dump() == m2.modela.model_dump()
        del m1.modela, m2.modela
        assert m1.model_dump() == m2.model_dump()

    assert not test_case
