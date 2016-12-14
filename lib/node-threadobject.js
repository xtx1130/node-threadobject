/*
   "license": "BSD"
*/

"use strict";
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
        return thread_.delayByMil_i(mils, function (err){
          cb(err);
        });
    }
    ,delayBySec : function(secs, cb){
       return thread_.delayBySec_i(secs, function(err){
         cb(err);
       })
    }
    ,delayByMin : function(mins, cb) {
       return thread_.delayByMin_i(mins, function (err) {
         cb(err);
       })
    }
    , delayByHour : function(hours, cb){
        return thread_.delayByHour_i(hours, function (err) {
          cb(err);
        })
    }
    , initPrint : function(path, cb){
        return thread_.initPrint_i(path, function(err, data){
          cb(err, data);
        })
    }
    ,printLog : function(cont, cb){
      return thread_.printLog_i(cont, function(err, data){
        cb(err, data);
      })
    }
    ,bytes : function(cb){
      return thread_.bytes_i(function (err, size) {
        cb(err, size);
      })
    }
    ,closeLog : function(){
      thread_.closeLog();
    }
    ,set_delay : function(delta){
      delta = delta - 0;
      if(delta > 0)
        thread_.bysec = delta;
    }
    , sha2: function({type, data}, cb) {
      if(256 == type || 384 == type || 512 == type) {
        type = type - 0;
        return thread_.sha2(type, data, function(err, data){
          cb(err, data);
        });
      }
      setImmediate(function(){
        cb(new Error('type should be one of {256,384,512}'));
      });
      return false;
    }
    ,hmac: function({type, key, data}, cb) {
      if(256 == type || 384 == type || 512 == type) {
        type = type - 0;
        return thread_.hmac(type, key, data, function(err, data){
          cb(err, data);
        });
      }
      setImmediate(function(){
        cb(new Error('type should be one of {256,384,512}'));
      });
      return false;
    }
    , set_encode: function(str){
      if(str == 'utf8'){
        thread_.encoding = 1;
      }else if(str == 'hex'){
        thread_.encoding = 2;
      }else if(str == 'base64'){
        thread_.encoding = 3;
      }else if(str == 'buffer'){
        thread_.encoding = 4;
      }else thread_.encoding = 0;
    }
    , numOfTasks: function(){
      return thread_.quen();
    }
  }
  var thread_ =  new THREAD();
  return o;
}

module.exports = Thread;
