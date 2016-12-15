'use strict';
let path = require('path');
var fs = require('fs');
let assert = require('assert');
let Thread = require('../../lib/node-threadobject');

var thread = new Thread();
thread.set_encode('base64');

console.log('HASH 计算之前');
fs.readFile('../thread.js', function(err, data) {
  thread.sha2({data: data, type: 256}, function(err, data){
    if(err) return console.error(err);
    console.log('HASH 计算结果');
    console.log(data);
    console.log('HASH 计算之后');
    console.log('正在排队处理的任务数：' + thread.numOfTasks());
  });
  console.log('正在排队处理的任务数：' + thread.numOfTasks());
});
