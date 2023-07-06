#ifndef SPONGE_LIBSPONGE_TCP_SENDER_HH
#define SPONGE_LIBSPONGE_TCP_SENDER_HH

#include "byte_stream.hh"
#include "tcp_config.hh"
#include "tcp_segment.hh"
#include "wrapping_integers.hh"

#include <functional>
#include <queue>

using namespace std;
// 实现一个定时器，进行更新rto
// 还有是不是正在进行和生育时间
class Timer{
  private:
    uint32_t _rto;
    uint32_t _remain_time;
    bool _is_running;
  
  public:
    Timer(uint32_t rto);
    // 构造函数
    void start();
    // 打开running，设置rto=remain
    void stop();
    // 关闭定时器
    bool is_time_out();
    // 是不是超时，只要看剩余时间是不是小于=0
    void elapse(size_t elapsed);
    // 剩余时间减去这个，如果太小，直接剩余为0
    void set_time_out(uint32_t duration);
    // 超时更新时间，rto需要进行加倍，然后把rto和剩余一样更新



};
//! \brief The "sender" part of a TCP implementation.

//! Accepts a ByteStream, divides it up into segments and sends the
//! segments, keeps track of which segments are still in-flight,
//! maintains the Retransmission Timer, and retransmits in-flight
//! segments if the retransmission timer expires.
class TCPSender {
  private:
    //! our initial sequence number, the number for our SYN.
    WrappingInt32 _isn;

    //! outbound queue of segments that the TCPSender wants sent
    // 发送队列
    std::queue<TCPSegment> _segments_out{};
    // 还有没有确认的队列元素
      queue<pair<TCPSegment,uint64_t>> _outstand_segment{};
      
    //! retransmission timer for the connection
    // 重试次数
    unsigned int _initial_retransmission_timeout;

    //! outgoing stream of bytes that have not yet been sent
    ByteStream _stream;

    //! the (absolute) sequence number for the next byte to be sent
    uint64_t _next_seqno{0};
    // 返回的ack
    uint64_t _ack_seq{0};
    // 重传次数
    uint32_t _consecutive_retxs{0};
    uint64_t _outstand_bytes{0};
    // 没有确认的长度
    uint16_t _window_size{1};

    bool _is_syned{false};
    bool _is_fin{false};
    // 一个send维护一个定时器，在里面进行定时器操作
    Timer _timer;
    bool _not_rto{false};

  public:
    //! Initialize a TCPSender
    TCPSender(const size_t capacity = TCPConfig::DEFAULT_CAPACITY,
              const uint16_t retx_timeout = TCPConfig::TIMEOUT_DFLT,
              const std::optional<WrappingInt32> fixed_isn = {});

    //! \name "Input" interface for the writer
    //!@{
    ByteStream &stream_in() { return _stream; }
    const ByteStream &stream_in() const { return _stream; }
    //!@}

    //! \name Methods that can cause the TCPSender to send a segment
    //!@{

    //! \brief A new acknowledgment was received
    void ack_received(const WrappingInt32 ackno, const uint16_t window_size);

    //! \brief Generate an empty-payload segment (useful for creating empty ACK segments)
    void send_empty_segment();

    //! \brief create and send segments to fill as much of the window as possible
    void fill_window();

    //! \brief Notifies the TCPSender of the passage of time
    void tick(const size_t ms_since_last_tick);
    //!@}

    //! \name Accessors
    //!@{

    //! \brief How many sequence numbers are occupied by segments sent but not yet acknowledged?
    //! \note count is in "sequence space," i.e. SYN and FIN each count for one byte
    //! (see TCPSegment::length_in_sequence_space())
    size_t bytes_in_flight() const;

    //! \brief Number of consecutive retransmissions that have occurred in a row
    unsigned int consecutive_retransmissions() const;

    //! \brief TCPSegments that the TCPSender has enqueued for transmission.
    //! \note These must be dequeued and sent by the TCPConnection,
    //! which will need to fill in the fields that are set by the TCPReceiver
    //! (ackno and window size) before sending.
    std::queue<TCPSegment> &segments_out() { return _segments_out; }
    //!@}

    //! \name What is the next sequence number? (used for testing)
    //!@{

    //! \brief absolute seqno for the next byte to be sent
    uint64_t next_seqno_absolute() const { return _next_seqno; }

    //! \brief relative seqno for the next byte to be sent
    WrappingInt32 next_seqno() const { return wrap(_next_seqno, _isn); }
    // 发送代码，填充数据seq，还有data
    void send_segment(string &&data, bool syn, bool fin);
    //!@}
};

#endif  // SPONGE_LIBSPONGE_TCP_SENDER_HH
