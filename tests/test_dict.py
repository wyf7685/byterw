from byterw import ByteReader, ByteWriter

from .dataset import VT, get_test_case


def test_dict():
    test_case = get_test_case(VT.Dict)
    key = "7685_TEST_DICT"

    writer = ByteWriter(key)
    for i in test_case:
        writer.write_dict(i)
    data = writer.get()

    reader = ByteReader(data, key)
    while reader.any():
        assert reader.read_dict() == test_case.pop(0)

    assert not test_case
