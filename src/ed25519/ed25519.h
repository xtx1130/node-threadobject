/* 
   "license": "BSD"
*/

#ifndef RCIB_ED25519_
#define RCIB_ED25519_

#include "ed25519/ed25519.h"

class Ed25519Data {
public:
  Ed25519Data();
  ~Ed25519Data(){
  }
  unsigned char* _msg;
  unsigned char* _privateKey; // or as pub
  unsigned char* _seed;  //or as signature
  size_t _mlen;
};

class Ed25519Re {
public:
  enum SubTypes {
    NONE,
    SIGN,
    VERIFY
  };
  explicit Ed25519Re(base::WeakPtr<base::Thread> thr, SubTypes type){
    _thr = thr;
    _type = type;
  }
  ~Ed25519Re(){
  }
  void Dec();
  unsigned char data[64];
  base::WeakPtr<base::Thread> _thr;
  SubTypes _type;
};

class Ed25519Helper {
public:
  explicit Ed25519Helper();
  //static
  static Ed25519Helper* GetInstance();
  // Sign
  void Sign(const Ed25519Data &data, rcib::async_req * req);
  // Verify
  void Verify(const Ed25519Data& data, rcib::async_req * req);
};

#endif
