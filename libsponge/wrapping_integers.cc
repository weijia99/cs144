#include "wrapping_integers.hh"

// Dummy implementation of a 32-bit wrapping integer

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//实现把abs的64位转化为32位的wrappingInt32
//这里的n是64位的，
//给出一个绝对序列号n和一个初始序列号isn，产生的（相对）序列号。
//! Transform an "absolute" 64-bit sequence number (zero-indexed) into a WrappingInt32
//! \param n The input absolute 64-bit sequence number
//! \param isn The initial sequence number
WrappingInt32 wrap(uint64_t n, WrappingInt32 isn) {
//    DUMMY_CODE(n, isn);
return WrappingInt32{static_cast<uint32_t>(n) + isn.raw_value()};

}
//转换。给定一个序列号（n），初始序列号（isn），和一个绝对检查点（checkpoint）序列号，计算与对应的最接近checkpoint的绝对序列号。
//! Transform a WrappingInt32 into an "absolute" 64-bit sequence number (zero-indexed)
//! \param n The relative sequence number
//! \param isn The initial sequence number
//! \param checkpoint A recent absolute 64-bit sequence number
//! \returns the 64-bit sequence number that wraps to `n` and is closest to `checkpoint`
//!
//! \note Each of the two streams of the TCP connection has its own ISN. One stream
//! runs from the local TCPSender to the remote TCPReceiver and has one ISN,
//! and the other stream runs from the remote TCPSender to the local TCPReceiver and
//! has a different ISN.
//
uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint) {
//    DUMMY_CODE(n, isn, checkpoint);
int32_t min_step = n - wrap(checkpoint, isn);
// 将步数加到checkpoint上
int64_t ret = checkpoint + min_step;
// 如果反着走的话要加2^32
return ret >= 0 ? static_cast<uint64_t>(ret) : ret + (1ul << 32);

}
