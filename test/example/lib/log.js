/*
author Banz / classfellow@qq.com
*/

var path = require('path');
var Thread = require('./../../../index.js');
var app = module.exports = {};


// three threads are being created;
var thread1 = Thread();
thread1.bysec = 4;
var thread2 = Thread();
var thread3 = Thread();

 function GetTime(){
  return  ('' + new Date().getTime());
}

function Init1() {
  var logpath = path.join(__dirname, '..'
    , 'file_' + GetTime() + '_1.dat');
  thread1.initPrint(logpath, function(err, msg){
    if(err) console.error(err);
    else
    ;
  });
}

function Init2() {
  var logpath = path.join(__dirname, '..'
    , 'file_' + GetTime() + '_2.dat');
  thread2.initPrint(logpath, function(err, msg){
    if(err) console.error(err);
    else
    ;
  });
}


function reDo1(err, msg){
  if(err) console.error(err);
  else{
    Init1();
    thread1.delayBySec(5, reDo1);
  }
}

function reDo2(err, msg){
  if(err) console.error(err);
  else{
    Init2();
    thread2.delayBySec(2, reDo2);
  }
}

function set1hTimer1(){
  thread1.delayBySec(5, reDo1);
}

function set1hTimer2(){
  thread2.delayBySec(2, reDo2);
}

app.start = function(){
  console.log('---log runs');
  Init1();
  set1hTimer1();

  Init2();
  set1hTimer2();

  thread3.delayBySec(5, function(){
    // close the thread
    thread2.close();
    thread2 = null;
  });
}

if (require.main === module) {
  app.start();
  //test thread3 will be correctly closed by V8 Garbage Collection
  // I suggest you call close for responsibly,
  // You should maintain a thread of life
  //thread3 = null;
}else{
  ;
}

process.on('message', function(msg){
  do{
    if(msg.cmd == 'originalinfo'){
        var content = msg.content;
        if(thread1){
          thread1.printLog(content, function(err, msg){
            if(err) console.error(err);
          });
        }
        if(thread2){
          thread2.printLog(content, function(err, msg){
            if(err) console.error(err);
          });
        }
        break;
    }
  }while(false);
});

process.on('uncaughtException', function (err){
  console.error('logprocess uncaughtException: %s', err.message);
  var worker = require('cluster').worker;
  if(worker){
    process.exit(0);
  }
});
