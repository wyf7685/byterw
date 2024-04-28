#pragma once

#include "buffer.h"
#include "common.h"
#include "pytypedefs.h"
#include <cstddef>

namespace byterw::reader::inner {

class BReader {
public:
  unsigned char *buffer;
  std::size_t size;
  unsigned char *ptr;

  BReader(const unsigned char *buf, std::size_t bufSize);
  ~BReader();

  inline bool readable() const noexcept { return ptr - buffer < size; }
  ValueType read_sign();
  PyObject *read_none();
  PyObject *read_long();
  PyObject *read_double();
  PyObject *read_bool();
  PyObject *read_string();
  PyObject *read_bytes();
  PyObject *read_dict();
  PyObject *read_list();
  PyObject *read_set();
  PyObject *read_datetime();
  PyObject *read_path();
  PyObject *read_model();
  PyObject *read_object(ValueType vt);

private:
  inline unsigned char read_single_byte() { return *ptr++; };

protected:
  Py_ssize_t read_struct_length();
};

} // namespace byterw::reader::inner
