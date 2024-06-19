#include "buffer.h"
#include "common.h"

#include <cstddef>
#include <string>

namespace byterw {

void Buffer::append(elem c) { buf.emplace_back(c); }

void Buffer::extend(const elem *str, size_t length) {
  const elem *p = str;
  while (static_cast<size_t>(p - str) < length)
    append(*p++);
}

void Buffer::extend(const string &str) {
  for (const char &c : str)
    append(c);
}

void Buffer::extend(const Buffer &other) { extend(other.get(), other.size()); }

const Buffer ::elem *Buffer::get() const noexcept {
  elem *str = new elem[buf.size()];
  elem *p = str;
  for (const elem &c : buf)
    *p++ = c;
  return str;
}

} // namespace byterw
