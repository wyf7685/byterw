from byterw import ByteReader, ByteWriter

from .dataset import VT, get_test_case


def test_datetime():
    test_case = get_test_case(VT.Datetime)
    key = "7685_TEST_DATETIME"

    writer = ByteWriter(key)
    for i in test_case:
        writer.write_datetime(i)
    data = writer.get()

    reader = ByteReader(data, key)
    while reader.any():
        assert reader.read_datetime() == test_case.pop(0)

    assert not test_case
