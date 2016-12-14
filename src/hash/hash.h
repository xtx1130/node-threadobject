/* 
   "license": "BSD"
*/

#ifndef RCIB_HASH_
#define RCIB_HASH_

class HashData {
public:
  HashData();
  std::string _data;
  std::string _key;
  char * _p;
  size_t _plen;
  char *_k;
  size_t _klen;
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
  // sha
  void SHA(int type /*256|384|512*/, const HashData& data, rcib::async_req * req);
  // hmac
  void Hmac(int type /*256|384|512*/, const HashData& data, rcib::async_req * req);
};

#endif
