/* 
   "license": "BSD"
*/

#include "../rcib.h"
#include "hash.h"
#include "sha-256/sha-256.h"

//constructor
HashData::HashData() :_p(NULL), _plen(-1){
}

// constructor
HashHelper::HashHelper(){
}

//static
HashHelper* HashHelper::GetInstance(){
  static HashHelper This;
  return &This;
}

//static
void HashHelper::HashClean(void *data){
  free(data);  // to free mem
  rcib::RcibHelper::GetInstance()->DEC();
}

void HashHelper::SHA256(const HashData& data, rcib::async_req * req){
  const char *p = nullptr;
  size_t plen = 0;
  if (-1 == data._plen){
    p = data._data.c_str();
    plen = data._data.size();
  }
  else{
    p = data._p;
    plen = data._plen;
  }
  sha256_context ctx;
  HashRe *hre = reinterpret_cast<HashRe *>(req->out);
  hre->_len = 32 * sizeof(uint8_t);
  hre->_data = (uint8_t *)malloc(hre->_len);
  sha256_init(&ctx);
  sha256_hash(&ctx, (uint8_t *)p, plen);
  sha256_done(&ctx, hre->_data);
  req->result = hre->_len;
  rcib::RcibHelper::GetInstance()->Uv_Send(req, NULL);
  rcib::RcibHelper::GetInstance()->ChgProcessed(1);
}
