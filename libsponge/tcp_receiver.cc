#include "tcp_receiver.hh"

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;
//tcp断=tcp头+数据
void TCPReceiver::segment_received(const TCPSegment &seg) {
//    DUMMY_CODE(seg);
    //如果是空,或者SYN不存在,说明没有建立连接
    if(init_seq_peer==nullopt&&!seg.header().syn){
        return ;
    }
    //设置isn
    if(seg.header().syn){
        init_seq_peer=seg.header().seqno;

    }
//    将任何数据或流结束标记推到StreamReassembler
    if(init_seq_peer!=nullopt){
        //!有isn就可以写入数据,数据再payload
        auto buffer=seg.payload().copy();
        bool eof=seg.header().fin;
//        还是通过checkpoint来读取
        uint64_t index= unwrap(seg.header().seqno+static_cast<int>(seg.header().syn),init_seq_peer.value(),checkpoint);
        _reassembler.push_substring(buffer,index-1,eof);
        //abs是从0开始

    }
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if (init_seq_peer==nullopt){
        return  nullopt;
    }
    //返回ack
    uint64_t written=_reassembler.stream_out().bytes_written()+1;
    if(_reassembler.stream_out().input_ended()){
        written++;
    }
    return wrap(written,init_seq_peer.value());
}

size_t TCPReceiver::window_size() const { return _capacity-_reassembler.stream_out().buffer_size(); }
