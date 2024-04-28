#include "pyreader.h"
#include "common.h"
#include "crypt.h"
#include "reader.h"

#include <memory>
#include <sstream>

namespace byterw::reader {
using inner::BReader;

typedef struct {
  PyObject_HEAD;
  std::unique_ptr<BReader> reader;
} ByteReader;

static PyObject *ByteReader_new(PyTypeObject *type, PyObject *args,
                                PyObject *kwds) {
  ByteReader *self = (ByteReader *)type->tp_alloc(type, 0);
  if (self) {
    auto bytes = PyTuple_GetItem(args, 0);
    if (!bytes || !PyBytes_Check(bytes)) {
      PyErr_SetString(PyExc_TypeError, "data should be bytes");
      Py_DECREF(self);
      return nullptr;
    }

    // ByteReader(data, key)
    long long key = 7685;
    if (PyTuple_Size(args) == 2) {
      key = crypt::parse_key(PyTuple_GetItem(args, 1));
    }

    PyObject *pybytes = crypt::decrypt(bytes, key);
    if (pybytes == nullptr) {
      PyErr_SetString(PyExc_RuntimeError, "Fail to decrypt data");
      return nullptr;
    }

    char *data;
    Py_ssize_t length;
    PyBytes_AsStringAndSize(pybytes, &data, &length);
    self->reader = std::make_unique<BReader>((unsigned char *)data, length);
  }
  return (PyObject *)self;
}

static void ByteReader_dealloc(ByteReader *self) {
  Py_TYPE(self)->tp_free((PyObject *)self);
}

// ========== Method start ==========

#define CHECK_BUFFER_READABLE()                                                \
  if (!self->reader->readable()) {                                             \
    PyErr_SetString(PyExc_ValueError, "buffer not readable");                  \
    return nullptr;                                                            \
  }

#define CHECK_VT(vt)                                                           \
  ValueType sign = self->reader->read_sign();                                  \
  if (sign != vt) {                                                            \
    std::stringstream ss;                                                      \
    ss << "vt not match: expect " << (int)vt;                                  \
    ss << ", got " << (int)sign << ".";                                        \
    PyErr_SetString(PyExc_TypeError, ss.str().c_str());                        \
    return nullptr;                                                            \
  }

static PyObject *read_none(ByteReader *self, PyObject *args) {
  CHECK_BUFFER_READABLE();
  CHECK_VT(ValueType::Null);
  return self->reader->read_object(sign);
}

static PyObject *read_int(ByteReader *self, PyObject *args) {
  CHECK_BUFFER_READABLE();
  CHECK_VT(ValueType::Int);
  return self->reader->read_object(sign);
}

static PyObject *read_float(ByteReader *self, PyObject *args) {
  CHECK_BUFFER_READABLE();
  CHECK_VT(ValueType::Float);
  return self->reader->read_object(sign);
}

static PyObject *read_bool(ByteReader *self, PyObject *args) {
  CHECK_BUFFER_READABLE();
  CHECK_VT(ValueType::Bool);
  return self->reader->read_object(sign);
}

static PyObject *read_string(ByteReader *self, PyObject *args) {
  CHECK_BUFFER_READABLE();
  CHECK_VT(ValueType::Str);
  return self->reader->read_object(sign);
}

static PyObject *read_bytes(ByteReader *self, PyObject *args) {
  CHECK_BUFFER_READABLE();
  CHECK_VT(ValueType::Bytes);
  return self->reader->read_object(sign);
}

static PyObject *read_dict(ByteReader *self, PyObject *args) {
  CHECK_BUFFER_READABLE();
  CHECK_VT(ValueType::Dict);
  return self->reader->read_object(sign);
}

static PyObject *read_list(ByteReader *self, PyObject *args) {
  CHECK_BUFFER_READABLE();
  CHECK_VT(ValueType::List);
  return self->reader->read_object(sign);
}

static PyObject *read_set(ByteReader *self, PyObject *args) {
  CHECK_BUFFER_READABLE();
  CHECK_VT(ValueType::Set);
  return self->reader->read_object(sign);
}

static PyObject *read_datetime(ByteReader *self, PyObject *args) {
  CHECK_BUFFER_READABLE();
  CHECK_VT(ValueType::Datetime);
  return self->reader->read_object(sign);
}

static PyObject *read_path(ByteReader *self, PyObject *args) {
  CHECK_BUFFER_READABLE();
  CHECK_VT(ValueType::Path);
  return self->reader->read_object(sign);
}

static PyObject *read_model(ByteReader *self, PyObject *args) {
  CHECK_BUFFER_READABLE();
  CHECK_VT(ValueType::Model);
  return self->reader->read_object(sign);
}

static PyObject *read(ByteReader *self, PyObject *args) {
  CHECK_BUFFER_READABLE();
  return self->reader->read_object(self->reader->read_sign());
}

static PyObject *any(ByteReader *self, PyObject *args) {
  return PyBool_FromLong(self->reader->readable());
}

#undef CHECK_BUFFER_READABLE
#undef CHECK_VT

// ========== Method end ==========

// static PyMemberDef members[] = {
//     {nullptr}, /* Sentinel */
// };

static PyMethodDef methods[] = {
    {"read_none", (PyCFunction)read_none, METH_NOARGS},
    {"read_int", (PyCFunction)read_int, METH_NOARGS},
    {"read_float", (PyCFunction)read_float, METH_NOARGS},
    {"read_bool", (PyCFunction)read_bool, METH_NOARGS},
    {"read_string", (PyCFunction)read_string, METH_NOARGS},
    {"read_bytes", (PyCFunction)read_bytes, METH_NOARGS},
    {"read_dict", (PyCFunction)read_dict, METH_NOARGS},
    {"read_list", (PyCFunction)read_list, METH_NOARGS},
    {"read_set", (PyCFunction)read_set, METH_NOARGS},
    {"read_datetime", (PyCFunction)read_datetime, METH_NOARGS},
    {"read_path", (PyCFunction)read_path, METH_NOARGS},
    {"read_model", (PyCFunction)read_model, METH_VARARGS},
    {"read", (PyCFunction)read, METH_NOARGS},
    {"any", (PyCFunction)any, METH_NOARGS},
    {nullptr, nullptr, 0} /* Sentinel */
};

void init(PyTypeObject *TypeObject) {
  // TypeObject->ob_base = {{{1}, (0)}, (0)};
  // TypeObject->tp_name = "byterw.ByteReader";
  TypeObject->tp_basicsize = sizeof(ByteReader);
  TypeObject->tp_itemsize = 0;
  TypeObject->tp_dealloc = (destructor)ByteReader_dealloc;
  // TypeObject->tp_repr = (reprfunc)repr;
  TypeObject->tp_flags = Py_TPFLAGS_DEFAULT;
  TypeObject->tp_methods = methods;
  // TypeObject->tp_members = members;
  TypeObject->tp_new = ByteReader_new;
}

} // namespace byterw::reader
