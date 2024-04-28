#pragma once

#include "common.h"

namespace byterw::reader {

static PyTypeObject TypeObject = {
    .ob_base = {{{1}, (0)}, (0)},
    .tp_name = "byterw.ByteReader",
};

void init(PyTypeObject *);

} // namespace byterw::reader
