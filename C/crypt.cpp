#include "crypt.h"
#include "pyobj.h"

#include <algorithm>
#include <random>
#include <unordered_map>

namespace byterw::crypt {

long long parse_key(PyObject *obj) {
  auto unicode = PyUnicode_FromObject(obj);

  Py_ssize_t size = -1;
  const char *buf = PyUnicode_AsUTF8AndSize(unicode, &size);
  if (size == -1) {
    return 0;
  }

  long long result = 0;
  for (Py_ssize_t i = 0; i < size; i++) {
    result = (result * 7685 + buf[i]) % 5198892651;
  }

  Py_XDECREF(unicode);
  return result;
}

static const string basic_charset =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

static string get_charset(long long key) noexcept {
  std::mt19937 gen(key);
  string charset{basic_charset.substr(0, 64)};
  std::shuffle(charset.begin(), charset.end(), gen);
  return charset + '=';
}

PyObject *encrypt(PyObject *bytes, long long key) {
  auto b64encode = pyobj::load(pyobj::b64encode);
  auto b64 = PyObject_CallOneArg(b64encode, bytes);

  char *buffer;
  Py_ssize_t length;
  PyBytes_AsStringAndSize(b64, &buffer, &length);

  auto charset = get_charset(key);
  char *data = new char[length + 1];
  for (Py_ssize_t i = 0; i < length; i++)
    data[i] = (charset[basic_charset.find(buffer[i])] - 7685) % 256;

  auto pybytes = PyBytes_FromStringAndSize(data, length);
  delete[] data;
  return pybytes;
}

PyObject *decrypt(PyObject *bytes, long long key) {
  char *buffer;
  Py_ssize_t length;
  PyBytes_AsStringAndSize(bytes, &buffer, &length);

  auto charset = get_charset(key);
  char *data = new char[length + 1];
  for (Py_ssize_t i = 0; i < length; i++)
    data[i] = basic_charset[charset.find((buffer[i] + 7685) % 256)];

  auto b64 = PyBytes_FromStringAndSize(data, length);
  delete[] data;
  return PyObject_CallOneArg(pyobj::load(pyobj::b64decode), b64);
}

} // namespace byterw::crypt
