// #include "tcp_connection.hh"

// #include <iostream>

// // Dummy implementation of a TCP connection

// // For Lab 4, please replace with a real implementation that passes the
// // automated checks run by `make check`.

// template <typename... Targs>
// void DUMMY_CODE(Targs &&... /* unused */) {}

// using namespace std;

// size_t TCPConnection::remaining_outbound_capacity() const { return _sender.stream_in().remaining_capacity(); }

// size_t TCPConnection::bytes_in_flight() const { return _sender.bytes_in_flight(); }

// size_t TCPConnection::unassembled_bytes() const { return _receiver.unassembled_bytes(); }

// size_t TCPConnection::time_since_last_segment_received() const { return time_since_last_segment_received_; }

// void TCPConnection::segment_received(const TCPSegment &seg) { //DUMMY_CODE(seg);
//     // 具体实现
//     if (active_)
//     {
//         /* code */
//         return;
//     }
//     // 接受就开始重开定时器
//     time_since_last_segment_received_=0;
//     auto header =seg.header();
//     if(!_receiver.ackno().has_value()&& _sender.next_seqno_absolute() == 0){
//         // 开始做为服务端server，收到的消息
//         // 没有syn直接返回
//          if(!header.syn) {
//             return;
//         }
//         // 第一次握手情况，发送connect
//         // 放到接受received
//         _receiver.segment_received(seg);
//         // 收到之后就开始server发送连接connect
//         connect();
//         return;
//     }
//     // 收到rst，直接结束
//     if (header.rst)
//     {
//         /* code */
//         // todo:非正常关闭
//         unclean_shutdown();
//         return;
//     }
//     // 进行接受到receiver
//     _receiver.segment_received(seg);

//     // 作为客户机，没有收到回复直接return
//     if(!_receiver.ackno().has_value())return;

//     if(!_sender.stream_in().eof()&&_receiver.stream_out().input_ended()){
//         _linger_after_streams_finish=false;
//         // 当发送还有接受都接受了，这个就该结束
//     }
//     // 收到信息之后就是更新自己的窗口
//     if(header.ack){
//         // 让发生器，进行ack检验
//         _sender.ack_received(header.ackno,header.win);
//     }
//     // 校验成功，接着发送
//     _sender.fill_window();
    
//     if (seg.length_in_sequence_space()>0&&_sender.segments_out().empty())
//     {
//         /* code */
//         // 发送空的，seg有len，sender没东西
//         _sender.send_empty_segment();
//     }

//      // an extra special case to respond to a keep-alive segment
//     if (seg.length_in_sequence_space() == 0 && header.seqno == _receiver.ackno().value() - 1) {
//         _sender.send_empty_segment();
//     }
//     // 最后开始发送
//     send_data();
    
    
//  }

// bool TCPConnection::active() const { return active_; }

// size_t TCPConnection::write(const string &data) {
//     // DUMMY_CODE(data);
//     // 开始写入
//     if (data.empty())
//     {
//         /* code */
//         return 0;
//     }
//     // 进行计算
//     auto size =_sender.stream_in().write(data);
//     // 开始发送
//     _sender.fill_window();
//     send_data();
//     return size;
// }

// //! \param[in] ms_since_last_tick number of milliseconds since the last call to this method
// void TCPConnection::tick(const size_t ms_since_last_tick) { 
    
//     // DUMMY_CODE(ms_since_last_tick); 
//     // 传入trick，给sender    // 
//     if (!active_)
//     {
//         /* code */
//         return;
//     }
//     // 加上时间
//     time_since_last_segment_received_+=ms_since_last_tick;
//     // 开定时器，sender
//     _sender.tick(ms_since_last_tick);
//     // 重试太多，加入rst
//     if(_sender.consecutive_retransmissions()>TCPConfig::MAX_RETX_ATTEMPTS){
//         send_RST();
//         return;
//     }
//     // 正常关闭
//      // TCP clean shutdown if necessary
//     if (_receiver.stream_out().input_ended() && _sender.stream_in().eof() && _sender.bytes_in_flight() == 0 &&
//         (!_linger_after_streams_finish || time_since_last_segment_received_ >= 10 * _cfg.rt_timeout)) {
//         active_ = false;
//         return;
//     }
//     // send segments when `_sender.tick()` has a retransmission
//     send_data();
    
// }

// void TCPConnection::end_input_stream() {
//     // sender结束
//     // _sender
//     _sender.stream_in().end_input();
//     // 发送
//     _sender.fill_window();
//     send_data();
// }

// void TCPConnection::connect() {
//     // 开始建立练习
//     _sender.fill_window();
//     // 这个发送syn
//     // 调用发上去，发送sender的数据
//     send_data();
// }

