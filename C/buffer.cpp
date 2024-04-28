#include "buffer.h"
#include "common.h"

#include <string>

namespace byterw {

void Buffer::append(unsigned char c) { buf.emplace_back(c); }

void Buffer::extend(const unsigned char *str, size_t length) {
  const unsigned char *p = str;
  while (p - str < length)
    append(*p++);
}

void Buffer::extend(const string &str) {
  for (const char &c : str)
    append(c);
}

void Buffer::extend(const Buffer &other) { extend(other.get(), other.size()); }

const unsigned char *Buffer::get() const noexcept {
  unsigned char *str = new unsigned char[buf.size()];
  unsigned char *p = str;
  for (const char &c : buf)
    *p++ = c;
  return str;
}

} // namespace byterw
