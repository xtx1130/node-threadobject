'use strict';
var fs = require('fs');
let Thread = require('./../../index.js');
var thread = new Thread();

thread.set_encode('base64');

function callback(err, data) {
  thread.sha2({data: data, type: 256}, function(err, data){
  	if(err) return console.error(err);
    console.log('HASH 计算结果');
    console.log(data);

    fs.readFile('./mem-pressure-test', callback);
  });
}

fs.readFile('./mem-pressure-test', callback);
