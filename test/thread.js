'use strict';
let path = require('path');
let assert = require('assert');
let Thread = require('../lib/node-threadobject');

describe('exports', function () {
  describe('.Thread', function () {
    let filename = path.join(__dirname
            , 'file_' + new Date().getTime() + '.dat');

    let thread = new Thread(); 

    it('should create a new thread and can be closed', function () {
      // the same new Thread();
      let thread = new Thread();
      assert.equal(thread.isRunning(), true);
      thread.close();
      assert.equal(thread.isRunning(), false);
    });

    it('should create a new file named ' + filename, function (done) {
      thread.initPrint(filename, function(err){
         assert.ifError(err);
         done();
      })
      thread.closeLog();
    });

    it('should write `hello world` to the new file', function(done){
      thread.initPrint(filename, function(err){
         assert.ifError(err);
      })
      thread.printLog('hello world', function(err){
         assert.ifError(err);
         done();
      })
      thread.closeLog();
    });

    it('should open the file named '
        + filename
        + '\n          and append something to it.' 
        + '\n          Then create another file and write `hello world` to it', function(done){
      thread.initPrint(filename, function(err){
         assert.ifError(err);
      })
      thread.printLog('world peace', function(err){
         assert.ifError(err);
      })
      thread.printLog('道可道非常道', function(err){
         assert.ifError(err);
      })
      thread.printLog('上善若水', function(err){
         assert.ifError(err);
      })
      //closeLog is not necessary to be called
      thread.initPrint(filename + '2', function(err){
         assert.ifError(err);
      })
      thread.printLog('hello world', function(err){
         assert.ifError(err);
         done();
      })
      thread.closeLog();
    });

    it('should get the size of the file named '
         + filename 
         + '\n          , and the size should not be zero', function(done){
      thread.initPrint(filename, function(err){
         assert.ifError(err);
      })
      let firstsize;
      thread.bytes(function(err, size){
         assert.ifError(err);
         firstsize = size;
         assert.notEqual(0, firstsize);
      })
      thread.printLog('let file be larger', function(err){
         assert.ifError(err);
      })
      thread.bytes(function(err, size){
        assert.ifError(err);
        assert.ok(size > firstsize);
        thread.close();
        done(); 
      })
    });

    it('should after 2 sec run the callback', function(done){
      this.timeout(2500);
      var thread = new Thread();
      thread.delayBySec(2, function(err){
        assert.ifError(err);
        thread.close();
        done();
      })
    });
  });
});
