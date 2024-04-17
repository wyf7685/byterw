#define PY_SSIZE_T_CLEAN
#include <Python.h>

const char *_basic_charset =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

int _index_of(const char *str, const char ch) {
  const char *p = str;
  while (*p != ch)
    p++;
  return p - str;
}

static PyObject *encrypt(PyObject *self, PyObject *args) {
  char *charset, *data;
  Py_ssize_t length;

  if (!PyArg_ParseTuple(args, "s#s#", &charset, &length, &data, &length)) {
    return NULL;
  }

  for (Py_ssize_t i = 0; i < length; i++)
    data[i] = (charset[_index_of(_basic_charset, data[i])] - 7685) % 256;
  PyObject *bytes_obj = PyBytes_FromStringAndSize(data, length);
  return bytes_obj;
}

static PyObject *decrypt(PyObject *self, PyObject *args) {
  char *charset, *data;
  Py_ssize_t length;

  if (!PyArg_ParseTuple(args, "s#s#", &charset, &length, &data, &length)) {
    return NULL;
  }

  for (Py_ssize_t i = 0; i < length; i++)
    data[i] = _basic_charset[_index_of(charset, (data[i] + 7685) % 256)];

  PyObject *bytes_obj = PyBytes_FromStringAndSize(data, length);
  return bytes_obj;
}

static PyMethodDef methods[] = {
    {"encrypt", encrypt, METH_VARARGS, "encrypt a string"},
    {"decrypt", decrypt, METH_VARARGS, "decrypt a string"},
    {NULL, NULL, 0, NULL}};

static struct PyModuleDef crypt_module = {
    PyModuleDef_HEAD_INIT,
    .m_name = "crypt",
    .m_doc = "byterw crypt module",
    .m_methods = methods,
};

PyMODINIT_FUNC PyInit__crypt(void) { return PyModule_Create(&crypt_module); }
