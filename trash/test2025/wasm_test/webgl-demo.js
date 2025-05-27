var qDev={
  parr:[-1,-1,+1,-1,+1,+1,-1,+1].map(e=>e*256),
  carr:[
    1.0,
    1.0,
    1.0,
    1.0, // white
    1.0,
    0.0,
    0.0,
    1.0, // red
    0.0,
    1.0,
    0.0,
    1.0, // green
    0.0,
    0.0,
    1.0,
    1.0, // blue
  ],
  tarr:[0,0,1,0,1,1,0,1],
  iarr:[0,1,2,0,2,3],
  parr_buff:null,
  carr_buff:null,
  iarr_buff:null,
  gl:null,
  prog:null,
  fill_parr:(qDev)=>{
    let gl=qDev.gl;
    if(!qDev.parr_buff)qDev.parr_buff=gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER,qDev.parr_buff);
    gl.bufferData(gl.ARRAY_BUFFER,new Float32Array(qDev.parr),gl.DYNAMIC_DRAW);
  },
  fill_carr:(qDev)=>{
    let gl=qDev.gl;
    if(!qDev.carr_buff)qDev.carr_buff=gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER,qDev.carr_buff);
    gl.bufferData(gl.ARRAY_BUFFER,new Float32Array(qDev.carr),gl.DYNAMIC_DRAW);
  },
  fill_iarr:(qDev)=>{
    let gl=qDev.gl;
    if(!qDev.iarr_buff)qDev.iarr_buff=gl.createBuffer();
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER,qDev.iarr_buff);
    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER,new Uint32Array(qDev.iarr),gl.DYNAMIC_DRAW);
  },
  fill_tarr:(qDev)=>{
    let gl=qDev.gl;
    if(!qDev.tarr_buff)qDev.tarr_buff=gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER,qDev.tarr_buff);
    gl.bufferData(gl.ARRAY_BUFFER,new Float32Array(qDev.tarr),gl.DYNAMIC_DRAW);
  },
  set_parr:(qDev,prog)=>{
    let gl=qDev.gl;
    gl.bindBuffer(gl.ARRAY_BUFFER,qDev.parr_buff);
    const numComponents=2;
    const type = gl.FLOAT;
    const normalize=false;
    const stride=0;
    const offset=0;
    gl.vertexAttribPointer(
      prog.attribLocations.vpos,
      numComponents,
      type,
      normalize,
      stride,
      offset
    );
    gl.enableVertexAttribArray(prog.attribLocations.vpos);
  },
  set_carr:(qDev,prog)=>{
    let gl=qDev.gl;
    gl.bindBuffer(gl.ARRAY_BUFFER,qDev.carr_buff);
    const numComponents=4;
    const type = gl.FLOAT;//gl.UNSIGNED_BYTE;
    const normalize=false;
    const stride=0;
    const offset=0;
    gl.vertexAttribPointer(
      prog.attribLocations.vcol,
      numComponents,
      type,
      normalize,
      stride,
      offset
    );
    gl.enableVertexAttribArray(prog.attribLocations.vcol);
  },
  set_iarr:qDev=>{
    let gl=qDev.gl;
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER,qDev.iarr_buff);
  },
  set_tarr:(qDev,prog)=>{
    let gl=qDev.gl;
    gl.bindBuffer(gl.ARRAY_BUFFER,qDev.tarr_buff);
    const num=2;
    const type=gl.FLOAT;
    const normalize = false;
    const stride=0;
    const offset=0;
    gl.vertexAttribPointer(
      prog.attribLocations.vtex,
      num,
      type,
      normalize,
      stride,
      offset
    );
    gl.enableVertexAttribArray(prog.attribLocations.vtex);
  },
  fill_buffs:qDev=>{
    qDev.fill_parr(qDev);
    qDev.fill_carr(qDev);
    qDev.fill_iarr(qDev);
    qDev.fill_tarr(qDev);
  },
  set_buffs:(qDev)=>{
    qDev.set_parr(qDev,qDev.prog);
    qDev.set_carr(qDev,qDev.prog);
    qDev.set_iarr(qDev,qDev.prog);
    qDev.set_tarr(qDev,qDev.prog);
  },
  DIP:(qDev)=>{
    let gl=qDev.gl;
    qDev.fill_buffs(qDev);
    qDev.set_buffs(qDev);
    gl.drawElements(gl.TRIANGLES,qDev.iarr.length,gl.UNSIGNED_INT,0);
    qDev.buffs_pos++;
    if(qDev.buffs_pos>=qDev.buffs.length){
      qDev.buffs.push({
        parr_buff:qDev.parr_buff,
        carr_buff:qDev.carr_buff,
        tarr_buff:qDev.tarr_buff,
        iarr_buff:qDev.iarr_buff,
      });
      qDev.parr_buff=null;
      qDev.carr_buff=null;
      qDev.tarr_buff=null;
      qDev.iarr_buff=null;
    }else{
      let e=qDev.buffs[qDev.buffs_pos];
      qDev.parr_buff=e.parr_buff;
      qDev.carr_buff=e.carr_buff;
      qDev.tarr_buff=e.tarr_buff;
      qDev.iarr_buff=e.iarr_buff;
    }
  },
  buffs:[],
  buffs_pos:0,
  NextFrame:(qDev)=>{
    qDev.buffs_pos=0;
    if(qDev.buffs.length==0)return;
    let e=qDev.buffs[qDev.buffs_pos];
    qDev.parr_buff=e.parr_buff;
    qDev.carr_buff=e.carr_buff;
    qDev.tarr_buff=e.tarr_buff;
    qDev.iarr_buff=e.iarr_buff;
  }
};
function initFont(fontSize,fontFamily,texSize,bold){
  var out={W:[],H:[],wh:texSize};
  var canvas=document.createElement('canvas');
  canvas.style="display:none";
  var ctx=canvas.getContext('2d');
  document.body.appendChild(canvas);
  canvas.width=texSize;canvas.height=texSize;
  //ctx.textAlign    = 'left';
  //ctx.textBaseline = 'base';
  ctx.fillStyle = 'black';
  ctx.fillRect(0,0,canvas.width,canvas.height);
  ctx.fillStyle    = 'white';
  //ctx.textAlign    = 'left';
  //ctx.textBaseline = 'base';
  //ctx.font         = "20px Consolas";//fontSize+'px '+fontFamily;
  //out.font=fontSize+'px '+fontFamily;
  bold=true;//fontFamily="Consolas";
  ctx.font         = (bold?"bold ":"")+fontSize+'pt '+fontFamily;//out.font;
  let m=ctx.measureText('_');let gH=m.fontBoundingBoxAscent+m.fontBoundingBoxDescent;
  for(let j=0;j<=255;j++){
    let c=String.fromCharCode(j);let i=j;
    let m=ctx.measureText(c);
    let x=(i%16)*(texSize/16);let y=((i/16)|0)*(texSize/16)-gH/4;
    let H=gH;//m.fontBoundingBoxAscent+m.fontBoundingBoxDescent;
    ctx.fillText(c,x,y+H);
    out.W[i]=m.width;
    out.H[i]=H;
  }
  out.data=ctx.getImageData(0,0,texSize,texSize).data;
  out.ctx=ctx;
  return out;
}
function initFont_v2(font,dest,pW,pH){
  let p=font.data;
  let n=font.wh*font.wh*4;
  for(let i=0;i<n;i+=4){
    HEAPU8[dest+i+0]=255;
    HEAPU8[dest+i+1]=255;
    HEAPU8[dest+i+2]=255;
    HEAPU8[dest+i+3]=p[i+0];//!=255?0:255;
  }
  n=font.W.length;
  p=font.W;
  d=pW>>2;
  for(let i=0;i<n;i++){
    HEAPU32[d+i]=p[i];
  }
  n=font.H.length;
  p=font.H;
  d=pH>>2;
  for(let i=0;i<n;i++){
    HEAPU32[d+i]=p[i];
  }
}
function drawScene(gl,prog,buffers,squareRotation,texture) {
  let c=210/255;
  gl.clearColor(c,c,c,1.0);
  gl.clear(gl.COLOR_BUFFER_BIT);
  gl.disable(gl.DEPTH_TEST);
  //gl.clearDepth(1.0);
  //gl.enable(gl.DEPTH_TEST);
  //gl.depthFunc(gl.LEQUAL);// Near things obscure far things

  // Clear the canvas before we start drawing on it.

  //gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

  gl.enable(gl.BLEND);
  gl.blendFunc(gl.SRC_ALPHA,gl.ONE_MINUS_SRC_ALPHA);

  var OrthoLH=(w,h,zn,zf)=>{
    let mat=mat4.fromValues(
      2.0/w,0,0,0,
      0,2.0/h,0,0,
      0,0,1/(zf-zn),0,
      0,0,-zn/(zf-zn),1
    );
    return mat;
  }
  let w=gl.canvas.clientWidth
  let h=gl.canvas.clientHeight;
  let projectionMatrix=OrthoLH(w,h,0.1,100);

  // Set the drawing position to the "identity" point, which is
  // the center of the scene.
  const modelViewMatrix = mat4.create();

  // Now move the drawing position a bit to where we want to
  // start drawing the square.
  mat4.translate(
    modelViewMatrix, // destination matrix
    modelViewMatrix, // matrix to translate
    [-0.0, 0.0, +0.5]
  ); // amount to translate


  // Tell WebGL to use our program when drawing
  gl.useProgram(prog.program);

  // Set the shader uniforms
  gl.uniformMatrix4fv(
    prog.uniformLocations.projectionMatrix,
    false,
    projectionMatrix
  );
  gl.uniformMatrix4fv(
    prog.uniformLocations.modelViewMatrix,
    false,
    modelViewMatrix
  );
  gl.activeTexture(gl.TEXTURE0);
  gl.bindTexture(gl.TEXTURE_2D,texture);
  gl.uniform1i(prog.uniformLocations.uSampler,0);

  qDev.prog=prog;
  //qDev.parr[0]=squareRotation;
  qDev.NextFrame(qDev);
  Module.ccall('update','int',["int"],[0]);
  //qDev.DIP(qDev);
}

