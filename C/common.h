#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <string>
#include <unordered_map>

#define BYTERW_NO_DISCARD [[nodiscard]]

namespace byterw {

using std::string;

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

/*
 * pathlib.Path
 *
 * datetime.datetime
 *
 * pydantic.BaseModel (optional)
 *
 * typing._UnionGenericAlias -> typing.Union
 *
 * typing.GenericAlias -> list[str]
 *
 * typing._GenericAlias -> typing.List[str]
 *
 * typing._SpecialGenericAlias -> ...?
 */

// enum class SpecialTypes : int {
//   Path,
//   datetime,
//   datetime_from_timestamp,
//   BaseModel,
//   _UGA,
//   GA,
//   _GA,
//   _SGA,
// };

// static PyObject *special_types[10];

// BYTERW_NO_DISCARD static inline bool special_types_initialized() {
//   // return special_types.size();
//   return true;
// }

} // namespace byterw
