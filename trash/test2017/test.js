document.title="em_test.js";
var canvas;
var gl;
var VBO;
var VCO;
var mvMatrix;
var shaderProgram;
var vertexPositionAttribute;
var vertexColorAttribute;
var perspectiveMatrix;
var IBO;
function js_main(){/*Array.from(document.getElementsByTagName("input")).filter(e=>e.value=='Fullscreen')[0].click();*/start();}
function start() {
  canvas = document.getElementById("canvas");
  //canvas.width=640;
  //canvas.height=480;
  initWebGL(canvas);

  if (gl) {
    gl.clearColor(0.6, 0.6, 0.6, 1.0);
    gl.disable(gl.CULL_FACE);
    gl.cullFace(gl.FRONT_AND_BACK);
    //gl.clearDepth(1.0);
    gl.disable(gl.DEPTH_TEST);
    //gl.depthFunc(gl.LEQUAL);

    gl.enable(gl.BLEND);
    gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);// BT_SUB
    
    initShaders();

    initBuffers();

    //setInterval(drawScene, 15);
    
  }
}

function initWebGL() {
  gl = null;
  try {
    gl = canvas.getContext("experimental-webgl",{antialias: false});
  }
  catch(e) {
  }
  if (!gl) {
    alert("Unable to initialize WebGL. Your browser may not support it.");
  }
}
function js_one_iter(){
  if(g_Indices&&g_need_step)drawScene();
  if(g_mem_offset)if(g_need_step)g_need_step--;
  return 0;
}
var g_need_step=9000;
var g_mem_offset=0;
var g_mem_head=0;
var g_mem_size=0;var g_Indices=null;
// var z=[];for(var i=0;i<1224*1024;i++)if(HEAP16[(g_mem_offset>>1)+i]==6700)z.push(((g_mem_offset>>1)+i)<<1);
function initBuffers() {

  if(!IBO)if(!gl.getExtension('OES_element_index_uint'))
  {
    console.error("OES_element_index_uint - not supported");throw "no way";
  };
  /*
  var out=document.getElementById("output").value.split("\n")[0].split(",").map(e=>e|0);
  var g_mem_offset=out[0];
  var head=out[1];
  var size=out[2];
  var full=addr=>{
    var m=(addr&4);
    if(!m)return HEAP32[addr>>2];
    var i=4-m;
    var a=HEAP32[0+(addr>>2)]<< (m*8);
    var b=HEAP32[1+(addr>>2)]>>>(i*8);
    return a|b;
  };
  var f=id=>{
    var addr=(g_mem_offset+(head+id)*4);
    var v8=(HEAP8[addr+0]<<8*0)|(HEAP8[addr+1]<<8*1)|(HEAP8[addr+2]<<8*2)|(HEAP8[addr+3]<<8*3);

    var v32=full(addr);
    if(v32!=v8){console.error([v8,v32]);throw "no way";}
    return v32;
  }
  //for(var i=0;i<512*1024;i++){
  //  va[i]={color:read(),pos:[read(),read()]};
  //}
  var gv=pos=>HEAP32[(head>>2)+pos]
  var pos=head;
  var read=()=>{pos++;return f(pos-1);};
  read();*/
  
  var pos=(g_mem_offset+(g_mem_head+0)*4)>>>2;
  var read=()=>{var prev=pos;pos++;return HEAP32[prev];};
  var vn=read();var va=[];
  if(0)for(var i=0;i<vn;i++){
    va[i]={color:read(),pos:[read(),read()]};
  }
  
  {
    var s=100;
    var w=0.5*s;var h=0.5*s;var qqqx=200;var qqqy=200;
    var vertices = [
      -w, -h,  0.0,
      +w, -h,  0.0,
      +w, +h, 0.0,
      -w, +h, 0.0,
      -w+qqqx, -h+qqqy,  0.0,
      +w+qqqx, -h+qqqy,  0.0,
      +w+qqqx, +h+qqqy, 0.0,
      -w+qqqx, +h+qqqy, 0.0,
    ];
    if(0)for(var i=0;i<4;i++){
      vertices[i*4+0]-=0.5;
      vertices[i*4+1]-=0.5;
    }
    
    var VA=vertices;VA.length=vn*3;var ca=[];ca.length=vn;
    for(var i=0;i<vn;i++)
    {
      ca[i]=read();
      VA[i*3+0]=read()*0.25;
      VA[i*3+1]=read()*0.25;
      VA[i*3+2]=0;
    }
    
    if(!VBO)VBO=gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, VBO);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.DYNAMIC_DRAW);
  }
  
  {
    var colors = [
      1.0,  1.0,  1.0,  1.0,    // white
      1.0,  0.0,  0.0,  1.0,    // red
      0.0,  1.0,  0.0,  1.0,    // green
      0.0,  0.0,  1.0,  1.0,     // blue
    ];
    for(var i=0;i<6*4*2;i++)colors[i]=1.0;
   
    var CA=colors;CA.length=vn*4;var d=1.0/255.0;
    for(var i=0;i<vn;i++){var p=ca[i];for(var j=0;j<4;j++)CA[i*4+j]=((p>>>8*j)&0xff)*d;}

    if(!VCO)VCO = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, VCO);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(colors), gl.DYNAMIC_DRAW);
  }
  {
    var Indices = [ 3, 0, 1,
                    4+1, 4+0, 4+3,
                    3, 2, 1,
                    4+3, 4+2, 4+1,
    ];
    
    
    var IN=read();//console.log("Indices.length = "+IN);
    Indices.length=IN;
    for(var i=0;i<IN;i++)Indices[i]=read();
    //console.log("pos = "+pos);
    g_Indices=Indices;
    
    if(!IBO)IBO=gl.createBuffer();
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, IBO);
    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint32Array(Indices), gl.DYNAMIC_DRAW);
  }
}

