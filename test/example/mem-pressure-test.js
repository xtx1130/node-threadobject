'use strict';
var fs = require('fs');
let Thread = require('./../../index.js');
var thread = new Thread();
thread.set_encode('base64');

var fData = null;
function callback(err, data) {
  if(err) return console.error(err);
  console.log(data);
  setImmediate(function(){
    thread.sha2({data: fData, type: 256}, callback);
  });
}
fs.readFile('./mem-pressure-test', function(err, data){
  fData = data
  thread.sha2({data: fData, type: 256}, callback);
});
