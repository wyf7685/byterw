#include "common.h"
#include "pyreader.h"
#include "pywriter.h"

#include <functional>

namespace byterw {

static struct {
  const char *name;
  PyTypeObject *type;
  std::function<void(PyTypeObject *)> init;
} Types[] = {
    {"ByteWriter", &writer::TypeObject, writer::init},
    {"ByteReader", &reader::TypeObject, reader::init},
};

static PyMethodDef methods[] = {
    {nullptr, nullptr, 0, nullptr}};

static struct PyModuleDef module = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "_byterw",
    .m_doc = "byterw module",
    .m_methods = methods,
};

} // namespace byterw

PyMODINIT_FUNC PyInit__byterw(void) {
  for (auto &t : byterw::Types) {
    t.init(t.type);
    if (PyType_Ready(t.type) < 0)
      return nullptr;
  }

  PyObject *module = PyModule_Create(&byterw::module);
  if (!module)
    return nullptr;

  for (auto &t : byterw::Types) {
    Py_INCREF(t.type);
    PyModule_AddObject(module, t.name, (PyObject *)t.type);
  }

  return module;
}