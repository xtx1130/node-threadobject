/**
  author: Banz 
  email:classfellow@qq.com
  license: MIT
**/

#ifndef RCIB_HASH_
#define RCIB_HASH_

class HashData {
public:
  HashData(){
    _p = NULL;
    _plen = -1;
  }
  std::string _data;
  char * _p;
  size_t _plen;
};

struct HashRe {
  uint8_t * _data;
  ssize_t _len;
  node::encoding _encoding;
  void(*Clean)(void *);
};

#endif
