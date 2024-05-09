#include "reader.h"
#include "pyobj.h"

namespace byterw::reader::inner {

PyObject *vt2t(ValueType vt) {
  switch (vt) {
  case byterw::ValueType::Null:
    return Py_None;
  case byterw::ValueType::Int:
    return (PyObject *)&PyLong_Type;
  case byterw::ValueType::Float:
    return (PyObject *)&PyFloat_Type;
  case byterw::ValueType::Bool:
    return (PyObject *)&PyBool_Type;
  case byterw::ValueType::Str:
    return (PyObject *)&PyUnicode_Type;
  case byterw::ValueType::Bytes:
    return (PyObject *)&PyBytes_Type;
  case byterw::ValueType::Dict:
    return (PyObject *)&PyDict_Type;
  case byterw::ValueType::List:
    return (PyObject *)&PyList_Type;
  case byterw::ValueType::Set:
    return (PyObject *)&PySet_Type;
  case byterw::ValueType::Tuple:
    return (PyObject *)&PyTuple_Type;
  case byterw::ValueType::Datetime:
    return pyobj::load(pyobj::datetime, false);
  case byterw::ValueType::Path:
    return pyobj::load(pyobj::Path, false);
  case byterw::ValueType::Model:
    return pyobj::load(pyobj::BaseModel, false);
  case byterw::ValueType::Unsupported:
    PyErr_SetString(PyExc_TypeError, "Unsupported type");
    return nullptr;
  default:
    PyErr_SetString(PyExc_TypeError, "Unkown type");
    return nullptr;
  }
}

BReader::BReader(const unsigned char *buf, std::size_t bufSize) {
  buffer = new unsigned char[bufSize];
  size = bufSize;
  ptr = buffer;
  memcpy(buffer, buf, bufSize);
}

BReader::~BReader() { delete[] buffer; }

ValueType BReader::read_sign() { return static_cast<ValueType>(*ptr++); }

PyObject *BReader::read_none() { Py_RETURN_NONE; }

PyObject *BReader::read_long() {
  unsigned char length = read_single_byte();

  PyObject *pybytes = PyBytes_FromStringAndSize(nullptr, length + 1);
  if (pybytes == nullptr) {
    // How ?!
    PyErr_SetString(PyExc_RuntimeError, "Error allocating bytes");
    return nullptr;
  }
  unsigned char *bytes = (unsigned char *)PyBytes_AS_STRING(pybytes);

  memcpy(bytes, ptr, length);
  ptr += length;

  PyObject *pyint = _PyLong_FromByteArray(bytes, length, false, true);
  Py_DECREF(pybytes);
  return pyint;
}

PyObject *BReader::read_double() {
  int precision = read_single_byte();
  double v = read_struct_length();
  return PyFloat_FromDouble(v / std::pow(10, precision));
}

PyObject *BReader::read_bool() { return PyBool_FromLong(read_single_byte()); }

PyObject *BReader::read_string() {
  Py_ssize_t length = read_struct_length();
  if (length < 0)
    return nullptr;

  PyObject *pystr = PyUnicode_FromStringAndSize((char *)ptr, length);
  if (!pystr) {
    PyErr_SetString(PyExc_RuntimeError, "Error creating str obj");
    return nullptr;
  }

  ptr += length;
  return pystr;
}

PyObject *BReader::read_bytes() {
  Py_ssize_t length = read_struct_length();
  if (length < 0)
    return nullptr;

  PyObject *pybytes = PyBytes_FromStringAndSize((char *)ptr, length);
  if (!pybytes) {
    PyErr_SetString(PyExc_RuntimeError, "Error creating bytes obj");
    return nullptr;
  }

  ptr += length;
  return pybytes;
}

PyObject *BReader::read_dict() {
  Py_ssize_t length = read_struct_length();
  if (length < 0)
    return nullptr;

  PyObject *pydict = PyDict_New();
  if (pydict == nullptr) {
    PyErr_SetString(PyExc_RuntimeError, "Error creating dict obj");
    return nullptr;
  }

  PyObject *key = nullptr, *value = nullptr;
  auto clean = [&]() -> void {
    Py_XDECREF(pydict), Py_XDECREF(key), Py_XDECREF(value);
  };

  for (Py_ssize_t i = 0; i < length; ++i) {
    key = read_object(read_sign());
    if (key == nullptr) {
      PyErr_SetString(PyExc_RuntimeError, "Error reading key object");
      clean();
      return nullptr;
    }
    value = read_object(read_sign());
    if (value == nullptr) {
      PyErr_SetString(PyExc_RuntimeError, "Error reading value object");
      clean();
      return nullptr;
    }
    if (PyDict_SetItem(pydict, key, value) < 0) {
      PyErr_SetString(PyExc_RuntimeError, "Error setting dict item");
      clean();
      return nullptr;
    }
    key = value = nullptr;
  }

  return pydict;
}

PyObject *BReader::read_list() {
  Py_ssize_t length = read_struct_length();
  if (length < 0)
    return nullptr;

  PyObject *pylist = PyList_New(length);
  if (pylist == nullptr) {
    PyErr_SetString(PyExc_RuntimeError, "Error creating list obj");
    return nullptr;
  }

  for (Py_ssize_t i = 0; i < length; ++i) {
    PyObject *item = read_object(read_sign());
    if (!item) {
      Py_DECREF(pylist);
      PyErr_SetString(PyExc_RuntimeError, "Error reading item object");
      return nullptr;
    }
    if (PyList_SetItem(pylist, i, item) < 0) {
      Py_DECREF(pylist);
      Py_DECREF(item);
      PyErr_SetString(PyExc_RuntimeError, "Error setting list item");
      return nullptr;
    }
  }

  return pylist;
}

PyObject *BReader::read_set() {
  PyObject *pylist = read_list();
  if (pylist == nullptr) {
    PyErr_SetString(PyExc_RuntimeError, "Error reading set");
    return nullptr;
  }
  return PySet_New(pylist);
}

PyObject *BReader::read_tuple() {
  Py_ssize_t length = read_struct_length();
  if (length < 0)
    return nullptr;

  PyObject *pytuple = PyTuple_New(length);
  for (Py_ssize_t i = 0; i < length; ++i) {
    PyObject *obj = read_object(read_sign());
    if (obj == nullptr) {
      Py_DECREF(pytuple);
      PyErr_SetString(PyExc_RuntimeError, "Error reading tuple item");
      return nullptr;
    }
    if (PyTuple_SetItem(pytuple, i, obj) < 0) {
      Py_DECREF(pytuple);
      Py_DECREF(obj);
      PyErr_SetString(PyExc_RuntimeError, "Error setting tuple item");
      return nullptr;
    }
  }

  return pytuple;
}

PyObject *BReader::read_datetime() {
  PyObject *pyfloat_timestamp = read_double();
  if (pyfloat_timestamp == nullptr) {
    PyErr_SetString(PyExc_RuntimeError, "Error reading datetime timestamp");
    return nullptr;
  }

  return PyObject_CallOneArg(pyobj::load(pyobj::datetime_from_timestamp),
                             pyfloat_timestamp);
}

PyObject *BReader::read_path() {
  PyObject *pystr_path = read_string();
  if (pystr_path == nullptr) {
    PyErr_SetString(PyExc_RuntimeError, "Error reading path value");
    return nullptr;
  }
  return PyObject_CallOneArg(pyobj::load(pyobj::Path), pystr_path);
}

PyObject *BReader::read_model() {
  if (pyobj::load(pyobj::BaseModel, false) == nullptr) {
    PyErr_SetString(PyExc_RuntimeError, "Error: pydantic not found");
    return nullptr;
  }

  Py_ssize_t length = read_struct_length();
  if (length < 0)
    return nullptr;

  PyObject *model_name = read_string();
  if (model_name == nullptr)
    return nullptr;

  PyObject *Field = pyobj::load(pyobj::ModelField, false);
  PyObject *fields = PyDict_New(); // dict
  PyObject *parsed = PyDict_New(); // dict
  PyObject *attrs = PyDict_New();  // dict

  while (length--) {
    // Read each field of model

    PyObject *field_name = read_string();
    if (field_name == nullptr) {
      Py_DECREF(fields);
      Py_DECREF(parsed);
      Py_DECREF(attrs);
      return nullptr;
    }

    ValueType vt = read_sign();
    PyObject *value = read_object(vt);
    if (value == nullptr) {
      Py_DECREF(fields);
      Py_DECREF(parsed);
      Py_DECREF(attrs);
      Py_DECREF(field_name);
      return nullptr;
    }

    PyDict_SetItem(fields, field_name, vt2t(vt));
    PyDict_SetItem(parsed, field_name, value);

    PyObject *kwargs = PyDict_New();
    PyDict_SetItemString(kwargs, "default", value);
    PyObject *field_obj = PyObject_Call(Field, PyTuple_New(0), kwargs);
    PyDict_SetItem(attrs, field_name, field_obj);
  }

  PyDict_SetItemString(attrs, "__annotations__", fields);

  // Use `builtins.type` to create a Model class in runtime
  PyObject *Model = PyObject_Call(
      pyobj::load(pyobj::type), // callable = builtins.type
      PyTuple_Pack(3,
                   model_name,                                     // class name
                   PyTuple_Pack(1, pyobj::load(pyobj::BaseModel)), // base class
                   attrs                                           // attributes
                   ),
      nullptr); // kwargs = None

  // Create a Model object from parsed data
  PyObject *validate = PyObject_GetAttrString(Model, "model_validate");
  PyObject *model = PyObject_CallOneArg(validate, parsed);

  Py_DECREF(fields);
  Py_DECREF(parsed);
  Py_DECREF(attrs);
  return model;
}

PyObject *BReader::read_object(ValueType vt) {
  switch (vt) {
  case byterw::ValueType::Null:
    return read_none();
  case byterw::ValueType::Int:
    return read_long();
  case byterw::ValueType::Float:
    return read_double();
  case byterw::ValueType::Bool:
    return read_bool();
  case byterw::ValueType::Str:
    return read_string();
  case byterw::ValueType::Bytes:
    return read_bytes();
  case byterw::ValueType::Dict:
    return read_dict();
  case byterw::ValueType::List:
    return read_list();
  case byterw::ValueType::Set:
    return read_set();
  case byterw::ValueType::Tuple:
    return read_tuple();
  case byterw::ValueType::Datetime:
    return read_datetime();
  case byterw::ValueType::Path:
    return read_path();
  case byterw::ValueType::Model:
    return read_model();
  case byterw::ValueType::Unsupported:
    PyErr_SetString(PyExc_TypeError, "Unsupported type");
    return nullptr;
  }
}

Py_ssize_t BReader::read_struct_length() {
  PyObject *pyint_length = read_long();
  if (!pyint_length) {
    PyErr_SetString(PyExc_RuntimeError, "Error reading struct length");
    return -1;
  }

  Py_ssize_t length = PyLong_AsSsize_t(pyint_length);
  Py_DECREF(pyint_length);
  return length;
}

} // namespace byterw::reader::inner
