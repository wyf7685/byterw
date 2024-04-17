from copy import deepcopy
from datetime import datetime
from pathlib import Path
from random import choice, randbytes, randint, uniform, sample

from pydantic import BaseModel

from byterw.common import VT

CASE_NUM = 1000
INT_RANGE = 1 << 15
STR_LEN_RANGE = 100, 600
SAMPLE_K = 10


TEST_CASE = {
    VT.Int: [randint(0, INT_RANGE) for _ in range(CASE_NUM)],
    VT.Float: [uniform(0, INT_RANGE) for _ in range(CASE_NUM)],
    VT.Bool: [bool(randint(0, 1)) for _ in range(CASE_NUM)],
    VT.Str: [
        "".join(chr(randint(0, 255)) for _ in range(randint(*STR_LEN_RANGE)))
        for _ in range(CASE_NUM)
    ],
    VT.Bytes: [randbytes(randint(*STR_LEN_RANGE)) for _ in range(CASE_NUM)],
}

TEST_CASE[VT.Dict] = [
    {idx: val for idx, val in enumerate(sample(TEST_CASE[VT.Int], SAMPLE_K))}
    for _ in range(CASE_NUM)
]

TEST_CASE[VT.List] = [
    [val for val in sample(TEST_CASE[VT.Int], SAMPLE_K)]
    + [val for val in sample(TEST_CASE[VT.Str], SAMPLE_K)]
    for _ in range(CASE_NUM)
]

TEST_CASE[VT.Set] = [
    set(
        [val for val in sample(TEST_CASE[VT.Int], SAMPLE_K)]
        + [val for val in sample(TEST_CASE[VT.Str], SAMPLE_K)]
    )
    for _ in range(CASE_NUM)
]

TEST_CASE[VT.Datetime] = [datetime.now() for _ in range(CASE_NUM)]

TEST_CASE[VT.Path] = [Path(choice(TEST_CASE[VT.Str])) for _ in range(CASE_NUM)]


class InnerModel(BaseModel):
    mInt: int
    mBool: bool
    mStr: str


class Model(BaseModel):
    mModel: InnerModel
    mDict: dict
    mList: list
    mSet: set
    mDatetime: datetime
    mPath: Path


TEST_CASE[VT.Model] = [
    Model(
        mModel=InnerModel(
            mInt=choice(TEST_CASE[VT.Int]),
            mBool=choice(TEST_CASE[VT.Bool]),
            mStr=choice(TEST_CASE[VT.Str]),
        ),
        mDict=choice(TEST_CASE[VT.Dict]),
        mList=choice(TEST_CASE[VT.List]),
        mSet=choice(TEST_CASE[VT.Set]),
        mDatetime=choice(TEST_CASE[VT.Datetime]),
        mPath=choice(TEST_CASE[VT.Path]),
    )
    for _ in range(CASE_NUM // 2)
]


def get_test_case(vt: VT) -> list:
    return deepcopy(TEST_CASE[vt])
