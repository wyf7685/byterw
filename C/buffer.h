#pragma once

#include <string>
#include <vector>

namespace byterw {

class Buffer {
public:
  using elem = unsigned char;

  std::vector<elem> buf;

  Buffer() = default;
  ~Buffer() = default;

  void append(elem c);
  void extend(const elem *str, std::size_t length);
  void extend(const std::string &str);
  void extend(const Buffer &other);
  const elem *get() const noexcept;
  std::size_t size() const noexcept { return buf.size(); };
};

} // namespace byterw