// TCPConnection::~TCPConnection() {
//     try {
//         if (active()) {
//             cerr << "Warning: Unclean shutdown of TCPConnection\n";

//             // Your code here: need to send a RST segment to the peer
//         }
//     } catch (const exception &e) {
//         std::cerr << "Exception destructing TCP FSM: " << e.what() << std::endl;
//     }
// }
// void TCPConnection::send_data() {

//     // // 从sender里面读取数据进行发送
//     // while (!_sender.segments_out().empty())
//     // {
//     //     /* code */
//     //     // 开头进行截取
//     //     auto seg = _sender.segments_out().front();
//     //     _sender.segments_out().pop();
//     //     // 放入到connection里面
//     //     // 设置ack，如果自己接受的seg附带，就需要进行设置
//     //     if (_receiver.ackno().has_value())
//     //     {
//     //         /* code */
//     //         // 如果有ack，就进行设置
//     //         seg.header().ack=true;
//     //         seg.header().ackno=_receiver.ackno();
//     //         // 窗口设置
//     //         seg.header().win=_receiver.window_size();
//     //     }
//     //     _segments_out.push(seg);
//     //     // 放入发送的队列里面
        
//     // }
//     // if(_receiver.stream_out().input_ended()){
//     //     // 接受完成
//     //     // 挥手阶段，断开不用一直连接
//     //     if (!_sender.stream_in().eof())
//     //     {
//     //         /* code */_linger_after_streams_finish=false;
//     //     }else if(_sender.bytes_in_flight()==0){
//     //         // 没有需要发送的直接
//     //         if (!_linger_after_streams_finish || time_since_last_segment_received() >= 10 * _cfg.rt_timeout) {
//     //            active_ = false;
//     //        }
//     //     }
        

//     // }
//      while (!_sender.segments_out().empty()) {
//         TCPSegment segment = _sender.segments_out().front();
//         _sender.segments_out().pop();
//         optional<WrappingInt32> ackno = _receiver.ackno();
//         // if TCP does not receive SYN segments from the remote peer, i.e. at SYN_SENT state
//         // TCP will not set ACK flag and seqno
//         if (ackno.has_value()) {
//             // 进行填充，然后发送
//             segment.header().ack = true;
//             segment.header().ackno = ackno.value();
//         }
//         // set the local receiver's window size
//         segment.header().win = _receiver.window_size() <= numeric_limits<uint16_t>::max()
//                                    ? _receiver.window_size()
//                                    : numeric_limits<uint16_t>::max();
//         _segments_out.emplace(segment);
//     }
    
// }

// // 非正常关闭
//      inline void TCPConnection::unclean_shutdown() {
//              _sender.stream_in().set_error();
//             _receiver.stream_out().set_error();
//             active_ = false;
// }


// void TCPConnection::send_RST() {
//     // 从发送队列拿去加入rst
//     _sender.fill_window();
//      if (_sender.segments_out().empty()) {
//         _sender.send_empty_segment();
//     }
//     auto seg = _sender.segments_out().front();
//     _sender.segments_out().pop();

//     optional<WrappingInt32> ackno = _receiver.ackno();
//     if (ackno.has_value()) {
//         seg.header().ack = true;
//         seg.header().ackno = ackno.value();
//     }
//     seg.header().win = _receiver.window_size() <= numeric_limits<uint16_t>::max()
//                                ? _receiver.window_size()
//                                : numeric_limits<uint16_t>::max();

//     // 之前一样的进行ack封装，但是加入rst
//        seg.header().rst = true;
//     _segments_out.emplace(seg);

//     unclean_shutdown();
// }
#include "tcp_connection.hh"

#include <iostream>

// Dummy implementation of a TCP connection

// For Lab 4, please replace with a real implementation that passes the
// automated checks run by `make check`.

template <typename... Targs>
void DUMMY_CODE(Targs &&.../* unused */) {}

using namespace std;

size_t TCPConnection::remaining_outbound_capacity() const { return _sender.stream_in().remaining_capacity(); }

size_t TCPConnection::bytes_in_flight() const { return _sender.bytes_in_flight(); }

size_t TCPConnection::unassembled_bytes() const { return _receiver.unassembled_bytes(); }

size_t TCPConnection::time_since_last_segment_received() const { return _time_since_last_segment_received; }

