/* 
   Banz 
*/

var rcib = require('../build/Release/node-threadobject.node');
var THREAD = rcib.THREAD;

process.on('exit', function(){
  rcib.terminate();
})

var Thread =  function () {
  var o = {
    close : function () {
      thread_.close();
    }
    ,isRunning : function() {
      return thread_.isRunning();
    }
    ,delayByMil : function(mils, cb){
        thread_.delayByMil_i(mils, function (err){
          cb(err);
        });
    }
    ,delayBySec : function(secs, cb){
       thread_.delayBySec_i(secs, function(err){
         cb(err);
       })
    }
    ,delayByMin : function(mins, cb) {
       thread_.delayByMin_i(mins, function (err) {
         cb(err);
       })
    }
    , delayByHour : function(hours, cb){
        thread_.delayByHour_i(hours, function (err) {
          cb(err);
        })
    }
    , initPrint : function(path, cb){
        thread_.initPrint_i(path, function(err, data){
          cb(err, data);
        })
    }
    ,printLog : function(cont, cb){
      thread_.printLog_i(cont, function(err, data){
        cb(err, data);
      })
    }
    ,bytes : function(cb){
      thread_.bytes_i(function (err, size) {
        cb(err, size);
      })
    }
    ,closeLog : function(){
      thread_.closeLog();
    }
    ,set_delay : function(delta){
      if(delta > 0)
        thread_.bysec = delta;
    }
  }
  var thread_ =  new THREAD();
  return o;
}

module.exports = Thread;
