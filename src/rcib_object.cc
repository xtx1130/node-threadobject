/* 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <node.h>
//#include <libplatform/libplatform.h>
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

#include "rcib.h"

#define ONE ((char*)1)
#define TWO ((char *)2)

namespace rcib{
  extern bool bterminating_;
}

extern base::LazyInstance<rcib::ArrayBufferAllocator> array_buffer_allocator_;
extern base::LazyInstance<rcib::furOfThread> furThread_;

namespace rcib {

  void* ArrayBufferAllocator::Allocate(size_t length) {
    void* data = AllocateUninitialized(length);
    return data == nullptr ? data : memset(data, 0, length);
  }
  void* ArrayBufferAllocator::AllocateUninitialized(size_t length) { return malloc(length); }
  void ArrayBufferAllocator::Free(void* data, size_t) { free(data); }

  // constructor
  RcibHelper::RcibHelper()
    :cprocessed_(0), handle_(nullptr){
  }
  //static
  RcibHelper* RcibHelper::GetInstance(){
    static RcibHelper This;
    return &This;
  }

  static void RunCallBack(async_req *req){
    static const char * const prtFIN = "finished";
    v8::Isolate* isolate = req->isolate;
    v8::HandleScope scope(isolate);
    // there is always at least one argument. "error"
    int argc = 1;

    v8::Local<v8::Value> argv[2];

    if (-1 == req->result || !req->out){
      printf("error: %s\n", req->error.c_str());
      argv[0] = node::UVException(-1, req->error.c_str(), req->func_name_.c_str(), "");
    }else if (ONE == req->out){
      req->out = nullptr;
      // error value is empty or null for non-error.
      argv[0] = v8::Null(isolate);
      argc = 2;
      argv[1] = v8::String::NewFromUtf8(isolate, prtFIN);
    }else if (TWO == req->out){
      req->out = nullptr;
      // error value is empty or null for non-error.
      argv[0] = v8::Null(isolate);
      argc = 2;
      argv[1] = v8::Uint32::NewFromUnsigned(isolate, static_cast<size_t>(req->result));
    } else {
      // error value is empty or null for non-error.
      argv[0] = v8::Null(isolate);
      // the data to pass is a string , use json
      argc = 2;
      char *data = (char *)req->out;
      argv[1] = v8::String::NewFromUtf8(isolate, data);
    }

    v8::TryCatch try_catch(isolate);

    v8::Local<v8::Function> callback =
      v8::Local<v8::Function>::New(isolate, req->callback);
    callback->Call(isolate->GetCurrentContext()->Global(), argc, argv);

    // cleanup
    req->callback.Reset();
    if (req->out){
      free(req->out);
      req->out = nullptr;
    }
    delete req;

    if (try_catch.HasCaught()) {
      node::FatalException(isolate, try_catch);
    }
  }
  //static
  void RcibHelper::AfterAsync(uv_async_t* h){
    async_t_handle *realh = reinterpret_cast<async_t_handle*>(h);

    RcibHelper::GetInstance()->PickFinished();

    async_req * req = nullptr;
    while (true){
      req = RcibHelper::GetInstance()->Pop();
      if (!req)
        break;
      RunCallBack(req);
    }//end while
  }
  //static
  void RcibHelper::DoNopAsync(async_req* req){
    req->result = 1;

    req->out = ONE;
  }
  //static
  void RcibHelper::DoNopAsync(async_req* r, size_t size){
    r->result = size;
    r->out = TWO;
  }
  //static
  void RcibHelper::EMark(async_req* req, std::string message){
    req->out = nullptr;
    req->result = -1;
    req->error = message;
  }
  //static
  void RcibHelper::init_async_req(async_req *req){
    if (!req)
      return;
    req->finished = false;
    req->out = nullptr;
    req->result = 0;
    RcibHelper::GetInstance()->Push(req);
  }

  void RcibHelper::Init(){
    //TODO del 
    array_buffer_allocator_.Get();
    //init this
    handle_ = new async_t_handle;
    uv_async_init(uv_default_loop(), (uv_async_t*)handle_, RcibHelper::AfterAsync);
  }

  void RcibHelper::Terminate(){
  }

  void RcibHelper::DelayByMil(async_req * req){
    DoNopAsync(req);
    Uv_Send(req, (uv_async_t*)handle_);
    cprocessed_++;
  }

  void RcibHelper::DelayBySec(async_req * req){
    DoNopAsync(req);
    Uv_Send(req, (uv_async_t*)handle_);
    cprocessed_++;
  }

  void RcibHelper::DelayByMin(async_req * req){
    DoNopAsync(req);
    Uv_Send(req, (uv_async_t*)handle_);
    cprocessed_++;
  }

  void RcibHelper::DelayByHour(async_req * req){
    DoNopAsync(req);
    Uv_Send(req, (uv_async_t*)handle_);
    cprocessed_++;
  }

  void RcibHelper::InitPrint(const std::string & path, async_req * req, size_t bysec, base::Thread* thr){
    assert(thr);
    rcib::PrintHelper * t = NULL;
    bool fis = false;
    do
    {
      base::FurRoler ft = thr->GetRoler(ROLEDEF(print));
      t = (rcib::PrintHelper *)(ft.get());
    } while (false);
    if (!t){
      base::FurRoler ft = base::FurRoler(new rcib::PrintHelper());
      thr->SetRoler(ROLEDEF(print), ft);
      t = (rcib::PrintHelper *)(ft.get());
      fis = true;
    }
    assert(t);
    if(0 == bysec)
      bysec = 1;
    t->setbysec(bysec);
    t->albytes_ = 0;

    if (!t->fp_){
    }else{
      fflush(t->fp_);
      fclose(t->fp_);
      t->fp_ = NULL;
    }
    if (path.length() > 0){
      t->fp_ = fopen(path.c_str(), "a");
    }
    
    if (t->fp_){
      if (fis)
        Fflush(thr);
      if(!fis){
        fseek(t->fp_, 0, SEEK_END);
        t->albytes_ = ftell(t->fp_);
      }
      DoNopAsync(req);
    }else{
      t->close();
      //2s
      t->setbysec(2);
      EMark(req, std::string("open file error"));
    }
    Uv_Send(req, (uv_async_t*)handle_);
    cprocessed_++;
  }

  void RcibHelper::PrintLogs(const std::string & info, async_req * req, base::Thread* thr){
    assert(thr);

    base::FurRoler ft = thr->GetRoler(ROLEDEF(print));
    rcib::PrintHelper * t = (rcib::PrintHelper *)(ft.get());

    if (!t){
      EMark(req, std::string("you should call initPrint first"));
      Uv_Send(req, (uv_async_t*)handle_);
      cprocessed_++;
      return;
    }

    if (t->fp_){
      std::string content = info + "\n";
      fputs(content.c_str(), t->fp_);
      t->albytes_ += content.length();
      DoNopAsync(req);
    } else {
      EMark(req, std::string("file not open"));
    }
    Uv_Send(req, (uv_async_t*)handle_);
    cprocessed_++;
  }

  void RcibHelper::Fflush(base::Thread* thr) {
    assert(thr);
    base::FurRoler ft = thr->GetRoler(ROLEDEF(print));
    rcib::PrintHelper * t = (rcib::PrintHelper *)(ft.get());

    if (!t) return;

    if (t->fp_)
      fflush(t->fp_);
    INT64 bysec = t->bysec_;
    //default 2000 millisec interval;
    thr->message_loop()->PostDelayedTask(base::Bind(base::Unretained(RcibHelper::GetInstance()),
      &RcibHelper::Fflush, thr),
      base::TimeDelta::FromSeconds(bysec));
  }

  void RcibHelper::CloseLog(base::Thread* thr){
    assert(thr);
    base::FurRoler ft = thr->GetRoler(ROLEDEF(print));
    rcib::PrintHelper * t = (rcib::PrintHelper *)(ft.get());
    if (!t) return;
    t->close();
    // 2s
    t->setbysec(2);
    cprocessed_++;
  }

  void RcibHelper::LogSize(async_req * req, base::Thread* thr){
    assert(thr);
    base::FurRoler ft = thr->GetRoler(ROLEDEF(print));
    rcib::PrintHelper * t = (rcib::PrintHelper *)(ft.get());
    if (!t || !t->fp_){
      EMark(req, std::string("you should call initPrint first"));
    }else{
      size_t size = t->albytes_;
      DoNopAsync(req, size);
    }
    Uv_Send(req, (uv_async_t*)handle_);
    cprocessed_++;
  }

  void RcibHelper::Uv_Send(async_req* req, uv_async_t* h){
    req->finished = true;
    uv_async_send(h);
  }

  //this function runs in main
  void RcibHelper::PickFinished(){
    std::list<async_req *> tmp_q;
    async_req * item = nullptr;
    while (!pending_queue_.empty()){
      item = pending_queue_.front();
      pending_queue_.pop_front();
      if (!item)
        continue;
      if (item->finished){
        working_queue_.push_back(item);
      }else{
        tmp_q.push_back(item);
      }
    }// end while
    pending_queue_.swap(tmp_q);
    DCHECK_EQ(tmp_q.size(), 0);
  }// end func

} // end rcib
