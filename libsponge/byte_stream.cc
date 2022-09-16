#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity):  _capacity(capacity),is_eof(false),_written_size(0),_read_size(0),_output() {}

size_t ByteStream::write(const string &data) {
    // DUMMY_CODE(data);
    if (is_eof)
        return 0;
    size_t l1=data.size();
    size_t l2=_capacity-_output.size();
    size_t left =min(l1,l2);
    for(size_t i=0;i<left;i++){
        _written_size++;
        _output.push_back(data[i]);
    }
    return left;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    // DUMMY_CODE(len);
    size_t l1=_output.size();
    size_t l2=len;
    size_t out_size=min(l1,l2);
    string s;
    

        
    return string(_output.begin(), _output.begin() +out_size);
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) { 
    size_t l1=_written_size;
    size_t l2=len;
    size_t out_size=min(l1,l2);
    // _written_size-=out_size;
    _read_size+=out_size;
    for(size_t i=0;i<out_size;i++){
        _output.pop_front();
    }
 }

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    string data=this->peek_output(len);
    this->pop_output(len);
    return data;
}

void ByteStream::end_input() {is_eof=true;}

bool ByteStream::input_ended() const { return is_eof; }

size_t ByteStream::buffer_size() const { return _output.size(); }

bool ByteStream::buffer_empty() const { return _output.size()==0; }

bool ByteStream::eof() const { return is_eof&&_output.empty(); }

size_t ByteStream::bytes_written() const { return _written_size; }

size_t ByteStream::bytes_read() const { return _read_size; }

size_t ByteStream::remaining_capacity() const { return _capacity-_output.size(); }
