https://adler3d.github.io/qap_vm/trash/test2025/gd20250525/v0.html  
https://adler3d.github.io/qap_vm/trash/test2025/gd20250525/v1.html  

```diff
diff --git a/./v0.html b/./v1.html
index ecc0812..1644582 100644
--- a/./v0.html
+++ b/./v1.html
@@ -122,15 +122,15 @@ var qDev={
   fill_buffs:qDev=>{
     qDev.fill_parr(qDev);
     qDev.fill_carr(qDev);
     qDev.fill_iarr(qDev);
-    //qDev.fill_tarr(qDev);
+    qDev.fill_tarr(qDev);
   },
   set_buffs:(qDev)=>{
     qDev.set_parr(qDev,qDev.prog);
     qDev.set_carr(qDev,qDev.prog);
     qDev.set_iarr(qDev,qDev.prog);
-    //qDev.set_tarr(qDev,qDev.prog);
+    qDev.set_tarr(qDev,qDev.prog);
   },
   DIP:(qDev)=>{
     let gl=qDev.gl;
     qDev.fill_buffs(qDev);
```

https://adler3d.github.io/qap_vm/trash/test2025/gd20250525/v2.html  
```diff
diff --git a/./v1.html b/./v2.html
index 1644582..f4f07cc 100644
--- a/./v1.html
+++ b/./v2.html
@@ -249,11 +249,9 @@ function main() {
     varying highp vec2 vTextureCoord;
     uniform sampler2D uSampler;

     void main(void) {
-      //gl_FragColor = /*texture2D(uSampler,vTextureCoord)**/vColor;
-      //gl_FragColor = texture2D(uSampler,vTextureCoord);//vec4(1.0,1.0,1.0,1.0);
-      gl_FragColor = vColor;
+      gl_FragColor = texture2D(uSampler,vTextureCoord)*vColor;
     }
   `;

   const shaderProgram = initShaderProgram(gl, vsSource, fsSource);
```
