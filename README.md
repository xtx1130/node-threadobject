# 'node-threadobject'

`node-threadobject` is a package for providing ability to create new threads in js.The threads run in a loop like chrome thread mode.

**[Follow me on github!](https://github.com/classfellow/node-threadobject)**

## Platform Support
> Windows & Linux

## Compile & Link
> npm install -g node-gyp <br>
node-gyp configure <br>
node-gyp build

## How it works

## Usage
npm install node-threadobject

## Example:
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

## More API:
> 
close <br>
isRunning <br>
delayByMil <br>
delayBySec <br>
delayByMin <br>
delayByHour <br>
initPrint <br>
printLog

##Other example
```js
// see test/
```

## More descriptions

This package provides a good way to extend node ability.I have introduced a similar model with chrome thread programming framework, which makes it become more easier for c++ package.