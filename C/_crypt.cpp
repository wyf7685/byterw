#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <cstddef>
#include <string>

namespace byterw {

static const std::string basic_charset =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

size_t index_of(const std::string &str, const char ch) { return str.find(ch); }

static PyObject *encrypt(PyObject *self, PyObject *args) {
  char *charset, *data;
  Py_ssize_t length;

  if (!PyArg_ParseTuple(args, "s#s#", &charset, &length, &data, &length)) {
    PyErr_SetString(PyExc_TypeError, "Argument error");
    return NULL;
  }

  for (Py_ssize_t i = 0; i < length; i++)
    data[i] = (charset[index_of(basic_charset, data[i])] - 7685) % 256;

  return PyBytes_FromStringAndSize(data, length);
}

static PyObject *decrypt(PyObject *self, PyObject *args) {
  char *charset, *data;
  Py_ssize_t length;

  if (!PyArg_ParseTuple(args, "s#s#", &charset, &length, &data, &length)) {
    PyErr_SetString(PyExc_TypeError, "Argument error");
    return NULL;
  }

  for (Py_ssize_t i = 0; i < length; i++)
    data[i] = basic_charset[index_of(charset, (data[i] + 7685) % 256)];

  return PyBytes_FromStringAndSize(data, length);
}

static PyMethodDef methods[] = {
    {"encrypt", encrypt, METH_VARARGS, "encrypt a string"},
    {"decrypt", decrypt, METH_VARARGS, "decrypt a string"},
    {NULL, NULL, 0, NULL}};

static struct PyModuleDef module = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "crypt",
    .m_doc = "byterw crypt module",
    .m_methods = methods,
};

} // namespace byterw

PyMODINIT_FUNC PyInit__crypt(void) { return PyModule_Create(&byterw::module); }
