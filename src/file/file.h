/* 
   "license": "BSD"
*/

#ifndef RCIB_FILE_
#define RCIB_FILE_

class FileHelper {
public:
  //constructor
  explicit FileHelper();
  //static
  static FileHelper* GetInstance();
  
  void InitPrint(const std::string & path, rcib::async_req * req, size_t bysec, base::Thread* thr);
  void PrintLogs(const std::string & info, rcib::async_req * req, base::Thread* thr);
  void LogSize(rcib::async_req * req, base::Thread* thr);
  void CloseLog(base::Thread* thr);
private:
  void Fflush(base::Thread* thr);
};

//
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

#endif
