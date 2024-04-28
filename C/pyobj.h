#include "common.h"

namespace byterw::pyobj {

/*
 * builtins.type
 *
 * builtins.hash
 *
 * pathlib.Path
 *
 * datetime.datetime
 *
 * pydantic.BaseModel (optional)
 *
 * pydantic.Field (optional)
 *
 * base64.b64encode
 * base64.b64decode
 *
 * _UGA: typing._UnionGenericAlias -> typing.Union
 *
 * GA:
 *   typing.GenericAlias -> list[str]
 *
 *   typing._GenericAlias -> typing.List[str]
 *
 *   typing._SpecialGenericAlias -> ...?
 *
 * types.UnionType -> str | int
 */

enum : int {
  type,
  hash,
  Path,
  datetime,
  datetime_from_timestamp,
  BaseModel,
  ModelField,
  b64encode,
  b64decode,
  _UGA,
  GA,
  UnionType,
};

PyObject *load(int v, bool with_ref = true);

} // namespace byterw::pyobj
