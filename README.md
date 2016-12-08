# node-threadobject

在 Js 代码中创建线程对象，帮助处理 CPU 密集型任务，可扩展 C++ 模块的处理函数，处理不同的任务。

`node-threadobject` is a package for providing ability to create new threads in js.The threads run in a loop like command mode.

**[Follow me on github!](https://github.com/classfellow/node-threadobject)**

## Platform Support
> Windows & Linux

## Compile & Link
```
npm install -g node-gyp
node-gyp configure
node-gyp build (or **sudo node-gyp build**) 
```

## How it works

## Usage
```
npm install node-threadobject (or **sudo npm install node-threadobject**)
```

## Example:

**将一个定时器抛给线程对象，等待2秒钟后，回到主线程执行回调函数。支持以小时为单位的大定时器**
```js
/*
 
*/
var Thread = require('node-threadobject');

var thread = new Thread();

thread.delayBySec(2, function(err){
  if(err) 
    console.error(err);

  console.log('after two secs');
  thread.close();
  console.log('thread running state: ' + thread.isRunning());  
  thread = null;
  });

console.log('thread running state: ' + thread.isRunning());
```
**在另一个线程中计算一个文件的 HASH 值**
```js
/*
 see test/sha-256.js
*/
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

```
## More API:
```
close
isRunning
delayByMil
delayBySec
delayByMin
delayByHour
initPrint
printLog
sha256
```

## Other example
```js
// see test/
```

## More descriptions

This package provides a good way to extend node ability.It give you ability to have thread objects.