#include "reassembler.hh"
#include <algorithm>
#include <cstdint>
#include <sys/types.h>


using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  // Your code here.
  if (data.empty()) {
    if (is_last_substring) output_.writer().close();
    return;
  }


  uint64_t next_index = first_index + data.length();
  first_unacceptable_index_ = first_unassembled_index_ + output_.writer().available_capacity();

  if (next_index <= first_unassembled_index_ or first_index >= first_unacceptable_index_) {
    return;
  }

  
  if (first_index < first_unassembled_index_) {
    uint64_t head = first_unassembled_index_ - first_index;
    data = data.substr(head);
    first_index = first_unassembled_index_;
  }
  if (next_index > first_unacceptable_index_) {
    uint64_t tail = first_unacceptable_index_ - first_index;
    data = data.substr(0, tail);
    next_index = first_unacceptable_index_;
    is_last_substring = false;
  }

  if (is_last_substring) {
    has_last_substring = true;
  }

  uint64_t head_index {};
  uint64_t tail_index {};

  sort(container_.begin(), container_.end());
  auto head_it = std::lower_bound(container_.begin(), container_.end(), first_index,
                                [](const std::pair<uint64_t, std::string>& pair, uint64_t index) { return pair.first < index; });
  auto tail_it = std::lower_bound(container_.begin(), container_.end(), next_index,
                                [](const std::pair<uint64_t, std::string>& pair, uint64_t index) { return pair.first < index; });
  if (head_it != container_.begin()) {
    head_it --;
  }
  auto current_map = head_it;
  while (current_map != tail_it) {
    if (current_map == container_.end()) {
      break;
    }
    head_index = current_map->first;
    if (current_map->second.empty()) {
      current_map = container_.erase(current_map);
      continue;
    }
    tail_index = head_index + current_map->second.length();

    if (tail_index < first_index) {
      current_map ++;
    }
    else if (head_index < first_index and tail_index >= first_index and tail_index <= next_index) {
      uint64_t head = tail_index - first_index;
      data = data.substr(head);
      first_index = first_index + head;
      current_map ++;
    }
    else if (head_index < first_index and tail_index > next_index) {
      return;
    }
    else if (head_index >= first_index and tail_index <= next_index) {
      pending_ -= current_map->second.length();
      current_map = container_.erase(current_map);
      continue;
    }
    else if (head_index >= first_index and tail_index > next_index) {
      uint64_t tail = head_index - first_index;
      data = data.substr(0, tail);
      next_index = first_index + tail;
      current_map ++;
    }
    else if (head_index > next_index) {
      break;
    }
  }

  if (first_index == first_unassembled_index_) {
    first_unassembled_index_ = first_unassembled_index_ + data.length();
    output_.writer().push(data);
    sort(container_.begin(), container_.end());
    insert_from_container(first_unassembled_index_);
  } else {
    pending_ += data.length();
    container_.emplace(current_map, first_index, data);
  }

  if (has_last_substring and container_.empty()) {
    output_.writer().close();
  }
}

uint64_t Reassembler::bytes_pending() const
{
  // Your code here.
  return pending_;
}

void Reassembler::insert_from_container(uint64_t first_unassembled_index){
  if (container_.empty()) {
    return;
  }
  auto it = std::find_if(container_.begin(), container_.end(), [=](const std::pair<uint64_t, std::string>& pair){ return pair.first == first_unassembled_index; });
  if (it != container_.end()) {
      pending_ -= it->second.length();
      first_unassembled_index_ = first_unassembled_index + it->second.length();
      output_.writer().push(it->second);
      it = container_.erase(it);
      insert_from_container(first_unassembled_index_);
  }
}
