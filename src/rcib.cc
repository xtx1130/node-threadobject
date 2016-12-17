/* 
   "license": "BSD"
*/

#include "rcib.h"
#include "callbackinfo.h"

#include "hash/hash.h"
#include "delayed/delayed.h"
#include "file/file.h"

using namespace rcib;

namespace rcib{
  extern bool bterminating_;
}

extern base::LazyInstance<rcib::furOfThread> furThread_;

static void NewThread(const v8::FunctionCallbackInfo<v8::Value>& args){
  NOTH
  DCHECK(args.IsConstructCall());
  v8::HandleScope handle_scope(v8::Isolate::GetCurrent());
  furThread_.Get().Wrap(args.This());
  rcib::CallbackInfo::New(args.GetIsolate(), args.This(), rcib::CallbackInfo::Free);
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
