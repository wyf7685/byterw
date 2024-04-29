#include "pywriter.h"
#include "crypt.h"
#include "floatobject.h"
#include "longobject.h"
#include "pyobj.h"
#include "tupleobject.h"
#include "writer.h"

#include <memory>

namespace byterw::writer {
using inner::BWriter;

typedef struct {
  PyObject_HEAD;
  std::unique_ptr<BWriter> writer;
  long long key;
} ByteWriter;

static PyObject *ByteWriter_new(PyTypeObject *type, PyObject *args,
                                PyObject *kwds) {
  ByteWriter *self = (ByteWriter *)type->tp_alloc(type, 0);
  if (self) {
    self->key = 7685;
    if (PyTuple_Size(args) == 1) {
      self->key = crypt::parse_key(PyTuple_GetItem(args, 0));
    }
    self->writer = std::make_unique<BWriter>();
  }
  return (PyObject *)self;
}

static void ByteWriter_dealloc(ByteWriter *self) {
  Py_TYPE(self)->tp_free((PyObject *)self);
}

// ========== Method start ==========

static PyObject *write_none(ByteWriter *self, PyObject *args) {
  PyObject *obj;

  if (PyTuple_Size(args) == 1 &&
      (!PyArg_ParseTuple(args, "O", &obj) || !Py_IsNone(obj))) {
    PyErr_SetString(PyExc_TypeError, "obj should be None");
    return nullptr;
  }

  self->writer->write_sign(ValueType::Null);
  self->writer->buffer.append(0);

  Py_INCREF(self);
  return (PyObject *)self;
}

static PyObject *write_int(ByteWriter *self, PyObject *args) {
  long long value;

  if (!PyArg_ParseTuple(args, "L", &value)) {
    PyErr_SetString(PyExc_TypeError, "obj should be int");
    return nullptr;
  }

  self->writer->write_sign(ValueType::Int);
  if (self->writer->write_long(value) < 0)
    return nullptr;

  Py_INCREF(self);
  return (PyObject *)self;
}

static PyObject *write_float(ByteWriter *self, PyObject *args) {
  Py_ssize_t arglen = PyTuple_Size(args);
  if (arglen < 1 || arglen > 2) {
    PyErr_SetString(PyExc_TypeError, "write_float requires 1 or 2 args");
    return nullptr;
  }

  PyObject *obj = PyTuple_GetItem(args, 0);
  if (!PyFloat_Check(obj)) {
    PyErr_SetString(PyExc_TypeError, "obj should be float");
    return nullptr;
  }
  double value = PyFloat_AS_DOUBLE(obj);
  int precision = 8;

  if (arglen == 2) {
    PyObject *obj = PyTuple_GetItem(args, 1);
    if (!PyLong_Check(obj)) {
      PyErr_SetString(PyExc_TypeError, "obj should be int");
      return nullptr;
    }
    precision = PyLong_AsLong(obj);
  }

  self->writer->write_sign(ValueType::Float);
  if (self->writer->write_double(value, precision) < 0)
    return nullptr;

  Py_INCREF(self);
  return (PyObject *)self;
}

static PyObject *write_bool(ByteWriter *self, PyObject *args) {
  PyObject *obj;

  if (!PyArg_ParseTuple(args, "O!", &PyBool_Type, &obj)) {
    PyErr_SetString(PyExc_TypeError, "obj should be bool");
    return nullptr;
  }

  self->writer->write_sign(ValueType::Bool);
  if (self->writer->write_bool(Py_IsTrue(obj)) < 0)
    return nullptr;

  Py_INCREF(self);
  return (PyObject *)self;
}

static PyObject *write_string(ByteWriter *self, PyObject *args) {
  char *str;
  Py_ssize_t length;

  if (!PyArg_ParseTuple(args, "s#", &str, &length)) {
    PyErr_SetString(PyExc_TypeError, "obj should be str");
    return nullptr;
  }

  self->writer->write_sign(ValueType::Str);
  if (self->writer->write_bytes(str, length) < 0)
    return nullptr;

  Py_INCREF(self);
  return (PyObject *)self;
}

static PyObject *write_bytes(ByteWriter *self, PyObject *args) {
  char *str;
  Py_ssize_t length;

  if (!PyArg_ParseTuple(args, "s#", &str, &length)) {
    PyErr_SetString(PyExc_TypeError, "obj should be bytes");
    return nullptr;
  }

  self->writer->write_sign(ValueType::Bytes);
  if (self->writer->write_bytes(str, length) < 0)
    return nullptr;

  Py_INCREF(self);
  return (PyObject *)self;
}

static PyObject *write_dict(ByteWriter *self, PyObject *args) {
  PyObject *obj;

  if (!PyArg_ParseTuple(args, "O!", &PyDict_Type, &obj)) {
    PyErr_SetString(PyExc_TypeError, "obj should be dict");
    return nullptr;
  }

  self->writer->write_sign(ValueType::Dict);
  if (self->writer->write_dict(obj) < 0)
    return nullptr;

  Py_INCREF(self);
  return (PyObject *)self;
}

static PyObject *write_list(ByteWriter *self, PyObject *args) {
  PyObject *obj;

  if (!PyArg_ParseTuple(args, "O!", &PyList_Type, &obj)) {
    PyErr_SetString(PyExc_TypeError, "obj should be list");
    return nullptr;
  }

  self->writer->write_sign(ValueType::List);
  if (self->writer->write_list(obj) < 0)
    return nullptr;

  Py_INCREF(self);
  return (PyObject *)self;
}

static PyObject *write_set(ByteWriter *self, PyObject *args) {
  PyObject *obj;

  if (!PyArg_ParseTuple(args, "O!", &PySet_Type, &obj)) {
    PyErr_SetString(PyExc_TypeError, "obj should be set");
    return nullptr;
  }

  self->writer->write_sign(ValueType::Set);
  if (self->writer->write_set(obj) < 0)
    return nullptr;

  Py_INCREF(self);
  return (PyObject *)self;
}

static PyObject *write_tuple(ByteWriter *self, PyObject *args) {
  PyObject *obj;

  if (!PyArg_ParseTuple(args, "O!", &PyTuple_Type, &obj)) {
    PyErr_SetString(PyExc_TypeError, "obj should be tuple");
    return nullptr;
  }

  self->writer->write_sign(ValueType::Tuple);
  if (self->writer->write_tuple(obj) < 0)
    return nullptr;

  Py_INCREF(self);
  return (PyObject *)self;
}

static PyObject *write_datetime(ByteWriter *self, PyObject *args) {
  PyObject *obj;

  if (!PyArg_ParseTuple(args, "O!", pyobj::load(pyobj::datetime), &obj)) {
    PyErr_SetString(PyExc_TypeError, "obj should be datetime");
    return nullptr;
  }

  self->writer->write_sign(ValueType::Datetime);
  if (self->writer->write_datetime(obj) < 0)
    return nullptr;

  Py_INCREF(self);
  return (PyObject *)self;
}

static PyObject *write_path(ByteWriter *self, PyObject *args) {
  PyObject *obj;

  if (!PyArg_ParseTuple(args, "O!", pyobj::load(pyobj::Path), &obj)) {
    PyErr_SetString(PyExc_TypeError, "obj should be Path");
    return nullptr;
  }

  self->writer->write_sign(ValueType::Path);
  if (self->writer->write_path(obj))
    return nullptr;

  Py_INCREF(self);
  return (PyObject *)self;
}

static PyObject *write_model(ByteWriter *self, PyObject *args) {
  auto BaseModel = pyobj::load(pyobj::BaseModel);
  if (!BaseModel) {
    PyErr_SetString(PyExc_TypeError,
                    "pydantic not found, write_model is not avaliable");
    return nullptr;
  }

  PyObject *obj;

  if (!PyArg_ParseTuple(args, "O!", BaseModel, &obj)) {
    PyErr_SetString(PyExc_TypeError, "obj should be BaseModel");
    return nullptr;
  }

  self->writer->write_sign(ValueType::Model);
  if (self->writer->write_model(obj) < 0)
    return nullptr;

  Py_INCREF(self);
  return (PyObject *)self;
}

static PyObject *write(ByteWriter *self, PyObject *args) {
  PyObject *obj;

  if (!PyArg_ParseTuple(args, "O", &obj)) {
    // Maybe no args passed in
    PyErr_SetString(PyExc_TypeError, "obj must be an object");
    return nullptr;
  }

  if (self->writer->write_object(obj, true) < 0)
    return nullptr;

  Py_INCREF(self);
  return (PyObject *)self;
}

static PyObject *get(ByteWriter *self, PyObject *args) {
  const unsigned char *data = self->writer->buffer.get();
  std::size_t size = self->writer->buffer.size();
  auto bytes = PyBytes_FromStringAndSize((char *)data, size);
  return bytes ? crypt::encrypt(bytes, self->key) : nullptr;
}

// ========== Method end ==========

// static PyMemberDef members[] = {
//     {nullptr}, /* Sentinel */
// };

static PyMethodDef methods[] = {
    {"write_none", (PyCFunction)write_none, METH_VARARGS},
    {"write_int", (PyCFunction)write_int, METH_VARARGS},
    {"write_float", (PyCFunction)write_float, METH_VARARGS},
    {"write_bool", (PyCFunction)write_bool, METH_VARARGS},
    {"write_string", (PyCFunction)write_string, METH_VARARGS},
    {"write_bytes", (PyCFunction)write_bytes, METH_VARARGS},
    {"write_dict", (PyCFunction)write_dict, METH_VARARGS},
    {"write_list", (PyCFunction)write_list, METH_VARARGS},
    {"write_set", (PyCFunction)write_set, METH_VARARGS},
    {"write_tuple", (PyCFunction)write_tuple, METH_VARARGS},
    {"write_datetime", (PyCFunction)write_datetime, METH_VARARGS},
    {"write_path", (PyCFunction)write_path, METH_VARARGS},
    {"write_model", (PyCFunction)write_model, METH_VARARGS},
    {"write", (PyCFunction)write, METH_VARARGS},
    {"get", (PyCFunction)get, METH_NOARGS},
    {nullptr, nullptr, 0} /* Sentinel */
};

void init(PyTypeObject *TypeObject) {
  //   TypeObject->ob_base = {{{1}, (0)}, (0)};
  //   TypeObject->tp_name = "byterw.ByteWriter";
  TypeObject->tp_basicsize = sizeof(ByteWriter);
  TypeObject->tp_itemsize = 0;
  TypeObject->tp_dealloc = (destructor)ByteWriter_dealloc;
  // TypeObject->tp_repr = (reprfunc)repr;
  TypeObject->tp_flags = Py_TPFLAGS_DEFAULT;
  TypeObject->tp_methods = methods;
  // TypeObject->tp_members = members;
  TypeObject->tp_new = ByteWriter_new;
}

} // namespace byterw::writer
