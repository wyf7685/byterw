from byterw import ByteReader, ByteWriter

from .dataset import VT, get_test_case


def test_list():
    test_case = get_test_case(VT.List)
    key = "7685_TEST_LIST"

    writer = ByteWriter(key)
    for i in test_case:
        writer.write_list(i)
    data = writer.get()

    reader = ByteReader(data, key)
    while reader.any():
        assert reader.read_list() == test_case.pop(0)

    assert not test_case
