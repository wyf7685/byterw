from .dataset import VT, get_test_case

from byterw import ByteReader, ByteWriter


def test_set():
    test_case = get_test_case(VT.Set)
    key = "7685_TEST_SET"

    writer = ByteWriter(key)
    for i in test_case:
        writer.write_set(i)
    data = writer.get()

    reader = ByteReader(data, key)
    while reader.any():
        assert reader.read_set() == test_case.pop(0)

    assert not test_case