void TCPConnection::segment_received(const TCPSegment &seg) {
    if (!_active) {
        return;
    }
    // reset the timer
    _time_since_last_segment_received = 0;
    const TCPHeader &header = seg.header();
    // if TCP does not receive SYN from remote peer, and not send SYN to remote peer
    if (!_receiver.ackno().has_value() && _sender.next_seqno_absolute() == 0) {
        // at this time, TCP acts as a server,
        // and should not receive any segment except it has SYN flag
        if (!header.syn) {
            return;
        }
        _receiver.segment_received(seg);
        // try to send SYN segment, use for opening TCP at the same time
        connect();
        return;
    }
    // the TCP ends in an unclean shutdown when receiving RST segment
    if (header.rst) {
        unclean_shutdown();
        return;
    }
    _receiver.segment_received(seg);
    // if TCP sends SYN segment as a client but does not receive SYN from remote peer,
    // the local TCP should not handle it, too.
    if (!_receiver.ackno().has_value()) {
        return;
    }

    // set the `_linger_after_streams_finish` the first time the inbound stream ends
    if (!_sender.stream_in().eof() && _receiver.stream_out().input_ended()) {
        _linger_after_streams_finish = false;
    }
    // use the remote peer's ackno and window size to update the local sending window
    if (header.ack) {
        _sender.ack_received(header.ackno, header.win);
    }

    _sender.fill_window();
    // makes sure that at least one segment is sent in reply
    if (seg.length_in_sequence_space() > 0 && _sender.segments_out().empty()) {
        _sender.send_empty_segment();
    }
    // an extra special case to respond to a keep-alive segment
    if (seg.length_in_sequence_space() == 0 && header.seqno == _receiver.ackno().value() - 1) {
        _sender.send_empty_segment();
    }
    send_segments();
}

bool TCPConnection::active() const { return _active; }

size_t TCPConnection::write(const string &data) {
    if (!_active) {
        return 0;
    }
    size_t ret = _sender.stream_in().write(data);
    _sender.fill_window();
    send_segments();
    return ret;
}

//! \param[in] ms_since_last_tick number of milliseconds since the last call to this method
void TCPConnection::tick(const size_t ms_since_last_tick) {
    if (!_active) {
        return;
    }
    _time_since_last_segment_received += ms_since_last_tick;
    _sender.tick(ms_since_last_tick);
    // TCP unclean shutdown if the number of consecutive retransmissions
    // is more than an upper limit
    if (_sender.consecutive_retransmissions() > TCPConfig::MAX_RETX_ATTEMPTS) {
        send_RST();
        return;
    }
    // TCP clean shutdown if necessary
    if (_receiver.stream_out().input_ended() && _sender.stream_in().eof() && _sender.bytes_in_flight() == 0 &&
        (!_linger_after_streams_finish || _time_since_last_segment_received >= 10 * _cfg.rt_timeout)) {
        _active = false;
        return;
    }
    // send segments when `_sender.tick()` has a retransmission
    send_segments();
}

void TCPConnection::end_input_stream() {
    _sender.stream_in().end_input();
    _sender.fill_window();
    send_segments();
}

void TCPConnection::connect() {
    _sender.fill_window();
    send_segments();
}

TCPConnection::~TCPConnection() {
    try {
        if (active()) {
            cerr << "Warning: Unclean shutdown of TCPConnection\n";

            // Your code here: need to send a RST segment to the peer
            send_RST();
        }
    } catch (const exception &e) {
        std::cerr << "Exception destructing TCP FSM: " << e.what() << std::endl;
    }
}

void TCPConnection::send_segments() {
    while (!_sender.segments_out().empty()) {
        TCPSegment segment = _sender.segments_out().front();
        _sender.segments_out().pop();
        optional<WrappingInt32> ackno = _receiver.ackno();
        // if TCP does not receive SYN segments from the remote peer, i.e. at SYN_SENT state
        // TCP will not set ACK flag and seqno
        if (ackno.has_value()) {
            segment.header().ack = true;
            segment.header().ackno = ackno.value();
        }
        // set the local receiver's window size
        segment.header().win = _receiver.window_size() <= numeric_limits<uint16_t>::max()
                                   ? _receiver.window_size()
                                   : numeric_limits<uint16_t>::max();
        _segments_out.emplace(segment);
    }
}

void TCPConnection::send_RST() {
    _sender.fill_window();
    if (_sender.segments_out().empty()) {
        _sender.send_empty_segment();
    }
    TCPSegment segment = _sender.segments_out().front();
    _sender.segments_out().pop();
    optional<WrappingInt32> ackno = _receiver.ackno();
    if (ackno.has_value()) {
        segment.header().ack = true;
        segment.header().ackno = ackno.value();
    }
    segment.header().win = _receiver.window_size() <= numeric_limits<uint16_t>::max() ? _receiver.window_size()
                                                                                      : numeric_limits<uint16_t>::max();
    segment.header().rst = true;
    _segments_out.emplace(segment);

    unclean_shutdown();
}

inline void TCPConnection::unclean_shutdown() {
    _sender.stream_in().set_error();
    _receiver.stream_out().set_error();
    _active = false;
}
