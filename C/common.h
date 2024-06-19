#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <string>
#include <unordered_map>

#define BYTERW_NO_DISCARD [[nodiscard]]

#ifndef BYTERW_VERSION
#define BYTERW_VERSION "0.1.0"
#endif

namespace byterw {

using std::string;

static const string VERSION = BYTERW_VERSION;

enum class ValueType {
  Null = 0,
  Int,
  Float,
  Bool,
  Str,
  Bytes,
  Dict,
  List,
  Set,
  Tuple,
  Datetime,
  Path,
  Model,
  Unsupported = (1 << 7) - 1,
};

} // namespace byterw
