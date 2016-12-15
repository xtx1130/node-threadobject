/*
   "license": "BSD"
*/
"use strict";
if(process.versions.node >= '6.0.0')
  module.exports = require('./lib/node-threadobject.js');
else module.exports = require('./lib/node-threadobject-v4.js');
