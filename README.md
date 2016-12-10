# node-threadobject

在 Js 代码中创建线程对象，执行 CPU 密集型函数，例如计算大文件 HASH，加密解密等任务。可扩展 C++ 模块的处理函数，处理不同的复杂计算任务。在多核环境下，线程对象有助于更好的分配 node 集群中各个线程的 CPU 占用，以可控的方式减少线程等待和阻塞主线程。

在双核环境下，一个线程对象使得其与主线程各运行于一个 CPU。在这种情况下，如果有两个线程对象处理计算任务，则主线程有 66% 的机会争取到运行。与使用线程池相比，能够精确预期。

`node-threadobject` is a package for providing ability to create new threads in js.The threads run in a loop like command mode.

**[Follow me on github!](https://github.com/classfellow/node-threadobject)**

## Platform Support
> Windows & Linux

## Compile & Link
```
npm install -g node-gyp
node-gyp configure
node-gyp build (or  **sudo node-gyp build** ) 
```

## How it works

## Usage
```
npm install node-threadobject (or  **sudo npm install node-threadobject** )
```

## Example:

**将一个定时器抛给线程对象，等待2秒钟后，回到主线程执行回调函数。支持以小时为单位的大定时器**
```js
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
```
## More API:
```
close  //同步的关闭线程
isRunning  //返回线程对象的线程是否运行(存在)
delayByMil  //以毫秒延迟
delayBySec  //
delayByMin  //
delayByHour  //
initPrint  //初始化一个打印任务
printLog  //顺序打印
closeLog  //关闭打印
sha256  //计算SHA256
numOfTasks  //线程队列里CPU密集型任务个数
```

## Other example
```js
// see test/
```

## More descriptions

This package provides a good way to extend node ability.It give you ability to have thread objects.