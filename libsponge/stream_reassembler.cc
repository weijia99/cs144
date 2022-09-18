#include "stream_reassembler.hh"
#include"iostream"
// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;
// 
StreamReassembler::StreamReassembler(const size_t capacity) : next_index(0),rec(),s(capacity,'\0'),is_eof(false),unassembled_bytes_(0),_output(capacity), _capacity(capacity) {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    // DUMMY_CODE(data, index, eof);
    // 首先进行检索,看当前的index是不是超过
    if(index>=next_index+_output.remaining_capacity()){
        return;
        // 直接结束,说明索引,已经超过剩下的buffer里面
    }
    if(eof&&index+data.size()<=next_index+_output.remaining_capacity()){
        is_eof=true;
        // 代表写入结束,next+out是写入
        // output是之前那个byte
    }
    if(index+data.size()>next_index){
        // 首先选最小的
        for (size_t i=(index > next_index? index: next_index); i < next_index+ _output.remaining_capacity() && i < index + data.size(); i++)
        {
            /* code */
            // 直接选择最小的,然后这是输入的
            if (rec.count(i)==0){
                //set里面没有这个,进行加入data
                if (s.size()<=i)
                {
                    /* code */
                    string add(i,'\0');
//                    s.reserve(2*i);
                    s+=add;
//                    cout<<s.capacity();
                    //扩容
                }
                s[i]=data[i-index];
                //直接写入
                rec.insert(i);
                unassembled_bytes_++;
                //这是没有整理的,放入到字符串里面
                // 
                
            }
            
            
        }
        //写入结束,放入到output里面
        size_t n=0;
        size_t back=next_index;
        while (rec.count(next_index)>0)
        {
            /* code */
            n++;
            rec.erase(next_index);
            next_index++;
            unassembled_bytes_--;
            //这是已经写好了,然后进行减少
            
        }
        string x=s.substr(back,n);
        // cout<<x;
        _output.write(x);
        //写入这个单词


        
        
    }
    if (is_eof && empty()) {
       _output.end_input();
   }
}

size_t StreamReassembler::unassembled_bytes() const { return unassembled_bytes_; }

bool StreamReassembler::empty() const { return unassembled_bytes_==0; }
