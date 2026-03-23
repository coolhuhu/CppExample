from pymodule.lib._pymodule import (
    pyadd,
    pysub
)

def add(a: int, b: int) -> int:
    return pyadd(a, b)

def sub(a: int, b: int) -> int:
    return pysub(a, b)
    