/* 
   "license": "BSD"
*/

#include "../rcib.h"
#include "file.h"

#define ROLEDEF(V) ("role-"#V)

//constructor
FileHelper::FileHelper(){
}

//static
FileHelper* FileHelper::GetInstance(){
  static FileHelper This;
  return &This;
}

void FileHelper::InitPrint(const std::string & path, rcib::async_req * req, 
                           size_t bysec, base::Thread* thr){
  assert(thr);
  PrintHelper * t = nullptr;
  bool fis = false;
  do {
    base::FurRoler ft = thr->GetRoler(ROLEDEF(print));
    t = (PrintHelper *)(ft.get());
  } while (false);
  if (!t){
    base::FurRoler ft = base::FurRoler(new PrintHelper());
    thr->SetRoler(ROLEDEF(print), ft);
    t = (PrintHelper *)(ft.get());
    fis = true;
  }
  assert(t);
  if (0 == bysec)
    bysec = 1;
  t->setbysec(bysec);
  t->albytes_ = 0;

  if (!t->fp_){
  }
  else{
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
    if (!fis){
      fseek(t->fp_, 0, SEEK_END);
      t->albytes_ = ftell(t->fp_);
    }
    rcib::RcibHelper::DoNopAsync(req);
  }else{
    t->close();
    //def 5s
    t->setbysec(5);
    rcib::RcibHelper::EMark(req, std::string("open file error"));
  }
  rcib::RcibHelper::GetInstance()->Uv_Send(req, NULL);
  rcib::RcibHelper::GetInstance()->ChgProcessed(1);
}

void FileHelper::PrintLogs(const std::string & info, rcib::async_req * req, base::Thread* thr){
  assert(thr);

  base::FurRoler ft = thr->GetRoler(ROLEDEF(print));
  PrintHelper * t = (PrintHelper *)(ft.get());
  rcib::RcibHelper::GetInstance()->ChgProcessed(1);

  if (!t){
    rcib::RcibHelper::EMark(req, std::string("you should call initPrint first"));
    rcib::RcibHelper::GetInstance()->Uv_Send(req, nullptr);
    return;
  }

  if (t->fp_){
    std::string content = info + "\n";
    fputs(content.c_str(), t->fp_);
    t->albytes_ += content.length();
    rcib::RcibHelper::DoNopAsync(req);
  } else {
    rcib::RcibHelper::EMark(req, std::string("cannot open file"));
  }
  rcib::RcibHelper::GetInstance()->Uv_Send(req, nullptr);
}

void FileHelper::LogSize(rcib::async_req * req, base::Thread* thr){
  assert(thr);
  base::FurRoler ft = thr->GetRoler(ROLEDEF(print));
  PrintHelper * t = (PrintHelper *)(ft.get());
  if (!t || !t->fp_){
    rcib::RcibHelper::EMark(req, std::string("you should call initPrint first"));
  } else{
    size_t size = t->albytes_;
    rcib::RcibHelper::DoNopAsync(req, size);
  }
  rcib::RcibHelper::GetInstance()->Uv_Send(req, nullptr);
  rcib::RcibHelper::GetInstance()->ChgProcessed(1);
}

void FileHelper::CloseLog(base::Thread* thr){
  assert(thr);
  base::FurRoler ft = thr->GetRoler(ROLEDEF(print));
  PrintHelper * t = (PrintHelper *)(ft.get());
  if (!t) return;
  t->close();
  t->setbysec(5);
  rcib::RcibHelper::GetInstance()->ChgProcessed(1);
}

void FileHelper::Fflush(base::Thread* thr){
  assert(thr);
  base::FurRoler ft = thr->GetRoler(ROLEDEF(print));
  PrintHelper * t = (PrintHelper *)(ft.get());

  if (!t) return;

  if (t->fp_)
    fflush(t->fp_);
  INT64 bysec = t->bysec_;
  //default 2000 millisec interval;
  thr->message_loop()->PostDelayedTask(base::Bind(base::Unretained(FileHelper::GetInstance()),
    &FileHelper::Fflush, thr),
    base::TimeDelta::FromSeconds(bysec));
}
