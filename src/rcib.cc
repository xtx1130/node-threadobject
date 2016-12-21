/* 
   "license": "BSD"
*/

#include "rcib.h"
#include "callbackinfo.h"

#include "hash/hash.h"
#include "delayed/delayed.h"
#include "file/file.h"
#include "ed25519/ed25519.h"

using namespace rcib;

namespace rcib{
  extern bool bterminating_;
}

extern base::LazyInstance<rcib::furOfThread> furThread_;

static void NewThread(const v8::FunctionCallbackInfo<v8::Value>& args){
  NOTH
  DCHECK(args.IsConstructCall());
  v8::HandleScope handle_scope(v8::Isolate::GetCurrent());
  void * data = furThread_.Get().Wrap(args.This());
  if(data) rcib::CallbackInfo::New(args.GetIsolate(), args.This(), rcib::CallbackInfo::Free, data);
}

static void Close(const v8::FunctionCallbackInfo<v8::Value>& args) {
  furThread_.Get().Close(static_cast<base::Thread*>(furThread_.Get().Unwrap(args.Holder())));
  furThread_.Get().Wrap(args.Holder(), (void*)(&rcib::bterminating_));
}

static void IsRunning(const v8::FunctionCallbackInfo<v8::Value>& args){
  args.GetReturnValue().Set(v8::Boolean::New(v8::Isolate::GetCurrent(), 
    furThread_.Get().IsRunning(static_cast<base::Thread*>(furThread_.Get().Unwrap(args.Holder())))));
}

static void DelayByMil(const v8::FunctionCallbackInfo<v8::Value>& args){
  THREAD;
  DELAY_TASK_COMMON(args);
  thr->message_loop()->PostDelayedTask(base::Bind(base::Unretained(DelayedHelper::GetInstance()),
    &DelayedHelper::DelayByMil, req),
    base::TimeDelta::FromMilliseconds(delayed));

  RETURN_TRUE
}

static void DelayBySec(const v8::FunctionCallbackInfo<v8::Value>& args){
  THREAD;
  DELAY_TASK_COMMON(args);
  thr->message_loop()->PostDelayedTask(base::Bind(base::Unretained(DelayedHelper::GetInstance()),
    &DelayedHelper::DelayBySec, req),
    base::TimeDelta::FromSeconds(delayed));

  RETURN_TRUE
}

static void DelayByMin(const v8::FunctionCallbackInfo<v8::Value>& args){
  THREAD;
  DELAY_TASK_COMMON(args);
  thr->message_loop()->PostDelayedTask(base::Bind(base::Unretained(DelayedHelper::GetInstance()),
    &DelayedHelper::DelayByMin, req),
    base::TimeDelta::FromMinutes(delayed));
  RETURN_TRUE
}

static void DelayByHour(const v8::FunctionCallbackInfo<v8::Value>& args){
  THREAD;
  DELAY_TASK_COMMON(args);
  thr->message_loop()->PostDelayedTask(base::Bind(base::Unretained(DelayedHelper::GetInstance()),
    &DelayedHelper::DelayByHour, req),
    base::TimeDelta::FromHours(delayed));
  RETURN_TRUE
}

static void InitPrint(const v8::FunctionCallbackInfo<v8::Value>& args){
  ISOLATE(args)
  if (args.Length() != 2 || !args[0]->IsString() || !args[1]->IsFunction()) {
    TYPEERROR;
  }

  GETATTRINUM(bysec, args.Holder(), bysec);

  if (bysec == -1){
    //def 5s
    bysec = 5;
  }

  v8::String::Utf8Value path(args[0]);
  std::string strpath = *path;
  THREAD
  INITHELPER(args,1);
  thr->message_loop()->PostTask(base::Bind(base::Unretained(FileHelper::GetInstance()),
    &FileHelper::InitPrint, strpath, req, bysec, thr));
  RETURN_TRUE
}

