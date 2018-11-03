
#pragma once

#include <cstdint>

namespace tsar::detail {
class packed_ptr {
 public:
  static const constexpr size_t pointer_bits = 51;
  static const constexpr uint64_t ptr_bitmask = (1ll << 51) - 1;
  static const constexpr uint64_t data_bitmask = ~ptr_bitmask;

  packed_ptr() {}

  packed_ptr(void* ptr, uint64_t data)
      : data_(((data << pointer_bits) & data_bitmask) | (reinterpret_cast<uintptr_t>(ptr) & ptr_bitmask)) {}

  void* ptr() const { return reinterpret_cast<void*>(data_ & ptr_bitmask); }
  void save_ptr(void* ptr) { data_ = (data_ & data_bitmask) | (reinterpret_cast<uintptr_t>(ptr) & ptr_bitmask); }

  uint16_t data() const { return (data_ & data_bitmask) >> pointer_bits; }

  void save_data(uint64_t data) { data_ = ((data << pointer_bits) & data_bitmask) | (data_ & ptr_bitmask); }

  bool operator==(packed_ptr const& other) const { return other.data_ == data_; }

 private:
  uint64_t data_{};
};
}  // namespace tsar::detail