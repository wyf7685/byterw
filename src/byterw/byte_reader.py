from collections.abc import Buffer
from datetime import datetime
from pathlib import Path
from typing import Any, Callable, Dict, List, Optional, Set, Tuple, TypeVar

from .common import VT, ValidType
from .crypt import decrypt
from .mv2value import MemoryView2Value, mv2vt

_T = TypeVar("_T")

class ByteReader(object):
    __buffer: memoryview

    def __init__(self, buffer: Buffer, key: str | int | None = None) -> None:
        self.__buffer = memoryview(decrypt(buffer, key=key)).toreadonly()

    def any(self):
        return len(self.__buffer) != 0

    def _read_with(self, call: Callable[[memoryview], Tuple[_T, memoryview]]) -> _T:
        value, buffer = call(self.__buffer)
        self.__buffer.release()
        self.__buffer = buffer
        return value

    def _read(self, vt: VT) -> Any:
        if not self.any():
            raise ValueError("没有可供读取的内容")

        bvt = self._read_with(mv2vt)
        if vt != bvt:
            raise TypeError(f"预期读取 {vt!r}, 获取到 {bvt!r}")

        return self._read_with(MemoryView2Value[vt])

    def read_int(self) -> int:
        return self._read(VT.Int)

    def read_float(self) -> float:
        return self._read(VT.Float)

    def read_bool(self) -> bool:
        return self._read(VT.Bool)

    def read_string(self) -> str:
        return self._read(VT.Str)

    def read_bytes(self) -> bytes:
        return self._read(VT.Bytes)

    def read_dict(self) -> Dict[Any, Any]:
        return self._read(VT.Dict)

    def read_list(self) -> List[Any]:
        return self._read(VT.List)

    def read_set(self) -> Set[Any]:
        return self._read(VT.Set)

    def read_datetime(self) -> datetime:
        return self._read(VT.Datetime)

    def read_path(self) -> Path:
        return self._read(VT.Path)

    def read_model(self, __T: Optional[type[_T]] = None) -> _T:
        return self._read(VT.Model)

    def read(self) -> ValidType:
        if not self.any():
            raise ValueError("没有可供读取的内容")

        vt = self._read_with(mv2vt)
        return self._read_with(MemoryView2Value[vt])
