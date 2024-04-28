#pragma once

#include "common.h"

namespace byterw::writer {

static PyTypeObject TypeObject = {
    .ob_base = {{{1}, (0)}, (0)},
    .tp_name = "byterw.ByteWriter",
};

void init(PyTypeObject *);

} // namespace byterw::writer
