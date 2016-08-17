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

  DELAY_TASK_COMMON(args);
  THREAD;

  thr->message_loop()->PostDelayedTask(base::Bind(base::Unretained(RcibHelper::GetInstance()),
    &RcibHelper::DelayByMil, req),
    base::TimeDelta::FromMilliseconds(delayed));
}

static void DelayBySec(const v8::FunctionCallbackInfo<v8::Value>& args){

  DELAY_TASK_COMMON(args);
  THREAD;

  thr->message_loop()->PostDelayedTask(base::Bind(base::Unretained(RcibHelper::GetInstance()),
    &RcibHelper::DelayBySec, req),
    base::TimeDelta::FromSeconds(delayed));
}

static void DelayByMin(const v8::FunctionCallbackInfo<v8::Value>& args){

  DELAY_TASK_COMMON(args);
  THREAD;

  thr->message_loop()->PostDelayedTask(base::Bind(base::Unretained(RcibHelper::GetInstance()),
    &RcibHelper::DelayByMin, req),
    base::TimeDelta::FromMinutes(delayed));
}

static void DelayByHour(const v8::FunctionCallbackInfo<v8::Value>& args){

  DELAY_TASK_COMMON(args);
  THREAD;

  thr->message_loop()->PostDelayedTask(base::Bind(base::Unretained(RcibHelper::GetInstance()),
    &RcibHelper::DelayByHour, req),
    base::TimeDelta::FromHours(delayed));
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

  INITHELPER(args,1);
  THREAD

  thr->message_loop()->PostTask(base::Bind(base::Unretained(RcibHelper::GetInstance()),
    &RcibHelper::InitPrint, strpath, req, bysec, thr));
}

static void PrintLog(const v8::FunctionCallbackInfo<v8::Value>& args) {

  ISOLATE(args)

  if (args.Length() != 2 || !args[0]->IsString() || !args[1]->IsFunction()) {
    TYPEERROR;
  }

  v8::String::Utf8Value info(args[0]);
  std::string strinfo = *info;

  INITHELPER(args,1);
  THREAD

  thr->message_loop()->PostTask(base::Bind(base::Unretained(RcibHelper::GetInstance()),
    &RcibHelper::PrintLogs, strinfo, req, thr));
}

static void LogSize(const v8::FunctionCallbackInfo<v8::Value>& args) {
  ISOLATE(args)
  if (args.Length() != 1 || !args[0]->IsFunction()) {
    TYPEERROR;
  }
  INITHELPER(args, 0);
  THREAD
  thr->message_loop()->PostTask(base::Bind(base::Unretained(RcibHelper::GetInstance()),
  &RcibHelper::LogSize, req, thr));
}

static void CloseLog(const v8::FunctionCallbackInfo<v8::Value>& args) {
  ISOLATE(args)
  THREAD
  thr->message_loop()->PostTask(base::Bind(base::Unretained(RcibHelper::GetInstance()),
    &RcibHelper::CloseLog, thr));
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