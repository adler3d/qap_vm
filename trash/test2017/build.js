var clock=(start)=>{var f=(e)=>((e[0]*1000)+(e[1]/1000000));if(!start)return f(process.hrtime());var end=process.hrtime(start);return f(end);}
bef=clock();
//require('child_process').execSync("build");
{
  //this is js code for build time measure
  const spawn=require('child_process').spawn,ls=spawn('build.bat',[],{stdio:"inherit"});//spawn('ls',['-lh','/usr']);
  //ls.stdout.on('data',data=>{process.stdout.write(data);});
  //ls.stderr.on('data',data=>{process.stderr.write(data);});
  ls.on('close',code=>{console.log("\nchild process exited with code "+code);var t=clock()-bef;process.stdout.write("\nemcc_time = "+t.toFixed(2)+"ms");});
  ls.on('error',er=>{console.error(er);});
}