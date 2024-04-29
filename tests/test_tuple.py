from byterw import ByteReader, ByteWriter

from .dataset import VT, get_test_case


def test_tuple():
    test_case = get_test_case(VT.Tuple)
    key = "7685_TEST_TUPLE"

    writer = ByteWriter(key)
    for i in test_case:
        writer.write_tuple(i)
    data = writer.get()

    reader = ByteReader(data, key)
    while reader.any():
        assert reader.read_tuple() == test_case.pop(0)

    assert not test_case
