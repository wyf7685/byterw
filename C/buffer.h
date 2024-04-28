#pragma once

#include <string>
#include <vector>

namespace byterw {

class Buffer {
public:
  std::vector<unsigned char> buf;

  Buffer() = default;
  ~Buffer() = default;

  void append(unsigned char c);
  void extend(const unsigned char *str, std::size_t length);
  void extend(const std::string &str);
  void extend(const Buffer &other);
  const unsigned char *get() const noexcept;
  std::size_t size() const noexcept { return buf.size(); };
};

} // namespace byterw
