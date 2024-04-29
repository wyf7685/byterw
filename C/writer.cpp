#include "writer.h"
#include "buffer.h"
#include "common.h"
#include "modsupport.h"
#include "pyerrors.h"
#include "pyobj.h"
#include "pyport.h"

#include <string>

namespace byterw::writer::inner {

void BWriter::write_sign(ValueType vt) { buffer.append(static_cast<char>(vt)); }

BYTERW_NO_DISCARD int BWriter::write_none() { return 0; }

BYTERW_NO_DISCARD int BWriter::write_long(PyObject *value) {
  char length = _PyLong_NumBits(value) / 8 + 1;

  PyObject *bytes = PyBytes_FromStringAndSize(nullptr, length);
  if (!bytes) {
    PyErr_SetString(PyExc_RuntimeError, "Failed to create bytes obj for int");
    return -1;
  }

  if (_PyLong_AsByteArray((PyLongObject *)value,
                          (unsigned char *)PyBytes_AS_STRING(bytes), length,
                          false, true) < 0) {
    PyErr_SetString(PyExc_RuntimeError, "Failed to convert int to bytes");
    return -1;
  }

  buffer.append(length);
  buffer.extend((unsigned char *)PyBytes_AS_STRING(bytes), length);
  return 0;
}

BYTERW_NO_DISCARD int BWriter::write_long(long long value) {
  PyObject *pyint = PyLong_FromLongLong(value);
  int res = write_long(pyint);
  Py_DECREF(pyint);
  return res;
}

BYTERW_NO_DISCARD int BWriter::write_double(double value, int precision) {
  long long val = value * std::pow(10, precision);
  buffer.append(precision);
  return write_long(val);
}

BYTERW_NO_DISCARD int BWriter::write_double(PyObject *value, int precision) {
  return write_double(PyFloat_AS_DOUBLE(value));
}

BYTERW_NO_DISCARD int BWriter::write_bool(bool value) {
  buffer.append(value);
  return 0;
}

BYTERW_NO_DISCARD int BWriter::write_bool(PyObject *value) {
  return write_bool(Py_IsTrue(value));
}

BYTERW_NO_DISCARD int BWriter::write_bytes(const char *value,
                                           Py_ssize_t length) {
  if (write_long(length) < 0)
    return -1;
  buffer.extend((unsigned char *)value, length);
  return 0;
}

BYTERW_NO_DISCARD int BWriter::write_string(PyObject *value) {
  char *data;
  Py_ssize_t length;
  PyObject *args = PyTuple_Pack(1, value);
  PyArg_ParseTuple(args, "s#", &data, &length);
  return write_bytes(data, length);
}

BYTERW_NO_DISCARD int BWriter::write_bytes(PyObject *value) {
  return write_bytes(PyBytes_AsString(value), PyObject_Length(value));
}

BYTERW_NO_DISCARD int BWriter::write_dict(PyObject *value) {
  long length = PyDict_GET_SIZE(value);
  if (write_long(length) < 0)
    return -1;

  PyObject *keys = PyDict_Keys(value);
  for (Py_ssize_t i = 0; i < PyList_Size(keys); ++i) {
    PyObject *key = PyList_GetItem(keys, i);
    if (write_object(PyList_GetItem(keys, i), true) < 0)
      return -1;
    if (write_object(PyDict_GetItem(value, key), true) < 0)
      return -1;
  }
  return 0;
}

BYTERW_NO_DISCARD int BWriter::write_list(PyObject *value) {
  Py_ssize_t length = PyList_Size(value);
  if (write_long(length) < 0)
    return -1;

  for (Py_ssize_t i = 0; i < length; ++i)
    if (write_object(PyList_GetItem(value, i), true) < 0)
      return -1;
  return 0;
}

BYTERW_NO_DISCARD int BWriter::write_set(PyObject *value) {
  long length = PySet_Size(value);
  if (write_long(length) < 0)
    return -1;

  PyObject *iter = PyObject_GetIter(value);
  PyObject *item;

  if (!iter) {
    PyErr_SetString(PyExc_ValueError, "Failed getting iter of set");
    return -1;
  }

  while ((item = PyIter_Next(iter))) {
    Py_DECREF(item);
    if (write_object(item, true) < 0) {
      Py_DECREF(iter);
      return -1;
    }
  }

  Py_DECREF(iter);
  return 0;
}

BYTERW_NO_DISCARD int BWriter::write_tuple(PyObject *value) {
  long length = PyTuple_Size(value);
  if (write_long(length) < 0)
    return -1;

  PyObject *iter = PyObject_GetIter(value);
  PyObject *item;

  if (!iter) {
    PyErr_SetString(PyExc_ValueError, "Failed getting iter of tuple");
    return -1;
  }

  while ((item = PyIter_Next(iter))) {
    Py_DECREF(item);
    if (write_object(item, true) < 0) {
      Py_DECREF(iter);
      return -1;
    }
  }

  Py_DECREF(iter);
  return 0;
}

BYTERW_NO_DISCARD int BWriter::write_datetime(PyObject *value) {
  PyObject *timestamp =
      PyObject_CallMethodNoArgs(value, PyUnicode_FromString("timestamp"));
  if (!timestamp)
    return -1;
  return write_double(PyFloat_AS_DOUBLE(timestamp));
}

BYTERW_NO_DISCARD int BWriter::write_path(PyObject *value) {
  PyObject *str =
      PyObject_CallMethodNoArgs(value, PyUnicode_FromString("as_posix"));
  if (!str)
    return -1;
  return write_string(str);
}

BYTERW_NO_DISCARD int BWriter::write_model(PyObject *value) {
  PyObject *field_dict = PyObject_GetAttrString(value, "model_fields");
  if (write_long(PyObject_Length(field_dict)) < 0)
    return -1;

  std::string model_name(Py_TYPE(value)->tp_name);
  if (write_bytes(model_name.c_str(), model_name.length()) < 0)
    return -1;

  PyObject *field_dict_keys = PyDict_Keys(field_dict);
  for (Py_ssize_t i = 0; i < PyList_Size(field_dict_keys); ++i) {
    PyObject *field_name = PyList_GetItem(field_dict_keys, i); // PyUnicode
    PyObject *field_obj = PyDict_GetItem(field_dict, field_name);
    PyObject *field_anno = PyObject_GetAttrString(field_obj, "annotations");
    if (Py_IsNone(field_anno)) {
      PyErr_SetString(PyExc_ValueError, "annotation error: cannot be None");
      return -1;
    }

    if (write_string(field_name) < 0)
      return -1;
    PyObject *field_value =
        PyObject_GetAttrString(value, (char *)PyUnicode_DATA(field_name));
    if (field_value == nullptr || write_object(field_value, true) < 0)
      return -1;
  }
  return 0;
}

BYTERW_NO_DISCARD int BWriter::write_object(PyObject *obj, bool with_sign) {
  auto vt = get_vt(obj);

  if (with_sign)
    write_sign(vt);

  switch (vt) {
  case ValueType::Null: // None
    return write_none();

  case ValueType::Int: // int
    return write_long(obj);

  case ValueType::Float: // float
    return write_double(obj);

  case ValueType::Bool: // bool
    return write_bool(obj);

  case ValueType::Str: // str
    return write_string(obj);

  case ValueType::Bytes: // bytes
    return write_bytes(obj);

  case ValueType::Dict: // dict
    return write_dict(obj);

  case ValueType::List: // list
    return write_list(obj);

  case ValueType::Set: // set
    return write_set(obj);

  case ValueType::Tuple: // tuple
    return write_tuple(obj);

  case ValueType::Path: // pathlib.Path
    return write_path(obj);

  case ValueType::Datetime: // datetime.datetime
    return write_datetime(obj);

  case ValueType::Model: // pydantic.BaseModel (optional)
    return write_model(obj);

  case ValueType::Unsupported:
    PyErr_SetString(PyExc_TypeError, "Unsupported type");
    return -1;
  }

  return 0;
}

static ValueType get_vt(PyObject *obj) {
  // fast path for `None`
  if (Py_IsNone(obj))
    return ValueType::Null;

  // Make a shortcut
  auto check = [&obj](void *typ) -> bool {
    return PyObject_IsInstance(obj, (PyObject *)typ) > 0;
  };

  // Special annotations
  if (check(pyobj::load(pyobj::_UGA)) || check(pyobj::load(pyobj::UnionType)))
    return get_vt(PyTuple_GET_ITEM(PyObject_GetAttrString(obj, "__args__"), 0));
  else if (check(pyobj::load(pyobj::GA)))
    return get_vt(PyObject_GetAttrString(obj, "__origin__"));

  // Common types
  if (check(&PyBool_Type))
    return ValueType::Bool;
  else if (check(&PyLong_Type))
    return ValueType::Int;
  else if (check(&PyFloat_Type))
    return ValueType::Float;
  else if (check(&PyUnicode_Type))
    return ValueType::Str;
  else if (check(&PyBytes_Type))
    return ValueType::Bytes;
  else if (check(&PyDict_Type))
    return ValueType::Dict;
  else if (check(&PyList_Type))
    return ValueType::List;
  else if (check(&PySet_Type))
    return ValueType::Set;
  else if (check(&PyTuple_Type))
    return ValueType::Tuple;
  else if (check(pyobj::load(pyobj::Path)))
    return ValueType::Path;
  else if (check(pyobj::load(pyobj::datetime)))
    return ValueType::Datetime;

  // pydantic.BaseModel may not exist
  else if (pyobj::load(pyobj::BaseModel) &&
           check(pyobj::load(pyobj::BaseModel)))
    return ValueType::Model;

  // Unsupported types
  else
    return ValueType::Unsupported;
}

} // namespace byterw::writer::inner
