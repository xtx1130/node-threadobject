'use strict';
let path = require('path');
var fs = require('fs');
let assert = require('assert');
let Thread = require('../lib/node-threadobject');

var thread = new Thread();

console.log('HASH 计算之前');
fs.readFile('thread.js', function(err, data) {
  thread.sha256(data, function(err, data){
    console.log('HASH 计算结果');
    console.log(data);
    console.log('HASH 计算之后');
    console.log('正在排队处理的任务数：' + thread.numOfTasks());
  });
  console.log('正在排队处理的任务数：' + thread.numOfTasks());
});
