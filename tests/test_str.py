from byterw import ByteReader, ByteWriter

from .dataset import VT, get_test_case


def test_str():
    test_case = get_test_case(VT.Str)
    key = "7685_TEST_STRING"

    writer = ByteWriter(key)
    for i in test_case:
        writer.write_string(i)
    data = writer.get()

    reader = ByteReader(data, key)
    while reader.any():
        assert reader.read_string() == test_case.pop(0)

    assert not test_case
