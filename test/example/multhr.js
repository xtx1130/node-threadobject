/*
auth Banz
*/

var cluster = require('cluster');

var logpros = {};

function LogProLen() {
  var len = 0;
  for(var id in logpros){
    ++len;
  }
  return len;
}

var createLogProcess = function (val) {
  var worker = cluster.fork(val);
  logpros[worker.id] = worker;
  worker.on('exit', function(code){
    delete logpros[worker.id];
  });
  console.log('Create logprocess.id: ' + worker.id);
}

function StartOnlyLogProcess(){
  var new_worker_env = {};
  new_worker_env['LOGPRO'] = 'LogProcess';
  createLogProcess(new_worker_env);
}

if(cluster.isMaster){
  StartOnlyLogProcess();
}else{
  require('./lib/log.js').start();
  console.log('Two files in this directory are being written. And will be more');
}


setInterval(function(){
            try{
              for(var id in logpros){
                logpros[id].send({ cmd:'originalinfo', content: '' + 'hello, I just print something!' });
                break;
              }
            }
            catch(e){
              console.error('发送log日志失败');
            }
}, 40)

setInterval(function(){
            try{
              for(var id in logpros){
                logpros[id].send({ cmd:'originalinfo', content: '' + 'hello, I print what?' });
                break;
              }
            }
            catch(e){
              console.error('发送log日志失败');
            }
}, 40)
setInterval(function(){
            try{
              for(var id in logpros){
                logpros[id].send({ cmd:'originalinfo', content: '' + 'hello' });
                break;
              }
            }
            catch(e){
              console.error('发送log日志失败');
            }
}, 40)
setInterval(function(){
            try{
              for(var id in logpros){
                logpros[id].send({ cmd:'originalinfo', content: '' + 'hello' });
                break;
              }
            }
            catch(e){
              console.error('发送log日志失败');
            }
}, 40)
setInterval(function(){
            try{
              for(var id in logpros){
                logpros[id].send({ cmd:'originalinfo', content: '' + 'hello' });
                break;
              }
            }
            catch(e){
              console.error('发送log日志失败');
            }
}, 40)
setInterval(function(){
            try{
              for(var id in logpros){
                logpros[id].send({ cmd:'originalinfo', content: '' + 'hello, World Peace!' });
                break;
              }
            }
            catch(e){
              console.error('发送log日志失败');
            }
}, 40)
setInterval(function(){
            try{
              for(var id in logpros){
                logpros[id].send({ cmd:'originalinfo', content: '' + 'hello' });
                break;
              }
            }
            catch(e){
              console.error('发送log日志失败');
            }
}, 40)
setInterval(function(){
            try{
              for(var id in logpros){
                logpros[id].send({ cmd:'originalinfo', content: '' + 'hello' });
                break;
              }
            }
            catch(e){
              console.error('发送log日志失败');
            }
}, 40)
setInterval(function(){
            try{
              for(var id in logpros){
                logpros[id].send({ cmd:'originalinfo', content: '' + 'hello' });
                break;
              }
            }
            catch(e){
              console.error('发送log日志失败');
            }
}, 40)
setInterval(function(){
            try{
              for(var id in logpros){
                logpros[id].send({ cmd:'originalinfo', content: '' + 'hello' });
                break;
              }
            }
            catch(e){
              console.error('发送log日志失败');
            }
}, 40)

setInterval(function(){
            try{
              for(var id in logpros){
                logpros[id].send({ cmd:'originalinfo', content: '' + 'hello' });
                break;
              }
            }
            catch(e){
              console.error('发送log日志失败');
            }
}, 50)

