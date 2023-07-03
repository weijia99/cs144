#include "tcp_receiver.hh"

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;
//tcp断=tcp头+数据
void TCPReceiver::segment_received(const TCPSegment &seg) {
// 这个题目的意识就是要求我们把这个seg代码来进行写入到我们的接收器里面
// 首先没有isn，就直接失败
    if(!_set_syn_flag){
        if(!seg.header().syn)return;
        // 没有开头直接就是失败
        _set_syn_flag=true;
        _isn=seg.header().seqno;
        // 序列化在头文件里面
    }
    // 接下来我们就是来进行计算seq转化为绝对值，因为seq是32，所以需要我们上面的unwrap
    // 得到相对值checkpoint
    uint64_t checkpoint =_reassembler.stream_out().bytes_written()+1;
    // 已经写入了的，作为相对值，这个是在stream里面的，已经排好序的，可以作为相对值
    uint64_t convert_seq = unwrap(seg.header().seqno,_isn,checkpoint);

    // 这个知识相对值，我们的stream index
    // 要在上面进行减少1
    auto stream_index = convert_seq -1 +(seg.header().syn);
    // 接下来是写入，调用之前的代码
    _reassembler.push_substring(seg.payload().copy(),stream_index,seg.header().fin);
}

optional<WrappingInt32> TCPReceiver::ackno() const {
//    返回ack
    if(!_set_syn_flag){
        return nullopt;
    }
    // 计算下一个，就是相当于之前的next——index，只需要求出
     uint64_t abs_ack_no = _reassembler.stream_out().bytes_written() + 1;
    // 如果当前处于 FIN_RECV 状态，则还需要加上 FIN 标志长度
    if (_reassembler.stream_out().input_ended())
        ++abs_ack_no;
    return WrappingInt32(_isn) + abs_ack_no;
}

size_t TCPReceiver::window_size() const { return _capacity-_reassembler.stream_out().buffer_size(); }
