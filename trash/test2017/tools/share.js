console.log("share.js runned...");
var http = require("http"),
    https = require("https"),
    url = require("url"),
    path = require("path"),
    fs = require("fs"),
    os = require("os"),
    qs = require('querystring');

var qap_log=s=>console.log("["+getDateTime()+"] "+s);
var qap_err=(context)=>(s=>console.log("["+getDateTime()+"] "+context+": "+s));

function getDateTime() {
  var now     = new Date(); 
  var year    = now.getFullYear();
  var f=v=>(v.toString().length==1?'0':'')+v;
  var month   = f(now.getMonth()+1); 
  var day     = f(now.getDate());
  var hour    = f(now.getHours());
  var minute  = f(now.getMinutes());
  var second  = f(now.getSeconds()); 
  var dateTime = year+'.'+month+'.'+day+' '+hour+':'+minute+':'+second;   
  return dateTime;
}

var xhr_get=(url,ok,err)=>{
  var req=(url.substr(0,"https".length)=="https"?https:http).get(url,(res)=>{
    var statusCode=res.statusCode;var contentType=res.headers['content-type'];var error;
    if(statusCode!==200){error=new Error('Request Failed.\nStatus Code: '+statusCode);}
    if(error){err(error.message);res.resume();return;}
    //res.setEncoding('utf8');
    var rawData='';res.on('data',(chunk)=>rawData+=chunk);
    res.on('end',()=>{try{ok(rawData);}catch(e){err(e.message);}});
  }).on('error',(e)=>{err('Got error: '+e.message);});
  return req;
}

var xhr=(method,URL,data,ok,err)=>{
  var up=url.parse(URL);var secure=up.protocol=='https';
  var options={
    hostname:up.host,port:secure?443:80,path:up.path,method:method.toUpperCase(),
    headers:{'Content-Type':'application/x-www-form-urlencoded','Content-Length':Buffer.byteLength(data)}
  };
  var req=(secure?https:http).request(options,(res)=>{
    var statusCode=res.statusCode;var contentType=res.headers['content-type'];var error;
    if(statusCode!==200){error=new Error('Request Failed.\nStatus Code: '+statusCode);}
    if(error){err(error.message,res);res.resume();return;}
    //res.setEncoding('utf8');
    var rawData='';res.on('data',(chunk)=>rawData+=chunk);
    res.on('end',()=>{try{ok(rawData,res);}catch(e){err(e.message,res);}});
  }).on('error',e=>{err('Got error: '+e.message,null);});
  req.end(data);
  return req;
}

var xhr_post=(url,obj,ok,err)=>xhr('post',url,qs.stringify(obj),ok,err);

var rand=()=>(Math.random()*1024*64|0);
var mapkeys=Object.keys;var mapvals=(m)=>mapkeys(m).map(k=>m[k]);
var hosts={};var hosts_err_msg='';
var hosts_sync=(cb)=>{
  if(typeof cb=='undefined')cb=()=>{};
  xhr_get('https://raw.githubusercontent.com/adler3d/qap_vm/gh-pages/trash/test2017/hosts.json?t='+rand(),
    s=>{try{hosts=JSON.parse(s);}catch(e){cb('JSON.parse error:\n'+e+'\n\n'+s);return;}cb(s);},
    s=>{hosts_err_msg=s;cb(s);}
  );
};

hosts_sync((s)=>{
  //console.log(JSON.stringify(hosts));
  qap_log("xhr done //length = "+Buffer.byteLength(s));
  var shadow=mapkeys(hosts)[mapvals(hosts).indexOf('shadow')];
  var master=mapkeys(hosts)[mapvals(hosts).indexOf('public')];

  qap_log(master);
  
  var test2016_h=require("process").argv[2];
  //qap_log(fs.readFileSync(test2016_h));
  xhr_post("http://"+master+'/put?fn='+test2016_h,{data:fs.readFileSync(test2016_h).toString("binary")},s=>{
    //fs.writeFileSync(fn,s);
    qap_log("xhr done //length = "+Buffer.byteLength(s));
    qap_log(s);
  },qap_err('xhr_get fail'));

  if(0){
    var fn="http://adler3d.github.io/qap_vm/trash/test2017/hosts.json";
    xhr_get(fn+'?t='+rand(),s=>{
      //fs.writeFileSync(fn,s);
      qap_log("xhr done //length = "+Buffer.byteLength(s));
      console.log(s);
    },qap_err('xhr_get fail'));
    // unused due to 'hosts_sync'
  }
});
