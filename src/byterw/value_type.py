# See C/common.h


class __I:
    i = 0

    def __call__(self) -> int:
        self.i += 1
        return self.i - 1

__i = __I()

Null = __i()
Int = __i()
Float = __i()
Bool = __i()
Str = __i()
Bytes = __i()
Dict = __i()
List = __i()
Set = __i()
Tuple = __i()
Datetime = __i()
Path = __i()
Model = __i()
Unsupported = (1 << 7) - 1

del __i
