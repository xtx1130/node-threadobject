/**
  author: Banz 
  email:classfellow@qq.com
  license: MIT
**/

#include <node.h>
#include <v8.h>
#include <uv.h>
#if defined _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <node_buffer.h>
#include <malloc.h>
#include <stdlib.h>
#include <string>
#include <memory>
#include <list>
#include <queue>
#include <stack>
#include <map>
#include "rcib/macros.h"
#include "rcib/aligned_memory.h"
#include "rcib/lazy_instance.h"
#include "rcib/ref_counted.h"
#include "rcib/WrapperObj.h"
#include "rcib/WeakPtr.h"
#include "rcib/FastDelegateImpl.h"
#include "rcib/time/time.h"
#include "rcib/MessagePump.h"
#include "rcib/util_tools.h"
#include "rcib/Event/WaitableEvent.h"
#include "rcib/PendingTask.h"
#include "rcib/observer_list.h"
#include "rcib/MessagePumpDefault.h"
#include "rcib/MessageLoop.h"
#include "rcib/roler.h"
#include "rcib/Thread.h"
#include "rcib/at_exist.h"
#include "callbackinfo.h"
#include "hash/hash.h"
#include "rcib.h"

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
  thr->message_loop()->PostDelayedTask(base::Bind(base::Unretained(RcibHelper::GetInstance()),
    &RcibHelper::DelayByMil, req),
    base::TimeDelta::FromMilliseconds(delayed));

  RETURN_TRUE
}

static void DelayBySec(const v8::FunctionCallbackInfo<v8::Value>& args){
  THREAD;
  DELAY_TASK_COMMON(args);
  thr->message_loop()->PostDelayedTask(base::Bind(base::Unretained(RcibHelper::GetInstance()),
    &RcibHelper::DelayBySec, req),
    base::TimeDelta::FromSeconds(delayed));

  RETURN_TRUE
}

static void DelayByMin(const v8::FunctionCallbackInfo<v8::Value>& args){
  THREAD;
  DELAY_TASK_COMMON(args);
  thr->message_loop()->PostDelayedTask(base::Bind(base::Unretained(RcibHelper::GetInstance()),
    &RcibHelper::DelayByMin, req),
    base::TimeDelta::FromMinutes(delayed));
  RETURN_TRUE
}

static void DelayByHour(const v8::FunctionCallbackInfo<v8::Value>& args){
  THREAD;
  DELAY_TASK_COMMON(args);
  thr->message_loop()->PostDelayedTask(base::Bind(base::Unretained(RcibHelper::GetInstance()),
    &RcibHelper::DelayByHour, req),
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
    bysec = 10;
  }

  v8::String::Utf8Value path(args[0]);
  std::string strpath = *path;
  THREAD
  INITHELPER(args,1);
  thr->message_loop()->PostTask(base::Bind(base::Unretained(RcibHelper::GetInstance()),
    &RcibHelper::InitPrint, strpath, req, bysec, thr));
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
  thr->message_loop()->PostTask(base::Bind(base::Unretained(RcibHelper::GetInstance()),
    &RcibHelper::PrintLogs, strinfo, req, thr));
  RETURN_TRUE
}

static void LogSize(const v8::FunctionCallbackInfo<v8::Value>& args) {
  ISOLATE(args)
  if (args.Length() != 1 || !args[0]->IsFunction()) {
    TYPEERROR;
  }
  THREAD
  INITHELPER(args, 0);
  thr->message_loop()->PostTask(base::Bind(base::Unretained(RcibHelper::GetInstance()),
  &RcibHelper::LogSize, req, thr));
  RETURN_TRUE
}

static void CloseLog(const v8::FunctionCallbackInfo<v8::Value>& args) {
  ISOLATE(args)
  THREAD
  thr->message_loop()->PostTask(base::Bind(base::Unretained(RcibHelper::GetInstance()),
    &RcibHelper::CloseLog, thr));
  RETURN_TRUE
}

static void Sha256(const v8::FunctionCallbackInfo<v8::Value>& args) {
  ISOLATE(args);
  if (args.Length() != 2 
      || !(args[0]->IsString() || args[0]->IsUint8Array())  // should be a string or a buffer
      || !args[1]->IsFunction()) {
    TYPEERROR;
  }
  THREAD;  // if thread is not be created, return false in js
  INITHELPER(args, 1);
  HashData data;
  if (args[0]->IsString()) {
    v8::String::Utf8Value info(args[0]);
    data._data = *info;
  } else {
    data._p = node::Buffer::Data(args[0]);
    data._plen = node::Buffer::Length(args[0]);
  }
  req->w_t = TYPE_SHA_256;
  req->out = (char *)malloc(sizeof(HashRe));
  HashRe *hre = (HashRe *)(req->out);
  GETATTRINUM(encoding, args.Holder(), encoding);
  if (-1 == encoding || 0 == encoding){
    hre->_encoding = node::HEX;
  } else {
    //hre->_encoding = static_cast<node::encoding>(encoding);
    switch (encoding)
    {
    case 1:{
      hre->_encoding = node::UTF8;
    }
      break;
    case 2:{
      hre->_encoding = node::HEX;
    }
      break;
    case 3:{
      hre->_encoding = node::BASE64;
    }
      break;
    case 4:{
      hre->_encoding = node::BUFFER;
    }
      break;
    default:
      break;
    }
  }
  thr->message_loop()->PostTask(base::Bind(base::Unretained(RcibHelper::GetInstance()),
    &RcibHelper::SHA256, data, req));
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

    NODE_SET_PROTOTYPE_METHOD(t, "sha256", Sha256);

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