#include "tcp_receiver.hh"
#include "byte_stream.hh"
#include "wrapping_integers.hh"
#include <cstdint>
#include <optional>

using namespace std;

void TCPReceiver::receive( TCPSenderMessage message )
{
  // Your code here.
  if (message.RST) {
    reader().set_error();
    return;
  }
  if (message.SYN) {
    ISN = std::optional<Wrap32>(message.seqno);
    ackno_header = std::optional<Wrap32>(ISN.value()+1);
  }
  if (!ISN.has_value()) {
    return;
  }
  uint64_t abs_seqno = message.seqno.unwrap(ISN.value(), writer().bytes_pushed()+1);
  uint64_t first_index = message.SYN ? 0: abs_seqno-1; 
  
  bool is_last_substring = false;
  if (message.FIN) {
    is_last_substring = true;
  }
  reassembler_.insert(first_index, message.payload, is_last_substring);
  if (writer().is_closed()) {
    ackno_header = std::optional<Wrap32>(ackno_header.value()+1);
  }
  ackno = std::optional<Wrap32>(ackno_header.value()+writer().bytes_pushed());
}

TCPReceiverMessage TCPReceiver::send() const
{
  // Your code here.
  bool RST = false;
  if (writer().has_error()) {
    RST = true;
  }
  uint16_t window_size = writer().available_capacity() > UINT16_MAX ? UINT16_MAX : writer().available_capacity();
  return {ackno, window_size, RST};
}
