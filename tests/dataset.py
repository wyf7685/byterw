from copy import deepcopy
from datetime import datetime
from pathlib import Path
from random import choice, randbytes, randint, uniform, sample

from pydantic import BaseModel

from byterw.common import VT

# CASE_SIZE = 1000
# INT_RANGE = 1 << 15
# STR_LEN_RANGE = 100, 600
# SAMPLE_K = 10

CASE_SIZE = 2
INT_RANGE = 16
STR_LEN_RANGE = 2, 5
SAMPLE_K = 1

TEST_CASE = {
    VT.Int: [randint(0, INT_RANGE) for _ in range(CASE_SIZE)],
    VT.Float: [uniform(0, INT_RANGE) for _ in range(CASE_SIZE)],
    VT.Bool: [bool(randint(0, 1)) for _ in range(CASE_SIZE)],
    VT.Str: [
        "".join(chr(randint(0, 255)) for _ in range(randint(*STR_LEN_RANGE)))
        for _ in range(CASE_SIZE)
    ],
    VT.Bytes: [randbytes(randint(*STR_LEN_RANGE)) for _ in range(CASE_SIZE)],
}

TEST_CASE[VT.Dict] = [
    {idx: val for idx, val in enumerate(sample(TEST_CASE[VT.Int], SAMPLE_K))}
    for _ in range(CASE_SIZE)
]

TEST_CASE[VT.List] = [
    [val for val in sample(TEST_CASE[VT.Int], SAMPLE_K)]
    + [val for val in sample(TEST_CASE[VT.Str], SAMPLE_K)]
    for _ in range(CASE_SIZE)
]

TEST_CASE[VT.Set] = [
    set(
        [val for val in sample(TEST_CASE[VT.Int], SAMPLE_K)]
        + [val for val in sample(TEST_CASE[VT.Str], SAMPLE_K)]
    )
    for _ in range(CASE_SIZE)
]

TEST_CASE[VT.Datetime] = [datetime.now() for _ in range(CASE_SIZE)]

TEST_CASE[VT.Path] = [Path(choice(TEST_CASE[VT.Str])) for _ in range(CASE_SIZE)]


class ModelA(BaseModel):
    aaa: int
    bbb: bool
    ccc: str
    # ddd: bytes


class ModelB(BaseModel):
    modela: ModelA
    eee: dict
    fff: list
    ggg: set
    hhh: datetime
    iii: Path


TEST_CASE[VT.Model] = [
    ModelB(
        modela=ModelA(
            aaa=choice(TEST_CASE[VT.Int]),
            bbb=choice(TEST_CASE[VT.Bool]),
            ccc=choice(TEST_CASE[VT.Str]),
            # ddd=choice(TEST_CASE[VT.Bytes]),
        ),
        eee=choice(TEST_CASE[VT.Dict]),
        fff=choice(TEST_CASE[VT.List]),
        ggg=choice(TEST_CASE[VT.Set]),
        hhh=choice(TEST_CASE[VT.Datetime]),
        iii=choice(TEST_CASE[VT.Path]),
    )
    for _ in range(CASE_SIZE)
]


def get_test_case(vt: VT) -> list:
    return deepcopy(TEST_CASE[vt])
