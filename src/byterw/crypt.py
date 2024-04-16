import base64
import functools
import random
import string
from collections.abc import Buffer

from . import _crypt


@functools.cache
def get_charset(key: str | int | None = None) -> str:
    if key is None:
        key = 7685
    m = list(string.ascii_uppercase + string.ascii_lowercase + string.digits + "+/")
    random.Random(key).shuffle(m)
    return "".join(m) + "="


def encrypt(data: Buffer, *, key: str | int | None = None) -> bytes:
    return _crypt.encrypt(get_charset(key), base64.b64encode(data))


def decrypt(data: Buffer, *, key: str | int | None = None) -> bytes:
    return base64.b64decode(_crypt.decrypt(get_charset(key), memoryview(data)))
