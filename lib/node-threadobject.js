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
        return thread_.delayByMil_i(mils, cb);
    }
    ,delayBySec : function(secs, cb){
       return thread_.delayBySec_i(secs, cb);
    }
    ,delayByMin : function(mins, cb) {
       return thread_.delayByMin_i(mins, cb);
    }
    , delayByHour : function(hours, cb){
        return thread_.delayByHour_i(hours, cb);
    }
    , initPrint : function(path, cb){
        return thread_.initPrint_i(path, cb);
    }
    ,printLog : function(cont, cb){
      return thread_.printLog_i(cont, cb);
    }
    ,bytes : function(cb){
      return thread_.bytes_i(cb);
    }
    ,closeLog : function(){
      thread_.closeLog();
    }
    ,set_delay : function(delta){
      delta = delta - 0;
      if(delta > 0)
        thread_.bysec = delta;
    }
    ,sha2: function(parm, cb){
      let type = parm.type,
          data = parm.data;
      if(256 == type || 384 == type || 512 == type){
        type = type - 0;
        return thread_.sha2(type, data, cb);
      }
      setImmediate(function(){
        cb(new Error('type should be one of {256,384,512}'));
      });
      return false;
    }
    ,hmac: function(parm, cb){
      let type = parm.type, 
          key = parm.key, 
          data = parm.data;
      if(256 == type || 384 == type || 512 == type) {
        type = type - 0;
        return thread_.hmac(type, key, data, cb);
      }
      setImmediate(function(){
        cb(new Error('type should be one of {256,384,512}'));
      });
      return false;
    }
    ,set_encode: function(str){
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
    ,numOfTasks: function(){
      return thread_.quen();
    }
    ,makeKeypair: function(seed, type){
      if(typeof seed == 'string') seed = new Buffer(seed, 'hex');
      return thread_.makeKeypair(seed);
    }
    ,Sign: function(message, key, cb){
      if(typeof key == 'string') key = new Buffer(key, 'hex');
      return thread_.sign(message, key, cb);
    }
    ,Verify: function(message, signature, pubkey, cb){
      if(typeof signature == 'string') signature = new Buffer(signature, 'hex');
      if(typeof pubkey == 'string') pubkey = new Buffer(pubkey, 'hex');
      return thread_.verify(message, signature, pubkey, cb);
    }
  }
  var thread_ =  new THREAD();
  return o;
}

module.exports = Thread;
