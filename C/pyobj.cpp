#include "pyobj.h"

#include <unordered_map>

namespace byterw::pyobj {

static inline PyObject *import(const char *name) {
  return PyImport_ImportModule(name);
}

static inline PyObject *getattr(PyObject *obj, const char *name) {
  return PyObject_GetAttrString(obj, name);
}

static PyObject *real_load(int v) {
  static std::unordered_map<int, PyObject *> cache;

  if (cache.find(v) != cache.end())
    return cache[v];

  auto make_cache = [&v](PyObject *obj) -> PyObject * {
    cache[v] = obj;
    return obj;
  };

  switch (v) {
  case type:
    return make_cache(getattr(import("builtins"), "type"));
  case hash:
    return make_cache(getattr(import("builtins"), "hash"));

  case Path:
    return make_cache(getattr(import("pathlib"), "Path"));

  // datetime
  case datetime:
    return make_cache(getattr(import("datetime"), "datetime"));
  case datetime_from_timestamp:
    return make_cache(getattr(load(datetime), "fromtimestamp"));

  // pydantic
  case BaseModel: {
    auto module = import("pydantic");
    return make_cache(module ? getattr(module, "BaseModel") : nullptr);
  }
  case ModelField: {
    auto module = import("pydantic");
    return make_cache(module ? getattr(module, "Field") : nullptr);
  }

  // base64
  case b64encode:
    return make_cache(getattr(import("base64"), "b64encode"));
  case b64decode:
    return make_cache(getattr(import("base64"), "b64decode"));

  // types for annotation
  case _UGA:
    return make_cache(getattr(import("typing"), "_UnionGenericAlias"));
  case GA: {
    PyObject *typing = import("typing");
    PyObject *tuple = PyTuple_Pack(3, getattr(typing, "GenericAlias"),
                                   getattr(typing, "_GenericAlias"),
                                   getattr(typing, "_SpecialGenericAlias"));
    Py_DECREF(typing);
    return make_cache(tuple);
  }
  case UnionType:
    return make_cache(getattr(import("types"), "UnionType"));

  default:
    return nullptr;
  }
}

PyObject *load(int v, bool with_ref) {
  PyObject *obj = real_load(v);
  if (with_ref)
    Py_XINCREF(obj);
  return obj;
}

} // namespace byterw::pyobj
