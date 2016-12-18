#!/usr/local/bin/node
if(process.argv.length<3){
  console.log("Usage: %s %s [gen|test]",process.argv[0],process.argv[1]);
  process.exit(1);
}
var crypto = require('crypto');
var fs = require('fs');
var path = require('path');
var moment = require('moment');
var promise = require('q');

// HERE READ SOME KEY PAIRS

//function write_pair(prefix){
//  var ecdh = crypto.createECDH('secp256k1');
//  ecdh.generateKeys('base64');
//
//  fs.writeFileSync(path.join(__dirname,prefix+'.pub.base64'),ecdh.getPublicKey());
//  fs.writeFileSync(path.join(__dirname,prefix+'.priv.base64'),ecdh.getPrivateKey());
//}
//function read_public(prefix){
//  return fs.readFileSync(path.join(__dirname,prefix+'.pub.base64'));
//}
//function read_private(prefix){
//  return fs.readFileSync(path.join(__dirname,prefix+'.priv.base64'));
//}

// MIGHT BE GOOD FOR SALTED NONCES

var nonce = {};
function salt(){
  var counter_value = Math.floor(moment().unix() / (5)).toString();
  if(!nonce[counter_value]){
    var p = promise.defer();
    crypto.randomBytes(256,function(ex,buf){
      if(ex)throw ex;
      nonce[counter_value] = buf;
      p.resolve(nonce[counter_value]);
    });
    return p;
  }else{
    return nonce[counter_value];
  }
}

function human(buf){
//  buf = new Buffer([0x1f,0x86,0x98,0x69,0x0e,0x02,0xca,0x16,0x61,0x85,0x50,0xef,0x7f,0x19,0xda,0x8e,0x94,0x5b,0x55,0x5a]);
  var offset = buf[19] & 0xf;
  var bin_code = new Buffer([(buf[offset] & 0x7f),(buf[offset+1] & 0xff),(buf[offset+2] & 0xff),(buf[offset+3] & 0xff)]);  
  bin_code = bin_code.readIntBE(0,4);
  bin_code = (bin_code % (Math.pow(10,6)));
  buf = new Buffer(6);
  buf.fill('0');
  buf.write(bin_code.toString(),0,6-bin_code.length);
  return buf.toString();
}

function puzzle(off,cbyte,xbuf,pow){
  return function puz(buf1){
    var offset = buf1[off] &0xf;
    var bin_code = new Buffer(cbyte);
    var i = 0;
    while(i<cbyte)
      bin_code.writeUInt8(buf1[offset+i] & xbuf[i],i++);
    bin_code = bin_code.readIntBE(0,4);
    bin_code = (bin_code % (Math.pow(10,pow)));
    var buf = new Buffer(pow);
    buf.fill('0');
    buf.write(bin_code.toString(),0,6-bin_code.length);
    return buf.toString();
  }
}
xbuf = new Buffer([0x7f,0xff,0xff,0xff]);
buf1 = new Buffer([0x1f,0x86,0x98,0x69,0x0e,0x02,0xca,0x16,0x61,0x85,0x50,0xef,0x7f,0x19,0xda,0x8e,0x94,0x5b,0x55,0x5a]);
buf2 = new Buffer([0x1f,0x86,0x98,0x69,0x0e,0x02,0xca,0x16,0x61,0x85,0x50,0xef,0x7f,0x19,0xda,0x8e,0x94,0x5b,0x55,0x5a]);
console.log(puzzle(19,4,xbuf,6)(buf1));

function totp(secret){
  var p = promise.defer();
  promise.when(secret,function(secret){
    var sha256 = crypto.createHash('sha256');
    sha256.update(Math.floor(moment().unix() / (5)).toString());
    sha256.update(secret);
    p.resolve(human(sha256.digest('bin')));
  });
  return p;
}

// LOOKS LIKE AN AUTHENTICATOR THINGY

var but = new Buffer(20);
but.fill(0);
but.write('12345678901234567890');
function hotp(c){
  var sha1 = crypto.createHmac('sha1',but);
  var buf = new Buffer(20);
  buf.fill(0);
  buf.writeIntBE(c,16,4);
  sha1.update(buf);
  return human(sha1.digest('bin'));
}

//var secret = 'secret';
//setInterval(function(){
//  console.log(Math.floor(moment().unix() / (5)).toString());
//  totp(salt()).then(function(num){console.log(num)});
//},2000);
//setInterval(function(){process.exit();},10*1000);

// PROBABLY ECDH

//if(process.argv[2] == 'gen'){
//  write_pair('bob');
//  write_pair('alice');
//}else{
//  var alice_ecdh = crypto.createECDH('secp256k1');
//  var bob_ecdh = crypto.createECDH('secp256k1');
//  
//  alice_ecdh.setPrivateKey(read_private('alice'),'base64');
//  bob_ecdh.setPrivateKey(read_private('bob'),'base64');
//  
//  var alice_secret = alice_ecdh.computeSecret(read_public('bob'),'base64');
//  var bob_secret = bob_ecdh.computeSecret(read_public('alice'),'base64');
//
//  if(!alice_secret.compare(bob_secret)){
//    console.log(alice_secret.toString());
//  }else{
//    process.exit(1);
//  }
//  totp(alice_secret,salt()).then(function(){
//    totp(bob_secret,salt());
//  });
//  setTimeout(function(){
//    totp(alice_secret,salt()).then(function(){
//      totp(bob_secret,salt());
//    });
//  },15*1000);
