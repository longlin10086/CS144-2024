#include "byte_stream.hh"
#include <cstdint>
#include <sys/types.h>

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) {}

bool Writer::is_closed() const
{
  // Your code here.
  return is_closed_;
}

void Writer::push( string data )
{
  // Your code here.
  if (has_error() or is_closed() or data.empty()) {
    return;
  }

  uint64_t push_length = min(data.length(), available_capacity());
  data = data.substr(0, push_length);
  cache_.push(move(data));
  cache_view_.emplace(cache_.back().c_str(), push_length);
  buffered_byte_count_ += push_length;
  pushed_byte_count_ += push_length;

  return;
}

void Writer::close()
{
  // Your code here.
  is_closed_ = true;
}

uint64_t Writer::available_capacity() const
{
  // Your code here.
  uint64_t available_capacity = capacity_ - buffered_byte_count_;
  return available_capacity;
}

uint64_t Writer::bytes_pushed() const
{
  // Your code here.
  return pushed_byte_count_;
}

bool Reader::is_finished() const
{
  // Your code here.
  return is_closed_ and cache_view_.empty();
}

uint64_t Reader::bytes_popped() const
{
  // Your code here.
  return poped_byte_count_;
}

string_view Reader::peek() const
{
  // Your code here.
  if (cache_view_.empty()) {
    return {};
  }
  return cache_view_.front();
}

void Reader::pop( uint64_t len )
{
  // Your code here.
  if (len == 0 or cache_view_.empty() or has_error()) {
    return;
  }

  uint64_t poped_length = min(len, bytes_buffered());
  uint64_t front_length = cache_view_.front().size();
  if (poped_length < front_length) {
    cache_view_.front().remove_prefix(poped_length);
  } else {
    uint64_t pop = poped_length;
    while (pop >= front_length and !cache_view_.empty()) {
      pop -= front_length;
      cache_view_.pop();
      front_length = cache_view_.front().size();
    }
    cache_view_.front().remove_prefix(pop);
  }

  // uint64_t poped_length = min(len, bytes_buffered());
  // uint64_t pop = poped_length;
  // uint64_t front_length {};
  // while (pop > 0 and !cache_view_.empty()) {
  //   front_length = cache_view_.front().size();
  //   if (pop < front_length) {
  //     cache_view_.front().remove_prefix(pop);
  //     break;
  //   } else {
  //     pop -= front_length;
  //     cache_view_.pop();
  //   }
  // }

  buffered_byte_count_ -= poped_length;
  poped_byte_count_ += poped_length;
  
}

uint64_t Reader::bytes_buffered() const
{
  // Your code here.
  return buffered_byte_count_;
}