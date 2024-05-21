#include "common.h"
#include "modsupport.h"
#include "object.h"
#include "pyreader.h"
#include "pytypedefs.h"
#include "pywriter.h"
#include "unicodeobject.h"

#include <functional>
#include <iostream>
#include <ostream>

namespace byterw::module {

static struct {
  const char *name;
  PyTypeObject *type;
  std::function<void(PyTypeObject *)> init;
} types[] = {
    {"ByteWriter", &writer::TypeObject, writer::init},
    {"ByteReader", &reader::TypeObject, reader::init},
};

static struct PyModuleDef module = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "_byterw",
    .m_doc = "byterw module",
};

} // namespace byterw::module

PyMODINIT_FUNC PyInit__byterw(void) {

  // Initialize type objects
  for (auto &t : byterw::module::types) {
    t.init(t.type);
    if (PyType_Ready(t.type) < 0)
      return nullptr;
  }

  PyObject *version_string = PyUnicode_FromString(byterw::VERSION.c_str());
  if (!version_string)
    return nullptr;

  // Initial module object
  PyObject *module = PyModule_Create(&byterw::module::module);
  if (!module)
    return nullptr;

  // Export type objects
  for (auto &t : byterw::module::types) {
    Py_INCREF(t.type);
    PyModule_AddObject(module, t.name, (PyObject *)t.type);
  }

  Py_INCREF(version_string);
  PyModule_AddObject(module, "__version__", version_string);

  return module;
}
