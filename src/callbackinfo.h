#ifndef CALLBACKINFO_
#define CALLBACKINFO_

namespace rcib {
  typedef void(*FreeCallback)(void* data, void* hint);

  class CallbackInfo {
  public:
    static inline void Free(void* data, void* hint);
    static inline CallbackInfo* New(v8::Isolate* isolate,
      v8::Handle<v8::Object> object,
      FreeCallback callback,
      void* hint = 0);
    inline v8::Persistent<v8::Object>* persistent();
  private:
    static void WeakCallback(const v8::WeakCallbackData<v8::Object, CallbackInfo>&);
    inline void WeakCallback(v8::Isolate* isolate, v8::Local<v8::Object> object);
    inline CallbackInfo(v8::Isolate* isolate,
      v8::Handle<v8::Object> object,
      FreeCallback callback,
      void* hint);
    ~CallbackInfo();
    v8::Persistent<v8::Object> persistent_;
    FreeCallback const callback_;
    void* const hint_;
    DISALLOW_COPY_AND_ASSIGN_(CallbackInfo);
  };

  void CallbackInfo::Free(void* data, void*) {
    base::Thread *thr = static_cast<base::Thread*>(data);
    if (!thr) return;
    delete thr;
  }

  CallbackInfo* CallbackInfo::New(v8::Isolate* isolate,
    v8::Handle<v8::Object> object,
    FreeCallback callback,
    void* hint) {
    return new CallbackInfo(isolate, object, callback, hint);
  }

  v8::Persistent<v8::Object>* CallbackInfo::persistent() {
    return &persistent_;
  }

  CallbackInfo::CallbackInfo(v8::Isolate* isolate,
    v8::Handle<v8::Object> object,
    FreeCallback callback,
    void* hint)
    : persistent_(isolate, object),
    callback_(callback),
    hint_(hint) {
    DCHECK_G(object->InternalFieldCount(), 0);
    persistent_.SetWeak(this, WeakCallback);
    persistent_.MarkIndependent();
  }

  CallbackInfo::~CallbackInfo() {
    persistent_.Reset();
  }

  void CallbackInfo::WeakCallback(
    const v8::WeakCallbackData<v8::Object, CallbackInfo>& data) {
    data.GetParameter()->WeakCallback(data.GetIsolate(), data.GetValue());
  }

  void CallbackInfo::WeakCallback(v8::Isolate* isolate, v8::Local<v8::Object> object) {
    DCHECK_G(object->InternalFieldCount(), 0);
    base::Thread *thr = static_cast<base::Thread*>(object->GetAlignedPointerFromInternalField(0));
    callback_(thr, hint_);
    object->SetAlignedPointerInInternalField(0, NULL);
    delete this;
  }
} // end namespace

#endif