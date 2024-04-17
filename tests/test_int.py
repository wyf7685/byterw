from byterw import ByteReader, ByteWriter

from .dataset import VT, get_test_case


def test_int():
    test_case = get_test_case(VT.Int)
    key = "7685_TEST_INT"

    writer = ByteWriter(key)
    for i in test_case:
        writer.write_int(i)
    data = writer.get()

    reader = ByteReader(data, key)
    while reader.any():
        assert reader.read_int() == test_case.pop(0)

    assert not test_case
