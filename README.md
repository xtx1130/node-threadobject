# node-threadobject

在 Js 代码中创建线程对象，执行 CPU 密集型函数，例如计算大文件 HASH，加密解密等任务。可扩展 C++ 模块的处理函数，处理不同的复杂计算任务。在多核环境下，线程对象有助于更好的分配 node 集群中各个线程的 CPU 占用，以可控的方式减少线程等待和阻塞主线程。

例如，在双核环境下，一个线程对象使得其与主线程各运行于一个 CPU。在这种情况下，如果有两个线程对象处理计算任务，则主线程有 66% 的机会争取到运行。与使用线程池相比，能够精确预期。

在具体的场景中，例如使用 node 构建区块链(Block chain)的 P2P 网络，协议使得主线程面临着 CPU 计算的压力，比方说计算大块数据的 HASH。未经扩展的 node 在此类情景下可能遇到瓶颈。node-threadobject 是解决此类问题的通用方法。

`node-threadobject` is a package for providing ability to create new threads in js. It helps you consciously assign cpu-bound tasks to a limited number of CPUs.

**[Follow me on github!](https://github.com/classfellow/node-threadobject)**

## 支持的操作系统(Platform Support)
> Windows & Linux

## 编译链接(Compile & Link)
```
先全局安装 node-gyp
在工程根目录下运行 node-gyp configure
在工程根目录下运行 node-gyp build
（编译过程大概需要 10s 左右，如果出现提示无权访问目录，请尝试 sudo node-gyp rebuild）

npm install -g node-gyp
node-gyp configure
node-gyp build (or  **sudo node-gyp rebuild** )
```

## How it works

参考了 chrome 浏览器的线程模型(chrome thread model)，每个线程内部包含了一个 C++ 闭包(C++ closures)的队列，按序处理任务。

## 增加更多的计算型函数 (Add more computational type functions)

有充足的空间，你可以很方便的添加新函数。这意味着一般来讲，只需要增加新的文件，然后将头文件增加到rcib.h中。hash 是一个例子，它是一个无状态型的计算任务，file 是另外一个例子。一般来讲，增加新的计算型函数不需要修改 rcib(run codes in background) 目录里面的代码。

src/rcib 这个目录下的代码经过精心构建，一般来讲，扩展本模块不需要动里面的东西。

src/file 这是一个有状态计算型任务的范例。

src/hash 这是一个无状态计算任务的范例。

## 安装使用(Usage)
```
npm install node-threadobject (or  **sudo npm install node-threadobject** )
```

## 例子(Example)

**将一个定时器抛给线程对象，等待2秒钟后，回到主线程执行回调函数。内部使用64位表示时间，支持以小时为单位的大定时器**
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

/*
result:
    thread running state: true
    after two secs
    thread running state: false
*/
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

/*
result:
    HASH 计算之前
    正在排队处理的任务数：1
    HASH 计算结果
    791c0370a140b881836221f69c664b430617ddd8e2f13e4ef7af425eaa3f2313
    HASH 计算之后
    正在排队处理的任务数：0
*/
```
## 已包含的函数 (APIs)
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
/*
未来会有更多密集型计算的扩展 :)
*/
```

## Other example
```js
// see test/
```
## More descriptions

This package provides a good way to extend node ability.It give you ability to have thread objects.

## 证书 (License)

BSD