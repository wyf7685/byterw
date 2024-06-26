#include "pywriter.h"
#include "crypt.h"
#include "pyobj.h"
#include "writer.h"

#include <memory> // std::unique_ptr

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
  if (!self->writer->write_long(value))
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
  if (!self->writer->write_double(value, precision))
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
  if (!self->writer->write_bool(Py_IsTrue(obj)))
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
  if (!self->writer->write_bytes(str, length))
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
  if (!self->writer->write_bytes(str, length))
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
  if (!self->writer->write_dict(obj))
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
  if (!self->writer->write_list(obj))
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
  if (!self->writer->write_set(obj))
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
  if (!self->writer->write_tuple(obj))
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
  if (!self->writer->write_datetime(obj))
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
  if (!self->writer->write_path(obj))
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
  if (!self->writer->write_model(obj))
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

  if (!self->writer->write_object(obj, true))
    return nullptr;

  Py_INCREF(self);
  return (PyObject *)self;
}

static PyObject *get(ByteWriter *self, PyObject *args) {
  const unsigned char *data = self->writer->get();
  std::size_t size = self->writer->size();
  auto bytes = PyBytes_FromStringAndSize((char *)data, size);
  return bytes ? crypt::encrypt(bytes, self->key) : nullptr;
}

// ========== Method end ==========

#define NewMethodDef(identifier, flag)                                         \
  { #identifier, (PyCFunction)identifier, flag }

} // namespace byterw::writer

PyTypeObject *byterw::writer::init() {

  // static PyMemberDef members[] = {
  //     {nullptr}, /* Sentinel */
  // };

  static PyMethodDef methods[] = {
      NewMethodDef(write_none, METH_VARARGS),
      NewMethodDef(write_int, METH_VARARGS),
      NewMethodDef(write_float, METH_VARARGS),
      NewMethodDef(write_bool, METH_VARARGS),
      NewMethodDef(write_string, METH_VARARGS),
      NewMethodDef(write_bytes, METH_VARARGS),
      NewMethodDef(write_dict, METH_VARARGS),
      NewMethodDef(write_list, METH_VARARGS),
      NewMethodDef(write_set, METH_VARARGS),
      NewMethodDef(write_tuple, METH_VARARGS),
      NewMethodDef(write_datetime, METH_VARARGS),
      NewMethodDef(write_path, METH_VARARGS),
      NewMethodDef(write_model, METH_VARARGS),
      NewMethodDef(write, METH_VARARGS),
      NewMethodDef(get, METH_NOARGS),
      {nullptr, nullptr, 0} /* Sentinel */
  };

  static PyTypeObject TypeObject = {
      .ob_base = {{{1}, (0)}, (0)},
      .tp_name = "byterw.ByteWriter",
      .tp_basicsize = sizeof(ByteWriter),
      .tp_itemsize = 0,
      .tp_dealloc = (destructor)ByteWriter_dealloc,
      // .tp_repr = (reprfunc)repr,
      .tp_flags = Py_TPFLAGS_DEFAULT,
      .tp_methods = methods,
      // .tp_members = members,
      .tp_new = ByteWriter_new,
  };
  return &TypeObject;
}

#undef NewMethodDef