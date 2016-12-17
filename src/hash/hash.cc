/* 
   "license": "BSD"
*/

#include "../rcib.h"
#include "hash.h"
#include "sha/hmac_sha.h"

//constructor
HashData::HashData() :_p(nullptr), _k(nullptr), _plen(-1), _klen(-1){
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
void HashHelper::HashClean(void *data, base::WeakPtr<base::Thread>& thread){
  free(data);  // to free mem
  // to dec num of tasks in this thr
  if (thread.get()){
    thread->DecComputational();
  }
}

//static
void HashHelper::RetType(int encoding, node::encoding & val){
  switch (encoding)
  {
  case 1:{
    val = node::UTF8;
  }
    break;
  case 2:{
    val = node::HEX;
  }
    break;
  case 3:{
    val = node::BASE64;
  }
    break;
  case 4:{
    val = node::BUFFER;
  }
    break;
  default:
    val = node::HEX;
    break;
  }
}

void HashHelper::SHA(int type, const HashData &data, rcib::async_req * req){
  if (256 == type || 384 == type || 512 == type){
    const char *p = nullptr;
    size_t plen;
    if (-1 == data._plen){
      p = data._data.c_str();
      plen = data._data.size();
    }else{
      p = data._p;
      plen = data._plen;
    }
    HashRe *hre = reinterpret_cast<HashRe *>(req->out);
    hre->_len = (type / 8) * sizeof(uint8_t);
    hre->_data = (uint8_t *)malloc(hre->_len);
    if (256 == type){
      sha256((uint8_t *)p, (unsigned int)plen, hre->_data);
    }else if (512 == type){
      sha512((uint8_t *)p, (unsigned int)plen, hre->_data);
    }else {
      sha384((uint8_t *)p, (unsigned int)plen, hre->_data);
    }
    req->result = hre->_len;
    rcib::RcibHelper::GetInstance()->Uv_Send(req, NULL);
  }else{
    rcib::RcibHelper::EMark(req, std::string("type should be 256/384/512"));
  }
}

void HashHelper::Hmac(int type, const HashData& data, rcib::async_req * req){
  if (256 == type || 384 == type || 512 == type) {
    const char *p = nullptr;
    const char *k = nullptr;
    size_t plen = 0;
    size_t klen = 0;
    if (-1 == data._plen){
      p = data._data.c_str();
      plen = data._data.size();
    }else{
      p = data._p;
      plen = data._plen;
    }
    if (-1 == data._klen){
      k = data._key.c_str();
      klen = data._key.size();
    } else {
      k = data._k;
      klen = data._klen;
    }
    HashRe *hre = reinterpret_cast<HashRe *>(req->out);
    hre->_len = (type / 8)* sizeof(uint8_t);
    hre->_data = (uint8_t *)malloc(hre->_len);
    if (256 == type){
      hmac_sha256((uint8_t *)k, static_cast<unsigned int>(klen), (uint8_t *)p,
        static_cast<unsigned int>(plen),
        hre->_data, SHA256_DIGEST_SIZE);
    }else if(512 == type) {
      hmac_sha512((uint8_t *)k, static_cast<unsigned int>(klen), (uint8_t *)p,
        static_cast<unsigned int>(plen),
        hre->_data, SHA512_DIGEST_SIZE);
    }else{
      hmac_sha384((uint8_t *)k, static_cast<unsigned int>(klen), (uint8_t *)p,
        static_cast<unsigned int>(plen),
        hre->_data, SHA384_DIGEST_SIZE);
    }
    req->result = hre->_len;
    rcib::RcibHelper::GetInstance()->Uv_Send(req, NULL);
  }else{
    rcib::RcibHelper::EMark(req, std::string("type should be 256/384/512"));
  }
}
