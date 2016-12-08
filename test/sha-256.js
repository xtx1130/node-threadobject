'use strict';
let path = require('path');
var fs = require('fs');
let assert = require('assert');
let Thread = require('../lib/node-threadobject');

var thread = new Thread();

fs.readFile('thread.js', function(err, data) {
  thread.sha256(data, function(err, data){
    console.log(data);
  })
});
