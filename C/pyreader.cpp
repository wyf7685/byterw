#include "pyreader.h"
#include "common.h"
#include "crypt.h"
#include "reader.h"

#include <memory>  // std::unique_ptr
#include <sstream> // std::stringstream

namespace byterw::reader {
using inner::BReader;

typedef struct {
  PyObject_HEAD;
  std::unique_ptr<BReader> reader;
} ByteReader;

static PyObject *ByteReader_new(PyTypeObject *type, PyObject *args,
                                PyObject *kwds) {
  auto arglen = PyTuple_Size(args);
  if (arglen < 1 || arglen > 2) {
    std::stringstream ss;
    ss << "should be 1 or 2 args, got " << arglen << ".";
    PyErr_SetString(PyExc_TypeError, ss.str().c_str());
    return nullptr;
  }

  ByteReader *self = (ByteReader *)type->tp_alloc(type, 0);
  if (!self) {
    return nullptr;
  }

  auto bytes = PyTuple_GetItem(args, 0);
  if (!bytes || !PyBytes_Check(bytes)) {
    PyErr_SetString(PyExc_TypeError, "data should be bytes");
    Py_DECREF(self);
    return nullptr;
  }

  // ByteReader(data, key=7685)
  long long key = 7685;
  if (PyTuple_Size(args) == 2) {
    key = crypt::parse_key(PyTuple_GetItem(args, 1));
  }

  PyObject *pybytes = crypt::decrypt(bytes, key);
  if (pybytes == nullptr) {
    PyErr_SetString(PyExc_RuntimeError, "Fail to decrypt data");
    Py_DECREF(self);
    return nullptr;
  }

  char *data;
  Py_ssize_t length;
  PyBytes_AsStringAndSize(pybytes, &data, &length);
  self->reader = std::make_unique<BReader>((unsigned char *)data, length);

  return (PyObject *)self;
}

static void ByteReader_dealloc(ByteReader *self) {
  Py_TYPE(self)->tp_free((PyObject *)self);
}

// ========== Method start ==========

#define CHECK_VT(vt)                                                           \
  if (!self->reader->readable()) {                                             \
    PyErr_SetString(PyExc_ValueError, "buffer not readable");                  \
    return nullptr;                                                            \
  }                                                                            \
  ValueType sign = self->reader->read_sign();                                  \
  if (sign != vt) {                                                            \
    std::stringstream ss;                                                      \
    ss << "vt not match: expect " << (int)vt;                                  \
    ss << ", got " << (int)sign << ".";                                        \
    PyErr_SetString(PyExc_TypeError, ss.str().c_str());                        \
    return nullptr;                                                            \
  }

static PyObject *read_none(ByteReader *self, PyObject *args) {
  CHECK_VT(ValueType::Null);
  return self->reader->read_object(sign);
}

static PyObject *read_int(ByteReader *self, PyObject *args) {
  CHECK_VT(ValueType::Int);
  return self->reader->read_object(sign);
}

static PyObject *read_float(ByteReader *self, PyObject *args) {
  CHECK_VT(ValueType::Float);
  return self->reader->read_object(sign);
}

static PyObject *read_bool(ByteReader *self, PyObject *args) {
  CHECK_VT(ValueType::Bool);
  return self->reader->read_object(sign);
}

static PyObject *read_string(ByteReader *self, PyObject *args) {
  CHECK_VT(ValueType::Str);
  return self->reader->read_object(sign);
}

static PyObject *read_bytes(ByteReader *self, PyObject *args) {
  CHECK_VT(ValueType::Bytes);
  return self->reader->read_object(sign);
}

static PyObject *read_dict(ByteReader *self, PyObject *args) {
  CHECK_VT(ValueType::Dict);
  return self->reader->read_object(sign);
}

static PyObject *read_list(ByteReader *self, PyObject *args) {
  CHECK_VT(ValueType::List);
  return self->reader->read_object(sign);
}

static PyObject *read_set(ByteReader *self, PyObject *args) {
  CHECK_VT(ValueType::Set);
  return self->reader->read_object(sign);
}

static PyObject *read_tuple(ByteReader *self, PyObject *args) {
  CHECK_VT(ValueType::Tuple);
  return self->reader->read_object(sign);
}

static PyObject *read_datetime(ByteReader *self, PyObject *args) {
  CHECK_VT(ValueType::Datetime);
  return self->reader->read_object(sign);
}

static PyObject *read_path(ByteReader *self, PyObject *args) {
  CHECK_VT(ValueType::Path);
  return self->reader->read_object(sign);
}

static PyObject *read_model(ByteReader *self, PyObject *args) {
  CHECK_VT(ValueType::Model);
  return self->reader->read_object(sign);
}

static PyObject *read(ByteReader *self, PyObject *args) {
  if (!self->reader->readable()) {
    PyErr_SetString(PyExc_ValueError, "buffer not readable");
    return nullptr;
  }
  return self->reader->read_object(self->reader->read_sign());
}

static PyObject *any(ByteReader *self, PyObject *args) {
  return self->reader->readable() ? Py_True : Py_False;
}

#undef CHECK_VT

// ========== Method end ==========

} // namespace byterw::reader

#define NewMethodDef(identifier, flag)                                         \
  { #identifier, (PyCFunction)identifier, flag }

PyTypeObject *byterw::reader::init() {

  // static PyMemberDef members[] = {
  //     {nullptr}, /* Sentinel */
  // };

  static PyMethodDef methods[] = {
      NewMethodDef(read_none, METH_NOARGS),
      NewMethodDef(read_int, METH_NOARGS),
      NewMethodDef(read_float, METH_NOARGS),
      NewMethodDef(read_bool, METH_NOARGS),
      NewMethodDef(read_string, METH_NOARGS),
      NewMethodDef(read_bytes, METH_NOARGS),
      NewMethodDef(read_dict, METH_NOARGS),
      NewMethodDef(read_list, METH_NOARGS),
      NewMethodDef(read_set, METH_NOARGS),
      NewMethodDef(read_tuple, METH_NOARGS),
      NewMethodDef(read_datetime, METH_NOARGS),
      NewMethodDef(read_path, METH_NOARGS),
      NewMethodDef(read_model, METH_VARARGS),
      NewMethodDef(read, METH_NOARGS),
      NewMethodDef(any, METH_NOARGS),
      {nullptr, nullptr, 0} /* Sentinel */
  };

  static PyTypeObject TypeObject = {
      .ob_base = {{{1}, (0)}, (0)},
      .tp_name = "byterw.ByteReader",
      .tp_basicsize = sizeof(ByteReader),
      .tp_itemsize = 0,
      .tp_dealloc = (destructor)ByteReader_dealloc,
      // .tp_repr = (reprfunc)repr,
      .tp_flags = Py_TPFLAGS_DEFAULT,
      .tp_methods = methods,
      // .tp_members = members,
      .tp_new = ByteReader_new,
  };

  return &TypeObject;
}

#undef NewMethodDef