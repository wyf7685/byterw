from byterw import ByteReader, ByteWriter

from .dataset import VT, get_test_case


def test_bool():
    test_case = get_test_case(VT.Bool)
    key = "7685_TEST_BOOL"

    writer = ByteWriter(key)
    for i in test_case:
        writer.write_bool(i)
    data = writer.get()

    reader = ByteReader(data, key)
    while reader.any():
        assert reader.read_bool() == test_case.pop(0)

    assert not test_case
