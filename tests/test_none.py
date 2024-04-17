from byterw import ByteReader, ByteWriter


def test_none():
    key = "7685_TEST_NONE"

    writer = ByteWriter(key)
    for _ in range(100):
        writer.write(None)
    data = writer.get()

    reader = ByteReader(data, key)
    count = 0
    while reader.any():
        assert reader.read() == None
        count += 1

    assert count == 100
