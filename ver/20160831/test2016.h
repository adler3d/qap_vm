{
  int g_ptr_addr(){return 1024*(64+7);}
  int g_mem_addr(){return 1024*(64+7)+8;}
  int g_arr_addr(){return 1024*(64+7)+16;}
  int CatmullRomEx(int p1,int p2,int p3,int p4,int t,int z){
    int a=p2;
    int b=p3-p1;
    int c=2*p1-5*p2+4*p3-p4;
    int d=3*(p2-p3)+p4-p1;
    d=(d*t)/z;
    d+=c;
    d=(d*t)/z;
    d+=b;
    d=(d*t)/z;
    return a+d/2;
  }
  vec2i VCatmullRom(vec2i a_,vec2i b,vec2i c,vec2i d_,int t,int z)
  {
    vec2i a=a_;vec2i d=d_;
    int m=hypot(sub(b,c));
    {
      vec2i v=sub(a,b);
      int D=hypot(v);
      if(D)
      {
        a=add(b,div(mul(v,m),D));
      }
    }
    {
      vec2i v=sub(d,c);
      int D=hypot(v);
      if(D)
      {
        d=add(c,div(mul(v,m),D));
      }
    }
    return vec2i(
      CatmullRomEx(a.x,b.x,c.x,d.x,t,z),
      CatmullRomEx(a.y,b.y,c.y,d.y,t,z)
    );
  }
  int alloc(int size){
    int g_addr=g_ptr_addr();
    int g_ptr=get_int(g_addr);
    if(!g_ptr){
      int ram_base=(1024*(64+8));
      g_ptr=ram_base;
    }
    set_int(g_addr,g_ptr+size);
    return g_ptr;
  }
  struct vec2i{
    int x;
    int y;
    int mag(){return hypot(x,y);}
  };
  vec2i vec2i(int x,int y){vec2i tmp;tmp.x=x;tmp.y=y;return tmp;};
  int add(int addr,int v){return set_int(addr,v);}
  int hypot(vec2i v){return hypot(v.x,v.y);}
  vec2i sub(vec2i a,vec2i b){return vec2i(a.x-b.x,a.y-b.y);}
  vec2i add(vec2i a,vec2i b){return vec2i(a.x+b.x,a.y+b.y);}
  vec2i mul(vec2i a,int k){return vec2i(a.x*k,a.y*k);}
  vec2i div(vec2i a,int k){return vec2i(a.x/k,a.y/k);}
  // -----=====
  int get_int(int addr){int out=0;__asm("mov(eax,param_addr);mov(var_out,ptr[eax]);");return out;}
  int set_int(int addr,int v){__asm("mov(eax,param_addr);mov(ptr[eax],param_v);");return addr+1;}
  int setebx(int value){__asm("mov(ebx,param_value);");return 0;}
  int hypot(int x,int y){int out=0;__asm("mov(eax,param_x);mov(ebx,param_y);hypot(eax,ebx);mov(var_out,eax);");return out;}
  int sqrt(int v){int out=0;__asm("sqrt(eax,param_v);mov(var_out,eax);");return out;}
  // -----=====
  int QapAssert(int cond){if(!cond){__asm("label(\"Run-time error!\");mov(err,-1);");};}
  int QapAssert(int A,int B){if(A!=B){__asm("label(\"Run-time error!\");mov(err,-1);");};}
  int min(int a,int b){if(a<b)return a;return b;}
  int max(int a,int b){if(a>b)return a;return b;}
  int clamp(int low,int top,int value){return min(max(low,value),top);}
  struct qap_vertex{
    int color;
    vec2i pos;
  };
  qap_vertex qap_vertex(int color,vec2i pos){qap_vertex tmp;tmp.color=color;tmp.pos=pos;return tmp;}
  struct vector_qap_vertex{
    int p;int n;int cap;
    int reserve(int c){p=alloc(3*c);n=0;cap=c;return 0;}
    int sys_add(int ref){p=set_int(p,ref);return p;}
    int sys_add(vec2i ref){sys_add(ref.x);sys_add(ref.y);return p;}
    int add(int color,vec2i pos){QapAssert(n<cap);sys_add(color);sys_add(pos);n++;return p;}
    int add(qap_vertex ref){QapAssert(n<cap);sys_add(ref.color);sys_add(ref.pos);n++;return p;}
    vec2i get_vec2i(int addr){
      return vec2i(get_int(addr),get_int(addr+1));
    }
    qap_vertex get(int addr){
      return qap_vertex(get_int(addr),get_vec2i(addr+1));
    }
    qap_vertex at(int id){
      return get(p+(id-n)*3);
    }
    vector_int to_vector_int(){
      vector_int tmp;tmp.p=p;tmp.n=n*3;tmp.cap=cap*3;
      return tmp;
    }
    qap_vertex front(){QapAssert(n>0);return at(0);}
    qap_vertex back(){QapAssert(n>0);return at(n-1);}
    vector_qap_vertex clone(){
      vector_qap_vertex tmp;
      tmp.reserve(cap);
      for(int i=0;i<n;i++)tmp.add(at(i));
      return tmp;
    }
    int is_equal(vector_qap_vertex VA)
    {
      QapAssert(VA.n,n);
      for(int i=0;i<n;i++)
      {
        auto a=VA.at(i);
        auto b=at(i);
        QapAssert(a.color,b.color);
        QapAssert(a.pos.x,b.pos.x);
        QapAssert(a.pos.y,b.pos.y);
      }
    }
    int addr_at(int id){return p+(id-n)*3;}
    int add(vector_qap_vertex VA)
    {
      auto did=addr_at(n);auto sid=VA.addr_at(0);
      int van=VA.n*3;
      __asm("mov(edi,var_did);mov(esi,var_sid);");
      __asm("mov(ecx,var_van);");
      __asm("rep_movsd_inc();");
      __asm("mov(ecx,this_ptr);");
      n+=VA.n;
      p+=VA.n*3;
    }
    int add_with_offset(vector_qap_vertex VA,vec2i offset)
    {
      auto did=addr_at(n);auto sid=VA.addr_at(0);
      int x=offset.x;
      int y=offset.y;
      int van=VA.n;
      __asm("mov(edi,var_did);mov(esi,var_sid);");
      __asm("mov(eax,var_x);mov(ebx,var_y);");
      __asm("mov(ecx,var_van);");
      __asm("{");
      __asm("  LOOP_SCOPE();");
      __asm("  movsd_inc();");
      __asm("  addsd_inc(eax);addsd_inc(ebx);");
      __asm("}");
      __asm("mov(ecx,this_ptr);");
      n+=VA.n;
      p+=VA.n*3;
    }
    int add_with_color(vector_qap_vertex VA,int color,vec2i offset)
    {
      auto did=addr_at(n);auto sid=VA.addr_at(0);
      int x=offset.x;
      int y=offset.y;
      int van=VA.n;
      __asm("mov(edi,var_did);mov(esi,var_sid);");
      __asm("mov(ebx,var_x);mov(edx,var_y);");
      __asm("mov(ecx,var_van);");
      __asm("{");
      __asm("  LOOP_SCOPE();");
      __asm("  inc(esi);mov(eax,param_color);stosd_inc();");
      __asm("  addsd_inc(ebx);addsd_inc(edx);");
      __asm("}");
      __asm("mov(ecx,this_ptr);");
      n+=VA.n;
      p+=VA.n*3;
    }
  };
  struct vector_int{
    int p;int n;int cap;
    int reserve(int c){p=alloc(1*c);n=0;cap=c;return 0;}
    int add(int ref){QapAssert(n<cap);p=set_int(p,ref);n++;return p;}
    int get(int addr){
      return get_int(addr);
    }
    int at(int id){
      return get(p+id-n);
    }
    int front(){QapAssert(n>0);return at(0);}
    int back(){QapAssert(n>0);return at(n-1);}
    int addr_at(int id){return p+(id-n)*1;}
    int add_mega_fast(vector_int IA,int vpos){
      int sp=IA.p-IA.n;
      int c=IA.n;
      __asm("mov(edx,param_vpos);mov(eax,field_p);");
      for(;c;c--){
        //ptr[p]=ptr[sp];
        __asm("mov(ebx,var_sp);mov(ebx,ptr[ebx]);add(ebx,edx);mov(ptr[eax],ebx);inc(eax);");
        sp++;
      }
      n+=IA.n;
      p+=IA.n;
    }
  };
  struct vector_vec2i{
    int p;int n;
    int sys_add(int ref){p=set_int(p,ref);return p;}
    int add(vec2i ref){sys_add(ref.x);sys_add(ref.y);n++;return p;}
    vec2i get(int addr){
      return vec2i(get_int(addr),get_int(addr+1));
    }
    int addr_at(int id){return p+(id-n)*2;}
    vec2i at(int id){
      return get(p+(id-n)*2);
    }
    int CatmullRom(vector_vec2i arr,int id){
      QapAssert(arr.n>=2);
      int n=arr.n;
      vec2i a=arr.at(clamp(0,n-1,id-1));
      vec2i b=arr.at(clamp(0,n-1,id+0));
      vec2i c=arr.at(clamp(0,n-1,id+1));
      vec2i d=arr.at(clamp(0,n-1,id+2));
      int point_count=hypot(sub(b,c))/32;int z=point_count;
      for(int i=0;i<point_count;i++)
      {
        int t=(i*z)/point_count;
        add(VCatmullRom(a,b,c,d,t,z));
      }
      add(VCatmullRom(a,b,c,d,z,z));
      return p;
    }
  };
  struct t_out_dev{
    vector_qap_vertex VA;
    vector_int IA;
    int color;
    int AddTris(int a,int b,int c){IA.add(a);IA.add(b);IA.add(c);return 0;}
    int AddVertex(qap_vertex qv){VA.add(qv);return 0;}
    int AddVertex(int x,int y){VA.add(color,vec2i(x,y));return 0;}
    int AddVertex(vec2i pos){VA.add(color,pos);return 0;}
    int AddVertexAndRetVID(vec2i pos){VA.add(color,pos);return VA.n-1;}
    int add(t_out_dev geom)
    {
      QapAssert(0==geom.IA.n%3);
      IA.add_mega_fast(geom.IA,VA.n);
      VA.add(geom.VA);
      return 0;
    }
    int add_wo(t_out_dev geom,vec2i offset)
    {
      QapAssert(0==geom.IA.n%3);
      int vpos=VA.n;
      for(int i=0;i<geom.VA.n;i++){auto tmp=geom.VA.at(i);tmp.pos=add(tmp.pos,offset);VA.add(tmp);}
      for(int i=0;i<geom.IA.n;i++)IA.add(vpos+geom.IA.at(i));
      return 0;
    }
    int add_with_offset(t_out_dev geom,vec2i offset)
    {
      QapAssert(VA.n<1024*16*4);
      QapAssert(0==geom.IA.n%3);
      IA.add_mega_fast(geom.IA,VA.n);
      VA.add_with_offset(geom.VA,offset);
      return 0;
    }
    int add_with_color(t_out_dev geom,vec2i offset)
    {
      QapAssert(VA.n<1024*16*4);
      QapAssert(0==geom.IA.n%3);
      IA.add_mega_fast(geom.IA,VA.n);
      VA.add_with_color(geom.VA,color,offset);
      return 0;
    }
    int add_wc(t_out_dev geom,vec2i offset)
    {
      QapAssert(0==geom.IA.n%3);
      int vpos=VA.n;
      for(int i=0;i<geom.VA.n;i++){auto tmp=geom.VA.at(i);tmp.color=color;tmp.pos=add(tmp.pos,offset);VA.add(tmp);}
      for(int i=0;i<geom.IA.n;i++)IA.add(vpos+geom.IA.at(i));
      return 0;
    }
    t_out_dev GenGeomQuad(int w,int h){return GenGeomQuad(0,0,w,h);}
    t_out_dev GenGeomQuad(vec2i p,int w,int h){return GenGeomQuad(p.x,p.y,w,h);}
    t_out_dev GenGeomQuad(int x,int y,int w,int h)
    {
      auto geom=new_geom(1);geom.color=color;
      int hx=w/2;int hy=h/2;
      geom.AddVertex(x-hx,y-hy);
      geom.AddVertex(x+hx,y-hy);
      geom.AddVertex(x+hx,y+hy);
      geom.AddVertex(x-hx,y+hy); 
      geom.AddTris(1,0,3);
      geom.AddTris(3,2,1);
      return geom;
    }
    t_out_dev GenGeomLine(vec2i a,vec2i b,int hls){
      auto geom=new_geom(1);geom.color=color;
      auto ab=sub(b,a);
      auto n=set_mag(ort(ab),hls);
      auto d=add(ab,n);
      geom.AddVertex(add(a,mul(n,+1)));
      geom.AddVertex(add(a,mul(n,-1)));
      geom.AddVertex(add(b,mul(n,-1)));
      geom.AddVertex(add(b,mul(n,+1)));
      geom.AddTris(1,0,3);
      geom.AddTris(3,2,1);
      return geom;
    }
  };
  vec2i ort(vec2i v){return vec2i(v.y,-v.x);}
  vec2i set_mag(vec2i v,int mag){return div(mul(v,mag),v.mag());}
  t_out_dev new_geom(int n)
  {
    t_out_dev tmp;
    tmp.color=255<<24;
    tmp.VA.reserve(4*n);
    tmp.IA.reserve(6*n);
    return tmp;
  }
  // struct qap_mesh{
  //   vector{qap_vertex} VA;
  //   vector{int} IA;
  // };

  struct t_input{
    vec2i mpos;
    int zDelta;
    vector_int kb_down;
    vector_int kb_changed;
    int kb_on_down(int id){return kb_down.at(id)&&kb_changed.at(id);}
    int kb_on_up(int id){return !kb_down.at(id)&&kb_changed.at(id);}
  };
  struct t_dev{
    int p;
    int read_int(){int v=get_int(p);p++;return v;}
    vec2i read_vec2i(){return vec2i(read_int(),read_int());}
    vector_int read_vector_int(){vector_int tmp;tmp.n=read_int();p+=tmp.n;tmp.p=p;return tmp;}
    vector_int read_IA(){return read_vector_int();}
    vector_qap_vertex read_VA(){vector_qap_vertex tmp;tmp.n=read_int();p+=tmp.n*3;tmp.p=p;return tmp;}
    t_out_dev read_geom(){t_out_dev od;od.VA=read_VA();od.IA=read_IA();return od;}
    vector_vec2i read_vector_vec2i(){vector_vec2i tmp;tmp.n=read_int();p+=tmp.n*2;tmp.p=p;return tmp;}
    int write(int ref){p=set_int(p,ref);return p;}
    int write(int c,int x,int y){write(c);write(x);write(y);return p;}
    int write(vec2i pos){write(pos.x);write(pos.y);return p;}
    int write(int c,vec2i pos){write(c);write(pos.x);write(pos.y);return p;}
    int write(qap_vertex v){write(v.color);write(v.pos);return p;}
    /*int write(t_out_dev dev)
    {
      write(dev.VA.n);
      for(int i=0;i<dev.VA.n;i++)write(dev.VA.at(i));
      write(dev.IA.n);
      for(int i=0;i<dev.IA.n;i++)write(dev.IA.at(i));
    }*/
    int write(vector_vec2i arr){
      write(arr.n);
      {
        auto n=arr.n*2;auto sp=arr.addr_at(0);
        __asm("mov(esi,var_sp);mov(edi,field_p);");
        __asm("push(ecx);mov(ecx,var_n);rep_movsd_inc();pop(ecx);");
        p+=n;
      }
    }
    int write(t_out_dev dev)
    {
      write(dev.VA.n);
      {
        auto n=dev.VA.n*3;auto sp=dev.VA.addr_at(0);
        __asm("mov(esi,var_sp);mov(edi,field_p);");
        __asm("push(ecx);mov(ecx,var_n);rep_movsd_inc();pop(ecx);");
        p+=dev.VA.n*3;
      }
      write(dev.IA.n);
      {
        auto sp=dev.IA.addr_at(0);
        auto n=dev.IA.n;
        QapAssert(0,n%3);
        __asm("mov(esi,var_sp);mov(edi,field_p);");
        __asm("push(ecx);mov(ecx,var_n);rep_movsd_inc();pop(ecx);");
        p+=dev.IA.n;
      }
    }
  };
  struct t_mem{
    int ptr;
    int size;
  };
  int g_obj_addr(){return 1024*(64+7)+512;}
  vec2i get_obj_pos(t_input inp,int zoom)
  {
    auto obj=mul(vec2i(-0,0),zoom);auto obj_addr=g_obj_addr();
    if(get_int(obj_addr+2))
    {
      obj.x=get_int(obj_addr+0);
      obj.y=get_int(obj_addr+1);
    }else{
      set_int(obj_addr+2,1);
    }
    int VK_LEFT=0;int VK_RIGHT=0;
    int VK_UP=0;int VK_DOWN=0;
    __asm("mov(var_VK_LEFT,VK_LEFT);mov(var_VK_RIGHT,VK_RIGHT);");
    __asm("mov(var_VK_UP,VK_UP);mov(var_VK_DOWN,VK_DOWN);");
    int speed=2*zoom;
    if(inp.kb_down.at(VK_LEFT)){obj.x-=speed;}
    if(inp.kb_down.at(VK_RIGHT)){obj.x+=speed;}
    if(inp.kb_down.at(VK_UP)){obj.y+=speed;}
    if(inp.kb_down.at(VK_DOWN)){obj.y-=speed;}
    set_int(obj_addr+0,obj.x);
    set_int(obj_addr+1,obj.y);
    return obj;
  }
  int set_vec2i(int addr,vec2i v){set_int(addr,v.x);set_int(addr+1,v.y);return 0;}
  vector_vec2i get_points(int mposx,int mposy,int zoom){
    auto a=mul(vec2i(450, 50),zoom);
    auto b=mul(vec2i(650,150),zoom); b.x=mposx; b.y=mposy;
    auto c=mul(vec2i(300,500),zoom);
    auto d=mul(vec2i(390,550),zoom);
    vector_vec2i arr;arr.p=alloc(1024);arr.n=0;
    {
      arr.add(vec2i(0,0));
      arr.add(a);
      arr.add(b);
      arr.add(c);
      arr.add(d);
      arr.add(mul(vec2i(450,550),zoom));
    }
    {vec2i sum;for(int i=0;i<arr.n;i++)if(i!=2)sum=add(sum,arr.at(i));sum=div(sum,arr.n);
    for(int i=0;i<arr.n;i++)if(i!=2)set_vec2i(arr.addr_at(i),sub(arr.at(i),sum));}
    return arr;
  }
  t_out_dev GenGeomCatmullRom(vector_vec2i arr,int zoom){
    vector_vec2i dev;dev.p=alloc(1024*16);dev.n=0;
    for(int i=0;i+1<arr.n;i++)dev.CatmullRom(arr,i);
    auto od=new_geom(1024*4);
    int n=dev.n;
    for(int i=1;i<n;i++){
      od.add(od.GenGeomLine(dev.at(i-1),dev.at(i-0),zoom*2));
    }
    return od;
  }
  int limit255(int v){return max(0,min(255,v));}
  int sqrmag(int x,int y){return x*x+y*y;}
  int sqr(int x){return x*x;}
  t_out_dev gen_grid(vec2i mpos,int zoom)
  {
    auto od=new_geom(64*4*16*2);
    auto cell_size=1*zoom;
    auto t=16;auto h=t/2;
    for(int x=0;x<t;x++)for(int y=0;y<t;y++)
    {
      auto c=((y*255/t)<<16)+((x*255/t)<<8);
      auto a=limit255(255-sqr(sqrmag(x-h,y-h))*255/sqr(h*h));
      c=(0<<16)+(a<<8)+0;
      od.color=(a<<24)+c;
      od.add_wo(od.GenGeomQuad(cell_size,cell_size),add(sub(mpos,vec2i(h*cell_size,h*cell_size)),mul(vec2i(x,y),cell_size)));
    }
    return od;
    /*
    auto od=new_geom(64*4*16*2);
    auto quad=od.GenGeomQuad(0,0,10,10);
    auto line=new_geom(64*2*4);
    for(int x=0;x<64;x++)
    {
      line.color=(255<<24)+((16+x*7)<<8);
      auto pos=vec2i(mpos.x+16*x,0);
      line.add_with_color(quad,pos);
    }
    for(int y=0;y<16;y++)
    {
      auto pos=vec2i(0,16*y);
      od.add_with_offset(line,pos);
    }
    return od;
    */
  }
  t_out_dev get_grid(vec2i mpos,int zoom)
  {
    t_dev dev;
    dev.p=get_int(g_mem_addr());
    if(!dev.read_int())
    {
      int dev_p=dev.p;
      dev.write(gen_grid(mpos,zoom));
      int size=dev.p-dev_p;
      set_int(dev_p-1,size);
      dev.p=dev_p;
    }
    return dev.read_geom();
  }
  vector_vec2i get_arr(vec2i mpos,int zoom)
  {
    t_dev dev;
    dev.p=get_int(g_arr_addr());
    if(!dev.read_int())
    {
      int dev_p=dev.p;
      dev.write(get_points(mpos.x,mpos.y,zoom));
      int size=dev.p-dev_p;
      set_int(dev_p-1,size);
      dev.p=dev_p;
    }
    return dev.read_vector_vec2i();
  }
  struct t_app{
    int id=-1;
    vec2i obj_pos;
    vector_vec2i arr;
    t_out_dev grid;
  };
  //qap_save(app);
  struct t_str{
    int ptr;
    int n;
    /*
    int f(int c){setbyte(ptr*4+n,c);n++;return 0;}
    int hi_all(){
      f('H');
      f('i');
      f(' ');
      f('a');
      f('l');
      f('l');
      f('!');
      VAR("string",s);
      {
        mov(edi,s["ptr"]);add(edi,s["n"]);add(s["n"],7);
        mov(eax,'H');memsetbyte(edi);inc(edi);
        mov(eax,('a'<<24)+(' '<<16)+('i'<<8)+('H'<<0));stosd_inc();
        mov(eax,          ('!'<<16)+('l'<<8)+('l'<<0));stosd_inc();
        mov(s["n"],7);
      }
      setstr(s,"Hi all!");
    }*/
    int test_O2(){
      int r=1024;int n=0;int d=r*2;
      for(int i=0;i<d;i++)for(int j=0;j<d;j++){if(i*i+j*j>r*r)continue;n++;}
      return n;
    }
  }
  t_mem cr_test(t_input inp)
  {
    auto mbLeft=257;auto mbRight=258;auto mbMiddle=259;
    auto zoom=4;
    auto obj_pos=get_obj_pos(inp,zoom);
    //out.write_def(inp.mpos);
    vec2i mpos=mul(inp.mpos,zoom);
    auto od=new_geom(1024*16);
    od.color=(255<<24)+(128<<8);
    auto no_offset=vec2i(0,0);
    if(0)
    {
      auto q1=od.GenGeomQuad(mpos.x,mpos.y,30*zoom,30*zoom);
      od.color=(255<<24)+(128<<8)+(255<<16);
      auto q2=od.GenGeomQuad(mpos.x,mpos.y-64*zoom,100*zoom,10*zoom);
      auto q3=od.GenGeomQuad(200*zoom+mpos.x,mpos.y,10*zoom,60*zoom);
      od.add(q1);
      od.add(q2);
      od.add(q3);
      od.add(od.GenGeomLine(mpos,no_offset,zoom));
    }
    auto grid=get_grid(vec2i(0,0),zoom);
    auto arr=get_arr(vec2i(0,0),zoom);
    if(inp.kb_on_down('R')){
      arr.p=arr.addr_at(0);
      arr.n=0;
      t_dev dev;
      dev.p=get_int(g_arr_addr());
      dev.read_int();
      if(1)
      {
        int dev_p=dev.p;
        dev.write(arr);
        int size=dev.p-dev_p;
        set_int(dev_p-1,size);
        dev.p=dev_p;
      }
    }
    if(inp.kb_on_down(mbRight))
    {
      arr.add(mpos);
      t_dev dev;
      dev.p=get_int(g_arr_addr());
      dev.read_int();
      if(1)
      {
        int dev_p=dev.p;
        dev.write(arr);
        int size=dev.p-dev_p;
        set_int(dev_p-1,size);
        dev.p=dev_p;
      }
    }
    if(inp.kb_down.at(mbLeft))
    {
      auto id=0;
      for(int i=0;i<arr.n;i++){
        auto a=arr.at(id);
        auto b=arr.at(i);
        auto ad=hypot(sub(a,mpos));
        auto bd=hypot(sub(b,mpos));
        if(bd<ad)id=i;
      }
      set_vec2i(arr.addr_at(id),mpos);
    }
    od.add(GenGeomCatmullRom(arr,zoom));
    //auto no_offset=vec2i(0,0);
    int VK_SHIFT=0;__asm("mov(var_VK_SHIFT,VK_SHIFT);");
    int draw_fast=inp.kb_down.at(VK_SHIFT);
    {
      for(int i=0;i<arr.n;i++)od.add_with_offset(grid,arr.at(i));
    }
    //od.add_with_offset(grid,add(obj_pos,mpos));
    /*
    {
      int did=od.VA.addr_at(0);
      int n=od.VA.n;
      __asm("mov(ecx,var_n);");
      __asm("mov(edi,var_did);");
      __asm("mov(edx,var_zoom);");
      __asm("{LOOP_SCOPE();inc(edi);mov(eax,ptr[edi]);mul(eax,edx);stosd_inc();mov(eax,ptr[edi]);mul(eax,edx);stosd_inc();}");
    }*/
    t_mem mem;
    mem.ptr=alloc(1024*1024);
    t_dev out;
    out.p=mem.ptr;
    out.write(od);
    mem.size=out.p-mem.ptr;
    return mem;
  }
  int main(int ptr)
  {
    set_int(g_ptr_addr(),0);
    set_int(g_mem_addr(),alloc(1024*512));
    set_int(g_arr_addr(),alloc(1024*16));
    t_input inp;t_dev dev;dev.p=ptr;
    inp.mpos=dev.read_vec2i();
    inp.zDelta=dev.read_int();
    inp.kb_down=dev.read_vector_int();
    inp.kb_changed=dev.read_vector_int();
    t_mem out=cr_test(inp);
    setebx(out.size);
    __asm("mov(ecx,1);");
    return out.ptr;
  }
}