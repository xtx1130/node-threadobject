/* 
   "license": "BSD"
*/

#ifndef RCIB_HASH_
#define RCIB_HASH_

class HashData {
public:
  HashData();
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

class HashHelper {
public:
  explicit HashHelper();
  //static
  static HashHelper* GetInstance();
  //static
  static void HashClean(void *data);
  // sha-256 
  void SHA256(const HashData& data, rcib::async_req * req);
};

#endif
