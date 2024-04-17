from byterw import ByteReader, ByteWriter

from .dataset import VT, get_test_case


def test_float():
    test_case = get_test_case(VT.Float)
    key = "7685_TEST_FLOAT"
    precision = 8
    p = 10**-precision

    writer = ByteWriter(key)
    for i in test_case:
        writer.write_float(i, precision)
    data = writer.get()

    reader = ByteReader(data, key)
    while reader.any():
        assert abs(reader.read_float() - test_case.pop(0)) < p

    assert not test_case
