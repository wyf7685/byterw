#include "common.h"

namespace byterw::crypt {

long long parse_key(PyObject *);

PyObject *encrypt(PyObject *bytes, long long key);
PyObject *decrypt(PyObject *bytes, long long key);

} // namespace byterw::crypt