setInterval(function(){
            try{
              for(var id in logpros){
                logpros[id].send({ cmd:'originalinfo', content: '' + 'hello' });
                break;
              }
            }
            catch(e){
              console.error('发送log日志失败');
            }
}, 50)
setInterval(function(){
            try{
              for(var id in logpros){
                logpros[id].send({ cmd:'originalinfo', content: '' + 'hello' });
                break;
              }
            }
            catch(e){
              console.error('发送log日志失败');
            }
}, 50)
setInterval(function(){
            try{
              for(var id in logpros){
                logpros[id].send({ cmd:'originalinfo', content: '' + 'hello' });
                break;
              }
            }
            catch(e){
              console.error('发送log日志失败');
            }
}, 50)
setInterval(function(){
            try{
              for(var id in logpros){
                logpros[id].send({ cmd:'originalinfo', content: '' + 'hello' });
                break;
              }
            }
            catch(e){
              console.error('发送log日志失败');
            }
}, 50)
setInterval(function(){
            try{
              for(var id in logpros){
                logpros[id].send({ cmd:'originalinfo', content: '' + 'hello' });
                break;
              }
            }
            catch(e){
              console.error('发送log日志失败');
            }
}, 50)
setInterval(function(){
            try{
              for(var id in logpros){
                logpros[id].send({ cmd:'originalinfo', content: '' + 'hello' });
                break;
              }
            }
            catch(e){
              console.error('发送log日志失败');
            }
}, 50)
setInterval(function(){
            try{
              for(var id in logpros){
                logpros[id].send({ cmd:'originalinfo', content: '' + 'hello' });
                break;
              }
            }
            catch(e){
              console.error('发送log日志失败');
            }
}, 50)
setInterval(function(){
            try{
              for(var id in logpros){
                logpros[id].send({ cmd:'originalinfo', content: '' + 'hello' });
                break;
              }
            }
            catch(e){
              console.error('发送log日志失败');
            }
}, 50)
setInterval(function(){
            try{
              for(var id in logpros){
                logpros[id].send({ cmd:'originalinfo', content: '' + 'hello' });
                break;
              }
            }
            catch(e){
              console.error('发送log日志失败');
            }
}, 50)
setInterval(function(){
            try{
              for(var id in logpros){
                logpros[id].send({ cmd:'originalinfo', content: '' + 'hello' });
                break;
              }
            }
            catch(e){
              console.error('发送log日志失败');
            }
}, 50)
setInterval(function(){
            try{
              for(var id in logpros){
                logpros[id].send({ cmd:'originalinfo', content: '' + 'hello' });
                break;
              }
            }
            catch(e){
              console.error('发送log日志失败');
            }
}, 50)
setInterval(function(){
            try{
              for(var id in logpros){
                logpros[id].send({ cmd:'originalinfo', content: '' + 'hello' });
                break;
              }
            }
            catch(e){
              console.error('发送log日志失败');
            }
}, 50)
setInterval(function(){
            try{
              for(var id in logpros){
                logpros[id].send({ cmd:'originalinfo', content: '' + 'hello' });
                break;
              }
            }
            catch(e){
              console.error('发送log日志失败');
            }
}, 50)
setInterval(function(){
            try{
              for(var id in logpros){
                logpros[id].send({ cmd:'originalinfo', content: '' + 'hello' });
                break;
              }
            }
            catch(e){
              console.error('发送log日志失败');
            }
}, 50)
setInterval(function(){
            try{
              for(var id in logpros){
                logpros[id].send({ cmd:'originalinfo', content: '' + 'hello' });
                break;
              }
            }
            catch(e){
              console.error('发送log日志失败');
            }
}, 50)
setInterval(function(){
            try{
              for(var id in logpros){
                logpros[id].send({ cmd:'originalinfo', content: '' + 'hello' });
                break;
              }
            }
            catch(e){
              console.error('发送log日志失败');
            }
}, 50)
setInterval(function(){
            try{
              for(var id in logpros){
                logpros[id].send({ cmd:'originalinfo', content: '' + 'hello' });
                break;
              }
            }
            catch(e){
              console.error('发送log日志失败');
            }
}, 50)
setInterval(function(){
            try{
              for(var id in logpros){
                logpros[id].send({ cmd:'originalinfo', content: '' + 'hello' });
                break;
              }
            }
            catch(e){
              console.error('发送log日志失败');
            }
}, 50)