static void PrintLog(const v8::FunctionCallbackInfo<v8::Value>& args) {
  ISOLATE(args)

  if (args.Length() != 2 || !args[0]->IsString() || !args[1]->IsFunction()) {
    TYPEERROR;
  }

  v8::String::Utf8Value info(args[0]);
  std::string strinfo = *info;
  THREAD
  INITHELPER(args,1);
  thr->message_loop()->PostTask(base::Bind(base::Unretained(FileHelper::GetInstance()),
    &FileHelper::PrintLogs, strinfo, req, thr));
  RETURN_TRUE
}

static void LogSize(const v8::FunctionCallbackInfo<v8::Value>& args) {
  ISOLATE(args)
  if (args.Length() != 1 || !args[0]->IsFunction()) {
    TYPEERROR;
  }
  THREAD
  INITHELPER(args, 0);
  thr->message_loop()->PostTask(base::Bind(base::Unretained(FileHelper::GetInstance()),
    &FileHelper::LogSize, req, thr));
  RETURN_TRUE
}

static void CloseLog(const v8::FunctionCallbackInfo<v8::Value>& args) {
  ISOLATE(args)
  THREAD
  thr->message_loop()->PostTask(base::Bind(base::Unretained(FileHelper::GetInstance()),
  &FileHelper::CloseLog, thr));
  RETURN_TRUE
}

static void Sha2(const v8::FunctionCallbackInfo<v8::Value>& args) {
  ISOLATE(args);
  if (args.Length() != 3
      || !args[0]->IsNumber()
      || !(args[1]->IsString() || args[1]->IsUint8Array())  // should be a string or a buffer
      || !args[2]->IsFunction()) {
    TYPEERROR;
  }
  THREAD;  // if thread is not be created, return false in js
  INITHELPER(args, 2);
  HashData data;
  if (args[1]->IsString()) {
    v8::String::Utf8Value info(args[1]);
    data._data = *info;
  } else {
    data._p = node::Buffer::Data(args[1]);
    data._plen = node::Buffer::Length(args[1]);
  }
  req->w_t = TYPE_SHA;
  req->out = (char*)(new HashRe(&HashHelper::HashClean, thr->AsWeakPtr()));
  HashRe *hre = (HashRe *)(req->out);
  GETATTRINUM(encoding, args.Holder(), encoding);
  if (-1 == encoding || 0 == encoding){
    hre->_encoding = node::HEX;
  } else {
    HashHelper::RetType(encoding, hre->_encoding);
  }
  thr->message_loop()->PostTask(base::Bind(base::Unretained(HashHelper::GetInstance()),
    &HashHelper::SHA, args[0]->ToInt32()->Value(), data, req));
  thr->IncComputational();
  RETURN_TRUE
}

static void Hmac(const v8::FunctionCallbackInfo<v8::Value>& args) {
  ISOLATE(args);
  if (args.Length() != 4
    || !args[0]->IsNumber()
    || !(args[1]->IsString() || args[1]->IsUint8Array())  // should be a string or a buffer
    || !(args[2]->IsString() || args[2]->IsUint8Array())  // should be a string or a buffer
    || !args[3]->IsFunction()) {
    TYPEERROR;
  }
  THREAD;  // if thread is not be created, return false in js
  INITHELPER(args, 3);
  HashData data;
  if (args[1]->IsString()) {
    v8::String::Utf8Value info(args[1]);
    data._key = *info;
  } else {
    data._k = node::Buffer::Data(args[1]);
    data._klen = node::Buffer::Length(args[1]);
  }
  if (args[2]->IsString()) {
    v8::String::Utf8Value info(args[2]);
    data._data = *info;
  } else {
    data._p = node::Buffer::Data(args[2]);
    data._plen = node::Buffer::Length(args[2]);
  }
  req->w_t = TYPE_SHA;
  req->out = (char*)(new HashRe(&HashHelper::HashClean, thr->AsWeakPtr()));
  HashRe *hre = (HashRe *)(req->out);
  GETATTRINUM(encoding, args.Holder(), encoding);
  if (-1 == encoding || 0 == encoding){
    hre->_encoding = node::HEX;
  } else {
    HashHelper::RetType(encoding, hre->_encoding);
  }
  thr->message_loop()->PostTask(base::Bind(base::Unretained(HashHelper::GetInstance()),
    &HashHelper::Hmac, args[0]->ToInt32()->Value(), data, req));
  thr->IncComputational();
  RETURN_TRUE
}

