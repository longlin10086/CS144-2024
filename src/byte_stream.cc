#include "byte_stream.hh"
#include <string_view>

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
  cache_ += data.substr(0, push_length);
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
  uint64_t available_capacity = capacity_ - cache_.size();
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
  return is_closed_ and cache_.empty();
}

uint64_t Reader::bytes_popped() const
{
  // Your code here.
  return poped_byte_count_;
}

string_view Reader::peek() const
{
  // Your code here.

  return this->cache_;
}

void Reader::pop( uint64_t len )
{
  // Your code here.
  if (len == 0) {
    return;
  }

  if (!has_error()) {
    uint64_t poped_length = min(len, bytes_buffered());

    cache_.erase(0, poped_length);
    poped_byte_count_ += poped_length;
  }

}

uint64_t Reader::bytes_buffered() const
{
  // Your code here.
  return cache_.size();
}