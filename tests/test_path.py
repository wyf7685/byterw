from .dataset import VT, get_test_case

from byterw import ByteReader, ByteWriter


def test_path():
    test_case = get_test_case(VT.Path)
    key = "7685_TEST_PATH"

    writer = ByteWriter(key)
    for i in test_case:
        writer.write_path(i)
    data = writer.get()

    reader = ByteReader(data, key)
    while reader.any():
        assert reader.read_path() == test_case.pop(0)

    assert not test_case