var squareRotation=-256;

function main() {
  const canvas = document.querySelector("#glcanvas");
  const gl = canvas.getContext("webgl",{ alpha: false,premultipliedAlpha: false});
  const ext = gl.getExtension("OES_element_index_uint");
  if (gl===null||ext==null) {
    alert(
      "Unable to initialize WebGL. Your browser or machine may not support it."
    );
    return;
  }

  let c=210/255;
  gl.clearColor(c,c,c,1.0);
  gl.clear(gl.COLOR_BUFFER_BIT);
  
  const vsSource = `
    attribute vec4 vpos;
    attribute vec4 vcol;
    attribute vec2 vtex;

    uniform mat4 uModelViewMatrix;
    uniform mat4 uProjectionMatrix;

    varying highp vec4 vColor;
    varying highp vec2 vTextureCoord;

    void main(void) {
      gl_Position = uProjectionMatrix * uModelViewMatrix * vpos;
      vColor = vcol;
      vTextureCoord = vtex;
    }
  `;


  const fsSource = `
    precision highp float;
    varying highp vec4 vColor;
    varying highp vec2 vTextureCoord;
    uniform sampler2D uSampler;

    void main(void) {
      vec4 t=texture2D(uSampler,vTextureCoord);
      gl_FragColor = t*vColor;
      gl_FragColor.rgb *= gl_FragColor.a;
    }
  `;

  const shaderProgram = initShaderProgram(gl, vsSource, fsSource);

  const prog={
    program:shaderProgram,
    attribLocations:{
      vpos:gl.getAttribLocation(shaderProgram,"vpos"),
      vcol:gl.getAttribLocation(shaderProgram,"vcol"),
      vtex:gl.getAttribLocation(shaderProgram,"vtex"),
    },
    uniformLocations:{
      projectionMatrix:gl.getUniformLocation(shaderProgram,"uProjectionMatrix"),
      modelViewMatrix: gl.getUniformLocation(shaderProgram,"uModelViewMatrix"),
      uSampler:        gl.getUniformLocation(shaderProgram,"uSampler"),
    },
  };

  qDev.gl=gl;

  const texture = loadTexture(gl, "cubetexture.png");
  gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);

  let then = 0;

  function render(now) {
    now *= 0.001;
    deltaTime = now - then;
    then = now;
    drawScene(gl, prog, null, squareRotation,texture);
    //squareRotation += deltaTime;
    requestAnimationFrame(render);
  }
  requestAnimationFrame(render);
}

