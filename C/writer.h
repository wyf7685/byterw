#pragma once

#include "buffer.h"
#include "common.h"

namespace byterw::writer::inner {

class BWriter {
public:
  Buffer buffer;
  BWriter() { buffer = Buffer(); }

  inline const unsigned char *get() const noexcept { return buffer.get(); }
  inline std::size_t size() const noexcept { return buffer.size(); }

  void write_sign(ValueType vt);
  BYTERW_NO_DISCARD bool write_none();
  BYTERW_NO_DISCARD bool write_long(PyObject *value);
  BYTERW_NO_DISCARD bool write_long(long long value);
  BYTERW_NO_DISCARD bool write_double(double value, int precision = 8);
  BYTERW_NO_DISCARD bool write_double(PyObject *value, int precision = 8);
  BYTERW_NO_DISCARD bool write_bool(bool value);
  BYTERW_NO_DISCARD bool write_bool(PyObject *value);
  BYTERW_NO_DISCARD bool write_bytes(const char *value, Py_ssize_t length);
  BYTERW_NO_DISCARD bool write_string(PyObject *value);
  BYTERW_NO_DISCARD bool write_bytes(PyObject *value);
  BYTERW_NO_DISCARD bool write_dict(PyObject *value);
  BYTERW_NO_DISCARD bool write_list(PyObject *value);
  BYTERW_NO_DISCARD bool write_set(PyObject *value);
  BYTERW_NO_DISCARD bool write_tuple(PyObject *value);
  BYTERW_NO_DISCARD bool write_datetime(PyObject *value);
  BYTERW_NO_DISCARD bool write_path(PyObject *value);
  BYTERW_NO_DISCARD bool write_model(PyObject *value);
  BYTERW_NO_DISCARD bool write_object(PyObject *obj, bool with_sign = false);
};

ValueType get_vt(PyObject *obj);

} // namespace byterw::writer::inner
