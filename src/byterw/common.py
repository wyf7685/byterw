from datetime import datetime
from enum import IntEnum
from pathlib import Path
from typing import Any, Dict, List, Set, Union, TypeAlias

from pydantic import BaseModel

ValidType: TypeAlias = Union[
    None,
    int,
    float,
    bool,
    str,
    bytes,
    str,
    Dict[Any, Any],
    List[Any],
    Set[Any],
    datetime,
    Path,
    BaseModel,
]


class VT(IntEnum):
    Null = 0
    Int = 1
    Float = 2
    Bool = 3
    Str = 4
    Bytes = 5
    Dict = 6
    List = 7
    Set = 8
    Datetime = 9
    Path = 10
    Model = 11