function start(){
  Module.ccall('qap_main','int',["int"],[0]);
}

function initShaderProgram(gl, vsSource, fsSource) {
  const vertexShader = loadShader(gl, gl.VERTEX_SHADER, vsSource);
  const fragmentShader = loadShader(gl, gl.FRAGMENT_SHADER, fsSource);
  const shaderProgram = gl.createProgram();
  gl.attachShader(shaderProgram, vertexShader);
  gl.attachShader(shaderProgram, fragmentShader);
  gl.linkProgram(shaderProgram);
  if (!gl.getProgramParameter(shaderProgram, gl.LINK_STATUS)) {
    alert(
      `Unable to initialize the shader program: ${gl.getProgramInfoLog(
        shaderProgram
      )}`
    );
    return null;
  }
  return shaderProgram;
}

function loadShader(gl, type, source) {
  const shader = gl.createShader(type);
  gl.shaderSource(shader, source);
  gl.compileShader(shader);
  if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
    alert(
      `An error occurred compiling the shaders: ${gl.getShaderInfoLog(shader)}`
    );
    gl.deleteShader(shader);
    return null;
  }
  return shader;
}
function bindTex(qDev,texture){
  let gl=qDev.gl;
  gl.activeTexture(gl.TEXTURE0);
  gl.bindTexture(gl.TEXTURE_2D,g_textures[texture]);
}
g_textures=[null];
function makeTexture(qDev,w,h,pix){
  let gl=qDev.gl;
  let pixel=new Uint8Array(HEAPU8.buffer,pix,w*h*4);
  const texture=gl.createTexture();
  gl.bindTexture(gl.TEXTURE_2D,texture);
  const level = 0;
  const internalFormat = gl.RGBA;
  const width = w;
  const height = h;
  const border = 0;
  const srcFormat = gl.RGBA;
  const srcType = gl.UNSIGNED_BYTE;
  gl.texImage2D(
    gl.TEXTURE_2D,
    level,
    internalFormat,
    width,
    height,
    border,
    srcFormat,
    srcType,
    pixel
  );
  gl.generateMipmap(gl.TEXTURE_2D);
  let tex_id=g_textures.length;
  g_textures.push(texture);
  return tex_id;
}
function loadTexture(gl, url) {
  const texture = gl.createTexture();
  gl.bindTexture(gl.TEXTURE_2D, texture);
  const level = 0;
  const internalFormat = gl.RGBA;
  const width = 1;
  const height = 1;
  const border = 0;
  const srcFormat = gl.RGBA;
  const srcType = gl.UNSIGNED_BYTE;
  const pixel = new Uint8Array([0, 0, 255, 255]); // opaque blue
  gl.texImage2D(
    gl.TEXTURE_2D,
    level,
    internalFormat,
    width,
    height,
    border,
    srcFormat,
    srcType,
    pixel
  );

  const image = new Image();
  image.onload = () => {
    gl.bindTexture(gl.TEXTURE_2D, texture);
    gl.texImage2D(
      gl.TEXTURE_2D,
      level,
      internalFormat,
      srcFormat,
      srcType,
      image
    );
    if(isPowerOf2(image.width) && isPowerOf2(image.height)){
      gl.generateMipmap(gl.TEXTURE_2D);
    }else{
      gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
      gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
      gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
    }
  };
  image.src=url;
  return texture;
}

function isPowerOf2(value) {
  return (value & (value - 1)) === 0;
}

//Module.onRuntimeInitialized=()=>start();
var g_start_int=setInterval(()=>{
  if((typeof start !== 'undefined')&&
     (typeof Module !== 'undefined')&&
     ('ccall' in Module)&&
     (typeof wasmExports !== 'undefined')
    ){
    clearInterval(g_start_int);start();
  };
},1000);
