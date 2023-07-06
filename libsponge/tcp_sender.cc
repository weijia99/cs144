#include "tcp_sender.hh"

#include "tcp_config.hh"
#include "iostream"
#include <random>

// Dummy implementation of a TCP sender

// For Lab 3, please replace with a real implementation that passes the
// automated checks run by `make check_lab3`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

// 初始化timer
Timer::Timer(uint32_t rto):_rto(rto),_remain_time(rto),_is_running(false){}
// 实现上面的代码
void Timer::start(){
    // 打开running
    _is_running=true;
    _remain_time=_rto;
}

void Timer::stop(){
    // 关闭
    _is_running=false;
}
bool Timer::is_time_out(){
    return _remain_time==0;
}
void Timer::elapse(size_t elapsed){
    if(_remain_time<elapsed){
        _remain_time=0;
    }else{
        _remain_time-=elapsed;
    }
}
void Timer::set_time_out(uint32_t duration){
    // 更新rto
    _rto=duration;
    _remain_time=duration;
}


//! \param[in] capacity the capacity of the outgoing byte stream
//! \param[in] retx_timeout the initial amount of time to wait before retransmitting the oldest outstanding segment
//! \param[in] fixed_isn the Initial Sequence Number to use, if set (otherwise uses a random ISN)
TCPSender::TCPSender(const size_t capacity, const uint16_t retx_timeout, const std::optional<WrappingInt32> fixed_isn)
    : _isn(fixed_isn.value_or(WrappingInt32{random_device()()}))
    , _initial_retransmission_timeout{retx_timeout}
    , _stream(capacity),_timer(retx_timeout) {}

uint64_t TCPSender::bytes_in_flight() const { return _outstand_bytes; }

void TCPSender::fill_window() {
    // 发送文件
    if(!_is_syned){
        // 还有就不是这个原因
         if (!_outstand_segment.empty())
            return;
        //   构造syn发送,只有开始的时候采用syn
        send_segment("",true,false);

    }else{
        //开始来进行装配
        // 能放的只有窗口大小减去还没有确认的
        // 如果是0，就变成1
        size_t remain_size =max(_window_size,static_cast<uint16_t>(1))+_ack_seq-_next_seqno;
    
        // 使用while进行发送
        while (remain_size>0&&!_stream.buffer_empty())
        {
            /* code */
            // 从stream进行去数据来发送
            // 取得能发送的大小
            auto ws = min(min(remain_size, TCPConfig::MAX_PAYLOAD_SIZE), _stream.buffer_size());
            remain_size-=ws;
            auto data =_stream.peek_output(ws);
            _stream.pop_output(ws);
            // 弹出这么多的数据
            // 开始封装
            _is_fin|=(_stream.eof()&&!_is_fin&&remain_size>0);
            // 还能发送，但是没有了pack
            send_segment(move(data),false,_is_fin);

        }
        // 发送结束的fin握手，2次发送（4次挥手）
        if (_stream.eof() && !_is_fin && remain_size > 0) {
            _is_fin = true;
            send_segment("", false, true);
        }

    }
}

//! \param ackno The remote receiver's ackno (acknowledgment number)
//! \param window_size The remote receiver's advertised window size
void TCPSender::ack_received(const WrappingInt32 ackno, const uint16_t window_size) { 
    _window_size =window_size;
    

    
    // 更新窗口
    // 换成64位的
    auto ack_seq = unwrap(ackno,_isn,_next_seqno);
    if(ack_seq>_next_seqno)return;

    if(ack_seq<=_ack_seq)return;
    // 接受到的是已经处理的ack，直接返回,或者直接超过了next
    // 更新重传次数，回复初始，还有字节数
    _ack_seq = ack_seq;
    _is_syned=true;
    // 确认收到了
    // 重开定时器
    if(!_outstand_segment.empty())
    _timer.set_time_out(_initial_retransmission_timeout);
    
    _consecutive_retxs=0;
    // 接着进行发送
//    超时还有，就重新发送

    // 移除在未确认的队列元素
    while(!_outstand_segment.empty()){
        // 小于序列号的直接移除ack
        auto seg = _outstand_segment.front().first;
        auto index  = _outstand_segment.front().second;
        if(index>=ack_seq)break;
        // 移除直接还有元素
        _outstand_bytes-=seg.length_in_sequence_space();
        _outstand_segment.pop();
    }

    fill_window();
    if(!_outstand_segment.empty()){
        // 还有没有确认的，就打开计时器
        _timer.start();
    }else{
        _timer.stop();
        // 关闭计时器
    }

 }

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void TCPSender::tick(const size_t ms_since_last_tick) { 
    // 发送了的时间
    _timer.elapse(ms_since_last_tick);

    if(!_timer.is_time_out())return;
    // 超时了才进行处理
    if(_outstand_segment.empty()){
        // 没有在进行的
        // 那就设置为初始长度（因为不是发送的问题）
        return _timer.set_time_out(_initial_retransmission_timeout);
    }

         // 下面就是次数增加，然后翻倍
    _segments_out.push(_outstand_segment.front().first);
    // 发送函数，就是推入到seg里面，重发第一个
    // 次数增加
    if(_window_size>0){
        _consecutive_retxs++;

        _timer.set_time_out(_initial_retransmission_timeout*(1<<_consecutive_retxs));

    }
    // 打开
    _timer.start();

   


 }

// 返回重传就可以
unsigned int TCPSender::consecutive_retransmissions() const { return _consecutive_retxs; }

void TCPSender::send_empty_segment() {
    // 构造一个seg，然后发送
    TCPSegment seg;
    seg.header().seqno=next_seqno();
    // 发送
    _segments_out.push(seg);
}

void TCPSender::send_segment(string &&data, bool syn, bool fin) {
    TCPSegment seg;
    seg.header().syn = syn;
    seg.header().seqno= next_seqno();
    seg.header().fin =fin;
    seg.payload() =move(data);
    // 复制上去
    // 发送
    _segments_out.push(seg);
    // 放入到待确认队列，长度进行增加
    _outstand_segment.push({seg,_next_seqno});

    auto len =seg.length_in_sequence_space();
    // 得到长度
    _outstand_bytes+=len;
    _next_seqno+=len;
    // 更新下一个的长度，还有发送了的长度
    // 打开定时器
    
    
}