static void QueueNum(const v8::FunctionCallbackInfo<v8::Value>& args) {
  THREAD;
  uint32_t num = static_cast<uint32_t>(thr->Computational());
  args.GetReturnValue().Set(num);
}

static void MakeKeypair(const v8::FunctionCallbackInfo<v8::Value>& args) {
  ISOLATE(args);
  if (args.Length() != 1
    || !args[0]->IsUint8Array()
    || node::Buffer::Length(args[0]) != 32 ){
    TYPEERROR2(MakeKeypair requires a 32 byte buffer);
  }
  const unsigned char* seed = (unsigned char*)node::Buffer::Data(args[0]);
  v8::Local<v8::Object> privateKey = node::Buffer::New(isolate, 64).ToLocalChecked();
  unsigned char* privateKeyData = (unsigned char*)node::Buffer::Data(privateKey);
  v8::Local<v8::Object> publicKey = node::Buffer::New(isolate, 32).ToLocalChecked();
  unsigned char* publicKeyData = (unsigned char*)node::Buffer::Data(publicKey);
  for (int i = 0; i < 32; i++)
    privateKeyData[i] = seed[i];
  crypto_sign_keypair(publicKeyData, privateKeyData);

  v8::Local<v8::Object> result = v8::Object::New(isolate);
  result->Set(v8::String::NewFromUtf8(isolate, "publicKey"), publicKey);
  result->Set(v8::String::NewFromUtf8(isolate, "privateKey"), privateKey);
  args.GetReturnValue().Set(result);
}

static void Sign(const v8::FunctionCallbackInfo<v8::Value>& args) {
  ISOLATE(args);
  if (args.Length() != 3
    || !args[0]->IsUint8Array()
    || !(args[1]->IsUint8Array() || (args[1]->IsObject() && !node::Buffer::HasInstance(args[1])))
    || !args[2]->IsFunction()){
    TYPEERROR2(Sign requires(Buffer, { Buffer(32 or 64) | keyPair object }, callback|1));
  }
  Ed25519Data data;
  data._msg = (unsigned char*)node::Buffer::Data(args[0]);
  data._mlen = node::Buffer::Length(args[0]);
  if(args[1]->IsObject() && !node::Buffer::HasInstance(args[1])){
    v8::Local<v8::Value> privateKeyBuffer = args[1]->ToObject()->Get(v8::String::NewFromUtf8(isolate, "privateKey"))->ToObject();
    if (!node::Buffer::HasInstance(privateKeyBuffer) || 64 != node::Buffer::Length(privateKeyBuffer)){
      TYPEERROR2(Sign requires(Buffer, { Buffer(32 or 64) | keyPair object }, callback|2));
    }
    data._privateKey = (unsigned char*)node::Buffer::Data(privateKeyBuffer);
  }
  else if(32 == node::Buffer::Length(args[1])){
    data._seed = (unsigned char*)node::Buffer::Data(args[1]);
  }
  else if(64 == node::Buffer::Length(args[1])){
    data._privateKey = (unsigned char*)node::Buffer::Data(args[1]);
  }
  else{
    TYPEERROR2(Sign requires(Buffer, { Buffer(32 or 64) | keyPair object }, callback|3));
  }
  THREAD;  // if thread is not be created, return false in js
  INITHELPER(args, 2);
  req->w_t = TYPE_ED25519;
  req->out = (char*)(new Ed25519Re(thr->AsWeakPtr(), Ed25519Re::SIGN));
  thr->message_loop()->PostTask(base::Bind(base::Unretained(Ed25519Helper::GetInstance()),
    &Ed25519Helper::Sign, data, req));
  thr->IncComputational();
  RETURN_TRUE
}

