#include "wrapping_integers.hh"
#include <cmath>
#include <cstdint>

using namespace std;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
  // Your code here.
  const uint32_t mol_ = n % (1UL << 32);
  return zero_point + mol_;
}

uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
  // Your code here.
  const uint64_t seqno_offset = this->raw_value_ - zero_point.raw_value_;
  if (checkpoint > seqno_offset) {
    constexpr uint64_t Range32 = 1UL << 32;
    // 四舍五入
    const uint64_t abs_check_seq = checkpoint - seqno_offset + (Range32 >> 1);
    const uint64_t times = abs_check_seq / Range32;
    return times * Range32 + seqno_offset;
  }
  return seqno_offset;
}