function OrthoLH(w,h,zn,zf)
{
  return $M([
    2.0/w,0,0,0,
    0,2.0/h,0,0,
    0,0,1/(zf-zn),0,
    0,0,-zn/(zf-zn),1
  ]);
}

function drawScene() {
  initBuffers();
  gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

  //perspectiveMatrix = makePerspective(45, 640.0/480.0, 0.1, 100.0);
  perspectiveMatrix = OrthoLH(canvas.width|0,canvas.height|0,-1,+1);

  loadIdentity();

  //mvTranslate([-0.0, 0.0, -0.0]);

  gl.bindBuffer(gl.ARRAY_BUFFER, VBO);
  gl.vertexAttribPointer(vertexPositionAttribute, 3, gl.FLOAT, false, 0, 0);

  gl.bindBuffer(gl.ARRAY_BUFFER, VCO);
  gl.vertexAttribPointer(vertexColorAttribute, 4, gl.FLOAT, false, 0, 0);

  gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, IBO);


  setMatrixUniforms();
  gl.drawElements(gl.TRIANGLES, g_Indices.length, gl.UNSIGNED_INT, 0);
  //gl.drawArrays(gl.TRIANGLE_STRIP, 0, 6);
}

function initShaders() {
  var fragmentShader = getCompiledShader(gl,gl.createShader(gl.FRAGMENT_SHADER),`
      varying lowp vec4 vColor;

      void main(void) {
        gl_FragColor = vColor;
      }
  `);
  var vertexShader = getCompiledShader(gl,gl.createShader(gl.VERTEX_SHADER),`
      attribute vec3 aVertexPosition;
      attribute vec4 aVertexColor;

      uniform mat4 uMVMatrix;
      uniform mat4 uPMatrix;

      varying lowp vec4 vColor;

      void main(void) {
        gl_Position = uPMatrix * uMVMatrix * vec4(aVertexPosition, 1.0);
        vColor = aVertexColor;
      }
  `);

  shaderProgram = gl.createProgram();
  gl.attachShader(shaderProgram, vertexShader);
  gl.attachShader(shaderProgram, fragmentShader);
  gl.linkProgram(shaderProgram);

  if (!gl.getProgramParameter(shaderProgram, gl.LINK_STATUS)) {
    alert("Unable to initialize the shader program: " + gl.getProgramInfoLog(shader));
  }

  gl.useProgram(shaderProgram);

  vertexPositionAttribute = gl.getAttribLocation(shaderProgram, "aVertexPosition");
  gl.enableVertexAttribArray(vertexPositionAttribute);

  vertexColorAttribute = gl.getAttribLocation(shaderProgram, "aVertexColor");
  gl.enableVertexAttribArray(vertexColorAttribute);
}

function getShader(gl, id) {
  var shaderScript = document.getElementById(id);

  if (!shaderScript) {
    return null;
  }

  var theSource = "";
  var currentChild = shaderScript.firstChild;

  while(currentChild) {
    if (currentChild.nodeType == 3) {
      theSource += currentChild.textContent;
    }

    currentChild = currentChild.nextSibling;
  }

  var shader;

  if (shaderScript.type == "x-shader/x-fragment") {
    shader = gl.createShader(gl.FRAGMENT_SHADER);
  } else if (shaderScript.type == "x-shader/x-vertex") {
    shader = gl.createShader(gl.VERTEX_SHADER);
  } else {
    return null;  // Unknown shader type
  }
  return getCompiledShader(gl,shader,theSource);
}

function getCompiledShader(gl,shader,theSource) {
  gl.shaderSource(shader, theSource);

  gl.compileShader(shader);

  if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
    alert("An error occurred compiling the shaders: " + gl.getShaderInfoLog(shader));
    return null;
  }

  return shader;
}

function loadIdentity() {
  mvMatrix = Matrix.I(4);
}

function multMatrix(m) {
  mvMatrix = mvMatrix.x(m);
}

function mvTranslate(v) {
  multMatrix(Matrix.Translation($V([v[0], v[1], v[2]])).ensure4x4());
}

function setMatrixUniforms() {
  var pUniform = gl.getUniformLocation(shaderProgram, "uPMatrix");
  gl.uniformMatrix4fv(pUniform, false, new Float32Array(perspectiveMatrix.flatten()));

  var mvUniform = gl.getUniformLocation(shaderProgram, "uMVMatrix");
  gl.uniformMatrix4fv(mvUniform, false, new Float32Array(mvMatrix.flatten()));
}