static void Verify(const v8::FunctionCallbackInfo<v8::Value>& args) {
  ISOLATE(args);
  Ed25519Data data;
  if (args.Length() != 4
    || !args[0]->IsUint8Array()
    || !args[1]->IsUint8Array()
    || !args[2]->IsUint8Array()
    || !args[3]->IsFunction()){
    TYPEERROR2(Verify requires(Buffer, Buffer(64), Buffer(32), callback));
  }
  if (!(32 == node::Buffer::Length(args[2]) && 64 == node::Buffer::Length(args[1]))){
    TYPEERROR2(Verify requires(Buffer, Buffer(64), Buffer(32), callback))
  }
  THREAD;  // if thread is not be created, return false in js
  INITHELPER(args, 3);
  data._msg = (unsigned char*)node::Buffer::Data(args[0]);
  data._mlen = node::Buffer::Length(args[0]);
  data._seed = (unsigned char*)node::Buffer::Data(args[1]);
  data._privateKey = (unsigned char*)node::Buffer::Data(args[2]); // here is pub
  req->w_t = TYPE_ED25519;
  req->out = (char*)(new Ed25519Re(thr->AsWeakPtr(), Ed25519Re::VERIFY));
  thr->message_loop()->PostTask(base::Bind(base::Unretained(Ed25519Helper::GetInstance()),
    &Ed25519Helper::Verify, data, req));
  thr->IncComputational();
  RETURN_TRUE
}

void Terminate(const v8::FunctionCallbackInfo<v8::Value>& args) {
  RcibHelper::GetInstance()->Terminate();
}

template <typename TypeName>
inline void NODE_CREATE_FUNCTION(const TypeName& target) {
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::HandleScope handle_scope(isolate);
    v8::Local<v8::FunctionTemplate> t = v8::FunctionTemplate::New(isolate,
      NewThread);
    t->InstanceTemplate()->SetInternalFieldCount(1);
    t->SetClassName(v8::String::NewFromUtf8(isolate, "THREAD"));

    //Add more func-s h-e-r-e ... ->rcib.cc
    NODE_SET_PROTOTYPE_METHOD(t, "close", Close);
    NODE_SET_PROTOTYPE_METHOD(t, "isRunning", IsRunning);
    NODE_SET_PROTOTYPE_METHOD(t, "delayByMil_i", DelayByMil);
    NODE_SET_PROTOTYPE_METHOD(t, "delayBySec_i", DelayBySec);
    NODE_SET_PROTOTYPE_METHOD(t, "delayByMin_i", DelayByMin);
    NODE_SET_PROTOTYPE_METHOD(t, "delayByHour_i", DelayByHour);
    NODE_SET_PROTOTYPE_METHOD(t, "initPrint_i", InitPrint);
    NODE_SET_PROTOTYPE_METHOD(t, "printLog_i", PrintLog);
    NODE_SET_PROTOTYPE_METHOD(t, "bytes_i", LogSize);
    NODE_SET_PROTOTYPE_METHOD(t, "closeLog", CloseLog);
    NODE_SET_PROTOTYPE_METHOD(t, "quen", QueueNum);
    NODE_SET_PROTOTYPE_METHOD(t, "sha2", Sha2);
    NODE_SET_PROTOTYPE_METHOD(t, "hmac", Hmac);
    NODE_SET_PROTOTYPE_METHOD(t, "makeKeypair", MakeKeypair);
    NODE_SET_PROTOTYPE_METHOD(t, "sign", Sign);
    NODE_SET_PROTOTYPE_METHOD(t, "verify", Verify);

    target->Set(v8::String::NewFromUtf8(isolate, "THREAD")
      , t->GetFunction());
  }

#define NODE_CREATE_FUNCTION NODE_CREATE_FUNCTION

void Init(v8::Local<v8::Object> target) {
  NODE_CREATE_FUNCTION(target);
  NODE_SET_METHOD(target, "terminate", Terminate);
  RcibHelper::GetInstance()->Init();
}

NODE_MODULE(rcib, Init)
