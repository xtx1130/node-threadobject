/**
  author: Banz 
  email:classfellow@qq.com
  license: MIT
**/

#ifndef RCIB_HELPER_
#define RCIB_HELPER_

#define ROLEDEF(V) ("role-" \
          #V)

namespace rcib {
  
  struct async_t_handle{
    uv_async_t handle_;
  };

  struct async_req {
    const char * file_content_;
    std::string func_name_;
    std::string param_;
    std::string error;
    char *out;
    ssize_t result;
    v8::Isolate* isolate;
    v8::Persistent<v8::Function> callback;
    bool finished;
  };

  class ArrayBufferAllocator : public v8::ArrayBuffer::Allocator {
  public:
    virtual void* Allocate(size_t length);
    virtual void* AllocateUninitialized(size_t length);
    virtual void Free(void* data, size_t);
  };

  class RcibHelper {
    public:
      explicit RcibHelper();
      //static
      static RcibHelper* GetInstance();
      static void AfterAsync(uv_async_t* h);
      static void DoNopAsync(async_req* r);
      static void DoNopAsync(async_req* r, size_t size);
      static void EMark(async_req* req, std::string message);
      static void init_async_req(async_req *req);

      void Init();
      void Terminate();
      void DelayByMil(async_req * req);
      void DelayBySec(async_req * req);
      void DelayByMin(async_req * req);
      void DelayByHour(async_req * req);
      void InitPrint(const std::string & path, async_req * req, size_t bysec, base::Thread* thr);
      void PrintLogs(const std::string & info, async_req * req, base::Thread* thr);
      void LogSize(async_req * req, base::Thread* thr);
      void CloseLog(base::Thread* thr);

      inline void Push(async_req *itme) {
        pending_queue_.push_back(itme);
      }
      inline async_req* Pop() {
        if (working_queue_.empty())
          return NULL;
        async_req * item = working_queue_.front();
        working_queue_.pop_front();
        return item;
      }
      inline void PickFinished();
    private:
      void Fflush(base::Thread* thr);
      void Uv_Send(async_req* req, uv_async_t* h);
      std::list<async_req *> pending_queue_;
      std::list<async_req *> working_queue_;
      async_t_handle *handle_;
      INT64 cprocessed_;
  };

  class furOfThread {
  public:
    furOfThread(){
    }
    ~furOfThread(){
    }

    void Wrap(v8::Local<v8::Object> object){
      DCHECK_EQ(false, object.IsEmpty());
      DCHECK_G(object->InternalFieldCount(), 0);
      base::Thread *thread = new base::Thread();
      if (!thread->IsRunning()){
        thread->set_thread_name("distribute_task_thread");
        thread->StartWithOptions(base::Thread::Options());
      }
      object->SetAlignedPointerInInternalField(0, (void*)thread);
    }

    void Wrap(v8::Local<v8::Object> object, void *tmp){
      DCHECK_EQ(false, object.IsEmpty());
      DCHECK_G(object->InternalFieldCount(), 0);
      object->SetAlignedPointerInInternalField(0, nullptr);
    }

    void *Unwrap(v8::Local<v8::Object> object){
      DCHECK_EQ(false, object.IsEmpty());
      DCHECK_G(object->InternalFieldCount(), 0);
      return object->GetAlignedPointerFromInternalField(0);
    }

    bool IsRunning(base::Thread* thread){
      if (thread){
        return thread->IsRunning();
      }
      return false;
    }

    void Close(base::Thread* thread){
      if (thread){
        delete thread;
      }
    }

    base::Thread* Get(v8::Local<v8::Object> object){
      return static_cast<base::Thread*>(Unwrap(object));
    }

  };

  class PrintHelper : public base::Roler{
  public:
    PrintHelper() :fp_(NULL), bysec_(-1), albytes_(0){
    }
    virtual ~PrintHelper(){
      close();
    }
    void setbysec(size_t t){
      bysec_ = t;
    }
    void close(){
      if (fp_){
        fflush(fp_);
        fclose(fp_);
        fp_ = NULL;
      }
    }
    FILE *fp_;
    size_t bysec_;
    size_t albytes_;
  };

#define ISOLATE(V) v8::Isolate* isolate = (V).GetIsolate();\
  v8::HandleScope scope(isolate); \

#define TYPEERROR isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "param error"))); \
  return; \

#define INITHELPER(V,I)  v8::Local<v8::Function> callback = v8::Local<v8::Function>::Cast((V)[(I)]);  \
  async_req* req = new async_req; \
  RcibHelper::init_async_req(req);  \
  req->isolate = isolate; \
  req->callback.Reset(isolate, callback); \

#define DELAY_TASK_COMMON(V) ISOLATE(V) \
  if ((V).Length() != 2 || !(V)[0]->IsNumber() || !(V)[1]->IsFunction()) { \
    TYPEERROR \
    } \
  INT64 delayed = (V)[0]->ToUint32()->Value(); \
  INITHELPER(V, 1)

#define   NOTH     if (bterminating_) return;
#define   THREAD     base::Thread* thr = furThread_.Get().Get(args.Holder()); \
                     if (!thr) return; \

#define GETATTRINUM(N, O, S) size_t N = -1;  \
   do{  \
     v8::Local<v8::Object> object = (O); \
     DCHECK_EQ(false, object.IsEmpty());  \
     v8::Local<v8::String> proto = v8::String::NewFromUtf8(isolate, (#S)); \
     v8::Local<v8::Value> value = object->Get(proto); \
     if (value->IsNumber()){  \
       N = value->ToUint32()->Value();  \
     }  \
   }while(false); \

} //end rcib

#endif
