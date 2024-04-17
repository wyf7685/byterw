from .dataset import VT, get_test_case

from byterw import ByteReader, ByteWriter


def test_bytes():
    test_case = get_test_case(VT.Bytes)
    key = "7685_TEST_BYTES"

    writer = ByteWriter(key)
    for i in test_case:
        writer.write_bytes(i)
    data = writer.get()

    reader = ByteReader(data, key)
    while reader.any():
        assert reader.read_bytes() == test_case.pop(0)

    assert not test_case
