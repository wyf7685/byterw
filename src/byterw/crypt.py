import base64
import functools
import random
import string
from collections.abc import Buffer

# gcc -o _crypt.cp312-win_amd64.pyd -shared -fPIC _crypt.c -I'C:\Program Files\Python312\include' -L'C:\Program Files\Python312\libs' -lpython312
# gcc -o _crypt.so -shared -fPIC _crypt.c -I/usr/local/include/python3.12
from . import _crypt  # type: ignore


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
    return base64.b64decode(_crypt.decrypt(get_charset(key), data))
