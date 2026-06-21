//#include "StdAfx.h"

struct t_meta_lexer;
#include <unordered_map>
#include <random>
#include <iostream>
#include <math.h>
#include <unordered_set>

#ifndef WIN32
#include <sys/mman.h>  // для mmap
#include <unistd.h>    // для sysconf
#include <cerrno>      // для errno
#include <cstring>     // для strerror
#endif

#include "lodepng.h"
#include "lodepng.cpp"
#undef make_unique
template <typename T>
constexpr T AlignUp(T size,size_t alignment){return (size+(alignment-1))&~(alignment-1);}
template <typename T>
constexpr T AlignUp16(T size){return AlignUp<T>(size,16);}
extern void UberInfoBox(const string&caption,const string&text);

typedef double real;
template<typename TYPE>inline TYPE Lerp(const TYPE&A,const TYPE&B,const real&v){return A+(B-A)*v;}
class QapColor{
public:
  typedef uint8_t byte;
  typedef uint8_t uchar;
public:
public:
  typedef QapColor SelfClass;
public:
  uchar b;
  uchar g;
  uchar r;
  uchar a;
public:
public:
  QapColor():b(255),g(255),r(255),a(255) {}
  QapColor(byte A,byte R,byte G,byte B):a(A),r(R),g(G),b(B) {}
  QapColor(byte R,byte G,byte B):a(255),r(R),g(G),b(B) {}
  QapColor(const QapColor& v):a(v.a),r(v.r),g(v.g),b(v.b) {}
  QapColor(const unsigned int&v){*this=(QapColor&)v;}
public:
  bool operator==(const QapColor&v)
  {
    return(a==v.a)&&(r==v.r)&&(g==v.g)&&(b==v.b);
  }
  QapColor&operator=(const QapColor&v)
  {
    a=v.a;
    r=v.r;
    g=v.g;
    b=v.b;
    return *this;
  }
  QapColor operator+()const
  {
    return *this;
  }
public:
  QapColor&operator*=(const QapColor&v)
  {
    {
      a=Clamp(int(a)*int(v.a)/int(255),int(0),int(255));
    }
    {
      r=Clamp(int(r)*int(v.r)/int(255),int(0),int(255));
    }
    {
      g=Clamp(int(g)*int(v.g)/int(255),int(0),int(255));
    }
    {
      b=Clamp(int(b)*int(v.b)/int(255),int(0),int(255));
    }
    return *this;
  }
  QapColor&operator+=(const QapColor&v)
  {
    {
      a=Clamp(int(a)+int(v.a),int(0),int(255));
    }
    {
      r=Clamp(int(r)+int(v.r),int(0),int(255));
    }
    {
      g=Clamp(int(g)+int(v.g),int(0),int(255));
    }
    {
      b=Clamp(int(b)+int(v.b),int(0),int(255));
    }
    return *this;
  }
  QapColor&operator-=(const QapColor&v)
  {
    {
      a=Clamp(int(a)-int(v.a),int(0),int(255));
    }
    {
      r=Clamp(int(r)-int(v.r),int(0),int(255));
    }
    {
      g=Clamp(int(g)-int(v.g),int(0),int(255));
    }
    {
      b=Clamp(int(b)-int(v.b),int(0),int(255));
    }
    return *this;
  }
  QapColor operator*(const QapColor&v)const
  {
    return QapColor(int(int(a)*int(v.a))/int(255),int(int(r)*int(v.r))/int(255),int(int(g)*int(v.g))/int(255),int(int(b)*int(v.b))/int(255));
  }
  QapColor operator+(const QapColor&v)const
  {
    return QapColor(Clamp(int(a)+int(v.a),int(0),int(255)),Clamp(int(r)+int(v.r),int(0),int(255)),Clamp(int(g)+int(v.g),int(0),int(255)),Clamp(int(b)+int(v.b),int(0),int(255)));
  }
  QapColor operator-(const QapColor&v)const
  {
    return QapColor(Clamp(int(a)-int(v.a),int(0),int(255)),Clamp(int(r)-int(v.r),int(0),int(255)),Clamp(int(g)-int(v.g),int(0),int(255)),Clamp(int(b)-int(v.b),int(0),int(255)));
  }
public:
  QapColor&operator*=(real f)
  {
    {
      b=byte(Clamp(real(b)*f,0.0,255.0));
    }
    ;
    {
      g=byte(Clamp(real(g)*f,0.0,255.0));
    }
    ;
    {
      r=byte(Clamp(real(r)*f,0.0,255.0));
    }
    ;
    return *this;
  }
  QapColor&operator/=(real r)
  {
    real f=1.0/r;
    {
      b=byte(Clamp(real(b)*f,0.0,255.0));
    }
    ;
    {
      g=byte(Clamp(real(g)*f,0.0,255.0));
    }
    ;
    {
      r=byte(Clamp(real(r)*f,0.0,255.0));
    }
    ;
    return *this;
  }
public:
  /*
  QapColor(const D3DCOLOR& v)
  {
    *((D3DCOLOR*)(void*)this)=v;
  }
  operator D3DCOLOR&()const
  {
    return *(DWORD*)this;
  };*/
  operator unsigned int&()const{
    return *(unsigned int*)this;
  }
public:
public:
  byte GetLuminance()const
  {
    return int(int(r)+int(g)+int(b))/int(3);
  }
  QapColor toGray()const
  {
    auto l=GetLuminance();
    return QapColor(a,l,l,l);
  }
  inline static QapColor Mix(const QapColor&A,const QapColor&B,const real&t)
  {
    real ct=Clamp(t,0.0,1.0);
    real tA=1.0-ct;
    real tB=ct;
    QapColor O;
    {
      O.b=byte(Clamp(real(A.b)*tA+real(B.b)*tB,0.0,255.0));
    }
    {
      O.g=byte(Clamp(real(A.g)*tA+real(B.g)*tB,0.0,255.0));
    }
    {
      O.r=byte(Clamp(real(A.r)*tA+real(B.r)*tB,0.0,255.0));
    }
    {
      O.a=byte(Clamp(real(A.a)*tA+real(B.a)*tB,0.0,255.0));
    }
    return O;
  }
  inline static QapColor HalfMix(const QapColor&A,const QapColor&B)
  {
    QapColor O;
    {
      O.b=(int(A.b)+int(B.b))>>1;
    }
    {
      O.g=(int(A.g)+int(B.g))>>1;
    }
    {
      O.r=(int(A.r)+int(B.r))>>1;
    }
    {
      O.a=(int(A.a)+int(B.a))>>1;
    }
    return O;
  }
  inline QapColor inv_rgb()const{return QapColor(a,0xff-r,0xff-g,0xff-b);}
  inline QapColor swap_rg()const{return QapColor(a,b,g,r);}
};

static void lodepng_encodeOneStep(const char* filename, std::vector<unsigned char>& image, unsigned width, unsigned height)
{
  unsigned error=lodepng::encode(filename, image, width, height);
  if(error)cerr<<("encoder error "+to_string(error)+": "+lodepng_error_text(error))<<endl;
}
void lodepng_save_to_png(int cx,int cy,const vector<QapColor>&arr,const char*fn)
{
  unsigned width=cx, height=cy;
  std::vector<unsigned char> image;
  image.resize(width * height * 4);
  for(unsigned y=0; y < height; y++)
  for(unsigned x=0; x < width; x++)
  {
    auto c=arr[x+cx*y];
    auto&out=*(QapColor*)(void*)&image[4 * width * y + 4 * x + 0];
    out=c.swap_rg();
  }
  lodepng_encodeOneStep(fn, image, width, height);
}

auto lex2str=[](auto&lex){
  string out;
  save_obj(lex,out);
  return out;
};

#include "t_calc_impl.hpp"

extern "C" {
  static double sin_(double x){return sin(x);}
  static double cos_(double x){return cos(x);}
  static double sqrt_(double x){return sqrt(x);}
  static double floor_(double x){return floor(x);}
  static double ceil_(double x){return ceil(x);}
  static double exp_(double x){return exp(x);}
  static double abs_(double x){return abs(x);}
  static double log_(double x){return log(x);}
  static double tan_(double x){return tan(x);}
  static double pow_(double x,double y){return pow(x,y);}
  static double dist_(double x,double y){return sqrt(x*x+y*y);}
  static double ifgtone_(double x,double y){return x>1?y:x;}
  static double atan2_(double y,double x){return atan2(y,x);}
  static double min_(double y,double x){
    //cout<<"passed"<<endl;cin.get();
    return min(y,x);
  }
  static double max_(double y,double x){
    return max(y,x);
  }
};

namespace t_calc_test{
enum
{
  XMM0,
  XMM1,
  XMM2,
  XMM3
};
struct t_sys_func{virtual double main(const vector<double>&args){return 0;}};
struct t_function{
  string name;
  vector<string> args;
  TAutoPtr<t_calc::i_stat>*body=nullptr;
  t_sys_func*pfunc=nullptr;
  size_t code_offset=0;
  bool compiled=false;
  vector<string> calls;

  unordered_map<string,int> local2slot;
  int frame_size=0;
  int frame_size_aligned=0;
  //vector<t_function*> children;
  t_function*parent=nullptr;

  unordered_map<string,unique_ptr<t_function>> funcs;

  //unordered_map<string,int> local2slot;
  int next_slot=0;
  int get_slot(const string&name){
    auto it=local2slot.find(name);
    if(it!=local2slot.end())return it->second;
    int slot=next_slot++;
    local2slot[name]=slot;
    return slot;
  }
};
#if(0)
struct t_codegen{
  vector<string> asm_text;
  vector<uint8_t> code;
  int rsp_bias = 0;
  void out(const string&s){asm_text.push_back(s);}

  void u8(uint8_t v){code.push_back(v);}

  void u32(uint32_t v)
  {
    auto*p=(uint8_t*)&v;
    code.insert(code.end(),p,p+4);
  }

  void u64(uint64_t v)
  {
    auto*p=(uint8_t*)&v;
    code.insert(code.end(),p,p+8);
  }

  static string sxmm(int id)
  {
    return "xmm"+IToS(id);
  }
  void emit_mov_rdi_r13() {  // для Linux: первый аргумент в RDI
    out("mov rdi,r13");
    u8(0x4C); u8(0x89); u8(0xEF);
  }
  void emit_mov_arg1_reg() {
    #ifdef __linux__
    emit_mov_rdi_r13();
    #else
    emit_mov_rcx_r13();
    #endif
  }
  void emit_call_user(t_function*target){
    // Сохраняем параметры в правильных регистрах
    #ifdef __linux__
    emit_mov_rdi_r13();  // Linux: 1-й аргумент в RDI
    #else
    emit_mov_rcx_r13();  // Windows: 1-й аргумент в RCX
    #endif
    
    out("mov rax,"+target->name);
    u8(0x48); u8(0xB8);
    auto pos=code.size();
    u64(0);
    patches.push_back({pos,target->name,target});
    
    // Выравнивание стека для Linux (должен быть 16-байтовым)
    #ifdef __linux__
    // Проверяем выравнивание стека
    int alignment = (rsp_bias + 8) & 15; // +8 для return address
    if (alignment != 0) {
      int pad = 16 - alignment;
      emit_sub_rsp(pad);
      emit_call_rax();
      emit_add_rsp(pad);
    } else {
      emit_call_rax();
    }
    #else
    emit_call_rax();
    #endif
  }
  void emit_call(const string&name){
    if(!fn_addr.count(name)){cerr<<"fatal error! function not found: "<<name<<endl;exit(-1);}
    auto addr=(uint64_t)fn_addr[name];
    out("mov rax,"+name);
    u8(0x48);
    u8(0xB8);
    u64(addr);
    
    #ifdef __linux__
    // Выравнивание стека для Linux
    int alignment = (rsp_bias + 8) & 15;
    if (alignment != 0) {
      int pad = 16 - alignment;
      emit_sub_rsp(pad);
      emit_call_rax();
      emit_add_rsp(pad);
    } else {
      emit_call_rax();
    }
    #else
    emit_call_rax();
    #endif
  }
  void emit_load_const(double v)
  {
    out("mov rax,"+to_string((uint64_t&)v)+"//"+to_string(v));
    uint64_t bits=*(uint64_t*)&v;
    u8(0x48);
    u8(0xB8);
    u64(bits);
    out("movq xmm0,rax");
    u8(0x66);
    u8(0x48);
    u8(0x0F);
    u8(0x6E);
    u8(0xC0);
  }
  void emit_load_var(int id)
  {
    int disp=id*8;
    out("movsd xmm0,[rcx+"+IToS(disp)+"]");
    u8(0xF2);
    u8(0x0F);
    u8(0x10);
    u8(0x81);
    u32(disp);
  }
  void emit_store_var(int id)
  {
    int disp=id*8;

    out("movsd [rcx+"+IToS(disp)+"],xmm0");

    u8(0xF2);
    u8(0x0F);
    u8(0x11);
    u8(0x81);

    u32(disp);
  }
  void emit_movapd_xmm2_xmm0()
  {
    out("movapd xmm2,xmm0");

    u8(0x66);
    u8(0x0F);
    u8(0x28);
    u8(0xD0);
  }
  void emit_movapd_xmm3_xmm0()
  {
    out("movapd xmm3,xmm0");

    u8(0x66);
    u8(0x0F);
    u8(0x28);
    u8(0xD8);
  }
  void emit_set_call_arg(int arg,int reg)
  {
    QapAssert(reg==XMM0);

    switch(arg)
    {
      case 0:
        return;

      case 1:
      {
        out("movapd xmm1,xmm0");

        u8(0x66);
        u8(0x0F);
        u8(0x28);
        u8(0xC8);
        return;
      }

      case 2:
      {
        out("movapd xmm2,xmm0");

        u8(0x66);
        u8(0x0F);
        u8(0x28);
        u8(0xD0);
        return;
      }

      case 3:
      {
        out("movapd xmm3,xmm0");

        u8(0x66);
        u8(0x0F);
        u8(0x28);
        u8(0xD8);
        return;
      }
    }

    QapAssert(false);
  }
  void emit_movapd_xmm0_xmm1()
  {
    out("movapd xmm0,xmm1");

    u8(0x66);
    u8(0x0F);
    u8(0x28);
    u8(0xC1);
  }
  void emit_addsd(int dst=XMM0,int src=XMM1)
  {
    QapAssert(dst==XMM0);
    QapAssert(src==XMM1);

    out("addsd xmm0,xmm1");

    u8(0xF2);
    u8(0x0F);
    u8(0x58);
    u8(0xC1);
  }

  void emit_subsd(int dst=XMM1,int src=XMM0)
  {
    QapAssert(dst==XMM1);
    QapAssert(src==XMM0);

    out("subsd xmm1,xmm0");

    u8(0xF2);
    u8(0x0F);
    u8(0x5C);
    u8(0xC8);
    emit_movapd_xmm0_xmm1();
  }

  void emit_mulsd(int dst=XMM0,int src=XMM1)
  {
    QapAssert(dst==XMM0);
    QapAssert(src==XMM1);

    out("mulsd xmm0,xmm1");

    u8(0xF2);
    u8(0x0F);
    u8(0x59);
    u8(0xC1);
  }

  void emit_divsd(int dst=XMM1,int src=XMM0)
  {
    QapAssert(dst==XMM1);
    QapAssert(src==XMM0);

    out("divsd xmm1,xmm0");

    u8(0xF2);
    u8(0x0F);
    u8(0x5E);
    u8(0xC8);
    emit_movapd_xmm0_xmm1();
  }
  void emit_movapd(int dst,int src)
  {
    QapAssert(dst==XMM0);
    QapAssert(src==XMM1);

    out("movapd xmm0,xmm1");

    u8(0x66);
    u8(0x0F);
    u8(0x28);
    u8(0xC1);
  }
  void emit_ret()
  {
    out("ret");
    u8(0xC3);
  }
  void emit_prolog()
  {
    out("lea r8,[rcx+2048]");

    u8(0x4C);
    u8(0x8D);
    u8(0x81);

    u32(2048);
  }
  void emit_root_func_begin(int frame_size){
    emit_prolog();
    emit_push_rbp();
    emit_mov_rbp_rsp();
    emit_push_r12();
    emit_push_r13();
    if(frame_size)emit_sub_rsp(frame_size);
    out("lea r12,[rcx+8192]");
    u8(0x4C);
    u8(0x8D);
    u8(0xA1);
    u32(8192);
    out("mov r13,r12");
    u8(0x4D);
    u8(0x89);
    u8(0xE5);
  }
  void emit_root_func_end(int frame_size){
    if(frame_size)emit_add_rsp(frame_size);
    emit_pop_r13();
    emit_pop_r12();
    emit_pop_rbp();
    emit_ret();
  }
  void emit_stack_push(int reg)
  {
    //QapAssert(reg==XMM0);

    out("movsd [r8],xmm"+IToS(reg-XMM0));

    u8(0xF2);
    u8(0x41);
    u8(0x0F);
    u8(0x11);
    u8(8*(reg-XMM0));

    out("add r8,8");

    u8(0x49);
    u8(0x83);
    u8(0xC0);
    u8(8);
  }
  void emit_stack_pop(int reg)
  {
    //QapAssert(reg==XMM1);

    out("sub r8,8");

    u8(0x49);
    u8(0x83);
    u8(0xE8);
    u8(8);

    out("movsd xmm"+IToS(reg-XMM0)+",[r8]");
    
    u8(0xF2);
    u8(0x41);
    u8(0x0F);
    u8(0x10);
    u8(8*(reg-XMM0));
  }
  struct t_call_patch{size_t offset;string name;t_function*target=nullptr;};
  vector<t_call_patch> patches;
  void emit_func_begin(int frame_size)
  {
    emit_push_rbp();
    emit_mov_rbp_rsp();
    if(frame_size)emit_sub_rsp(frame_size);
  }
  void emit_func_end(int frame_size)
  {
    if(frame_size)emit_add_rsp(frame_size);
    emit_pop_rbp();
    emit_ret();
  }
  void emit_mov_rbp_rsp(){
    out("mov rbp,rsp");
    u8(0x48);
    u8(0x89);
    u8(0xE5);
  }
  void emit_sub_rsp(int n){
    QapAssert(n>=0);
    out("sub rsp,"+IToS(n));
    if(n<=127){
      u8(0x48);
      u8(0x83);
      u8(0xEC);
      u8(n);
    }else{
      u8(0x48);
      u8(0x81);
      u8(0xEC);
      u32(n);
    }
    rsp_bias+=n;
  }
  void emit_add_rsp(int n){
    QapAssert(n>=0);
    out("add rsp,"+IToS(n));
    if(n<=127){
      u8(0x48);
      u8(0x83);
      u8(0xC4);
      u8(n);
    }else{
      u8(0x48);
      u8(0x81);
      u8(0xC4);
      u32(n);
    }
    rsp_bias-=n;
  }
  int calc_call_bytes()const{
    int shadow=32;
    int mis=(rsp_bias+8)&15;
    int pad=(16-mis)&15;
    return shadow+pad;
  }
  void emit_call_rax(){
    int bytes=calc_call_bytes();
    emit_sub_rsp(bytes);
    out("call rax");
    u8(0xFF);u8(0xD0);
    emit_add_rsp(bytes);
  }/*
  void emit_call(const string&name){
    if(!fn_addr.count(name)){cerr<<"fatal error! function not found: "<<name<<endl;exit(-1);}
    auto addr=(uint64_t)fn_addr[name];
    out("mov rax,"+name);
    u8(0x48);
    u8(0xB8);
    u64(addr);
    emit_call_rax();
  }
  void emit_call_user(t_function*target){
    emit_mov_rcx_rdx();
    out("mov rax,"+target->name);
    u8(0x48);u8(0xB8);
    auto pos=code.size();
    u64(0);
    patches.push_back({pos,target->name,target});
    emit_call_rax();
  }*/
  void emit_alloc_call_frame(int bytes)
  {
    out("mov rdx,r12");

    u8(0x4C);
    u8(0x89);
    u8(0xE2);

    out("add r12,"+IToS(bytes));

    u8(0x49);
    u8(0x81);
    u8(0xC4);
    u32(bytes);
  }
  void emit_free_call_frame(int bytes)
  {
    out("sub r12,"+IToS(bytes));

    u8(0x49);
    u8(0x81);
    u8(0xEC);
    u32(bytes);
  }
  void emit_mov_rcx_rdx()
  {
    out("mov rcx,rdx");

    u8(0x48);
    u8(0x89);
    u8(0xD1);
  }
  void emit_store_call_arg(int slot)
  {
    int disp=slot*8;

    out("movsd [rdx+"+IToS(disp)+"],xmm0");// так мы не сможем прокидывать аргументы внешней функции в аргументы вызываемой функции.

    u8(0xF2);
    u8(0x0F);
    u8(0x11);
    u8(0x82);
    u32(disp);
  }
  void emit_mov_rcx_r13(){out("mov rcx,r13");u8(0x4C);u8(0x89);u8(0xE9);}

  void emit_push_rbp(){out("push rbp");u8(0x55);rsp_bias+=8;}
  void emit_pop_rbp(){out("pop rbp");u8(0x5D);rsp_bias-=8;}
     
  void emit_push_r12(){out("push r12");u8(0x41);u8(0x54);rsp_bias+=8;}
  void emit_pop_r12(){out("pop r12");u8(0x41);u8(0x5C);rsp_bias-=8;}
     
  void emit_push_r13(){out("push r13");u8(0x41);u8(0x55);rsp_bias+=8;}
  void emit_pop_r13(){out("pop r13");u8(0x41);u8(0x5D);rsp_bias-=8;}
     
  void emit_push_rcx(){out("push rcx");u8(0x51);rsp_bias+=8;}
  void emit_pop_rcx(){out("pop rcx");u8(0x59);rsp_bias-=8;}
     
  void push_r8(){out("push r8");u8(0x41);u8(0x50);rsp_bias+=8;}
  void pop_r8(){out("pop r8");u8(0x41);u8(0x58);rsp_bias-=8;}

  unordered_map<string,void*> fn_addr;
  void init_fn_addr(){
    fn_addr["sin"]=(void*)sin_;
    fn_addr["cos"]=(void*)cos_;
    fn_addr["sqrt"]=(void*)sqrt_;
    fn_addr["atan2"]=(void*)atan2_;
    fn_addr["min"]=(void*)min_;
    fn_addr["max"]=(void*)max_;
    fn_addr["floor"]=(void*)floor_;
    fn_addr["ceil"]=(void*)ceil_;
    fn_addr["exp"]=(void*)exp_;
    fn_addr["pow"]=(void*)pow_;
    fn_addr["dist"]=(void*)dist_;
    fn_addr["abs"]=(void*)abs_;
    fn_addr["log"]=(void*)log_;
    fn_addr["tan"]=(void*)tan_;
    fn_addr["ifgtone"]=(void*)ifgtone_;
    #define F(CODE)cout<<#CODE"="<<(CODE)<<endl;
    //F(sin_(10))F(cos_(10))F(sqrt_(14))F(atan2_(10,20))F(min_(10,20))F(max_(10,20))
    #undef F
  }
};
#endif
// В начале файла после includes:
#ifdef __linux__
  #define QQ(LINUX_CODE, WIN_CODE) {LINUX_CODE;}
#else
  #define QQ(LINUX_CODE, WIN_CODE) {WIN_CODE;}
#endif

struct t_codegen{
  vector<string> asm_text;
  vector<uint8_t> code;
  int rsp_bias = 0;

  void out(const string&s){asm_text.push_back(s);}
  void u8(uint8_t v){code.push_back(v);}
  void u32(uint32_t v){auto*p=(uint8_t*)&v; code.insert(code.end(),p,p+4);}
  void u64(uint64_t v){auto*p=(uint8_t*)&v; code.insert(code.end(),p,p+8);}

  static string sxmm(int id){return "xmm"+IToS(id);}
  void emit_mov_rdi_r13() {  // для Linux: первый аргумент в RDI
    out("mov rdi,r13");
    u8(0x4C); u8(0x89); u8(0xEF);
  }
  void emit_load_const(double v){
    out("mov rax,"+to_string((uint64_t&)v)+"//"+to_string(v));
    uint64_t bits=*(uint64_t*)&v;
    u8(0x48); u8(0xB8); u64(bits);
    out("movq xmm0,rax");
    u8(0x66); u8(0x48); u8(0x0F); u8(0x6E); u8(0xC0);
  }

  void emit_load_var(int id){
    int disp=id*8;
    out("movsd xmm0,[rcx+"+IToS(disp)+"]");
    u8(0xF2); u8(0x0F); u8(0x10); u8(0x81); u32(disp);
  }

  void emit_store_var(int id){
    int disp=id*8;
    out("movsd [rcx+"+IToS(disp)+"],xmm0");
    u8(0xF2); u8(0x0F); u8(0x11); u8(0x81); u32(disp);
  }

  void emit_movapd_xmm2_xmm0(){
    out("movapd xmm2,xmm0");
    u8(0x66); u8(0x0F); u8(0x28); u8(0xD0);
  }

  void emit_movapd_xmm3_xmm0(){
    out("movapd xmm3,xmm0");
    u8(0x66); u8(0x0F); u8(0x28); u8(0xD8);
  }

  void emit_movapd_xmm0_xmm1(){
    out("movapd xmm0,xmm1");
    u8(0x66); u8(0x0F); u8(0x28); u8(0xC1);
  }

  void emit_movapd_xmm0_xmm2(){
    out("movapd xmm0,xmm2");
    u8(0x66); u8(0x0F); u8(0x28); u8(0xC2);
  }

  void emit_movapd_xmm0_xmm3(){
    out("movapd xmm0,xmm3");
    u8(0x66); u8(0x0F); u8(0x28); u8(0xC3);
  }

  void emit_addsd(int dst=XMM0,int src=XMM1){
    QapAssert(dst==XMM0); QapAssert(src==XMM1);
    out("addsd xmm0,xmm1");
    u8(0xF2); u8(0x0F); u8(0x58); u8(0xC1);
  }

  void emit_subsd(int dst=XMM1,int src=XMM0){
    QapAssert(dst==XMM1); QapAssert(src==XMM0);
    out("subsd xmm1,xmm0");
    u8(0xF2); u8(0x0F); u8(0x5C); u8(0xC8);
    emit_movapd_xmm0_xmm1();
  }

  void emit_mulsd(int dst=XMM0,int src=XMM1){
    QapAssert(dst==XMM0); QapAssert(src==XMM1);
    out("mulsd xmm0,xmm1");
    u8(0xF2); u8(0x0F); u8(0x59); u8(0xC1);
  }

  void emit_divsd(int dst=XMM1,int src=XMM0){
    QapAssert(dst==XMM1); QapAssert(src==XMM0);
    out("divsd xmm1,xmm0");
    u8(0xF2); u8(0x0F); u8(0x5E); u8(0xC8);
    emit_movapd_xmm0_xmm1();
  }

  void emit_movapd(int dst,int src){
    QapAssert(dst==XMM0); QapAssert(src==XMM1);
    out("movapd xmm0,xmm1");
    u8(0x66); u8(0x0F); u8(0x28); u8(0xC1);
  }

  void emit_ret(){
    out("ret");
    u8(0xC3);
  }

  void emit_prolog(){
    QQ(
      // Linux: используем r15
      out("lea r15,[rcx+2048]");
      u8(0x4C); u8(0x8D); u8(0xB9); u32(2048),
      
      // Windows: используем r8
      out("lea r8,[rcx+2048]");
      u8(0x4C); u8(0x8D); u8(0x81); u32(2048)
    )
  }

  void emit_stack_push(int reg){
    QQ(
      // Linux: используем r15
      out("movsd [r15],xmm"+IToS(reg-XMM0));
      u8(0xF2); u8(0x41); u8(0x0F); u8(0x11); u8(0x07);
      out("add r15,8");
      u8(0x49); u8(0x83); u8(0xC7); u8(8),
      
      // Windows: используем r8
      out("movsd [r8],xmm"+IToS(reg-XMM0));
      u8(0xF2); u8(0x41); u8(0x0F); u8(0x11); u8(8*(reg-XMM0));
      out("add r8,8");
      u8(0x49); u8(0x83); u8(0xC0); u8(8)
    )
  }

  void emit_stack_pop(int reg){
    QQ(
      // Linux: используем r15
      out("sub r15,8");
      u8(0x49); u8(0x83); u8(0xEF); u8(8);
      out("movsd xmm"+IToS(reg-XMM0)+",[r15]");
      u8(0xF2); u8(0x41); u8(0x0F); u8(0x10); u8(0x07),
      
      // Windows: используем r8
      out("sub r8,8");
      u8(0x49); u8(0x83); u8(0xE8); u8(8);
      out("movsd xmm"+IToS(reg-XMM0)+",[r8]");
      u8(0xF2); u8(0x41); u8(0x0F); u8(0x10); u8(8*(reg-XMM0))
    )
  }

  void emit_push_stack_reg(){
    QQ(
      out("push r15"); u8(0x41); u8(0x57); rsp_bias += 8,  // Linux
      out("push r8");  u8(0x41); u8(0x50); rsp_bias += 8   // Windows
    )
  }

  void emit_pop_stack_reg(){
    QQ(
      out("pop r15"); u8(0x41); u8(0x5F); rsp_bias -= 8,   // Linux
      out("pop r8");  u8(0x41); u8(0x58); rsp_bias -= 8    // Windows
    )
  }

  void emit_root_func_begin(int frame_size){
    emit_push_stack_reg();
    ifdef __linux__
    out("mov rcx,rdi");
    u8(0x48); u8(0x89); u8(0xF9);
    #endif
    emit_prolog();
    emit_push_rbp();
    emit_mov_rbp_rsp();
    emit_push_r12();
    emit_push_r13();
    
    QQ(
      // Linux: выравнивание стека
      {
        int total_push = 8 + 8 + 8 + 8;
        int adjusted_frame = (frame_size + 15) & ~15;
        if ((adjusted_frame + total_push) % 16 != 0) adjusted_frame += 8;
        if(adjusted_frame) emit_sub_rsp(adjusted_frame);
      },
      
      // Windows: без выравнивания
      {
        if(frame_size) emit_sub_rsp(frame_size);
      }
    )
    
    out("lea r12,[rcx+8192]");
    u8(0x4C); u8(0x8D); u8(0xA1); u32(8192);
    out("mov r13,r12");
    u8(0x4D); u8(0x89); u8(0xE5);
  }

  void emit_root_func_end(int frame_size){
    QQ(
      // Linux: восстановление стека
      {
        int total_push = 8 + 8 + 8 + 8;
        int adjusted_frame = (frame_size + 15) & ~15;
        if ((adjusted_frame + total_push) % 16 != 0) adjusted_frame += 8;
        if(adjusted_frame) emit_add_rsp(adjusted_frame);
      },
      
      // Windows
      {
        if(frame_size) emit_add_rsp(frame_size);
      }
    )
    
    emit_pop_r13();
    emit_pop_r12();
    emit_pop_rbp();
    emit_pop_stack_reg();
    emit_ret();
  }

  void emit_func_begin(int frame_size){
    emit_push_stack_reg();
    emit_prolog();
    emit_push_rbp();
    emit_mov_rbp_rsp();
    emit_push_r12();
    emit_push_r13();
    
    QQ(
      // Linux: выравнивание стека
      {
        int total_push = 8 + 8 + 8 + 8;
        int adjusted_frame = (frame_size + 15) & ~15;
        if ((adjusted_frame + total_push) % 16 != 0) adjusted_frame += 8;
        if(adjusted_frame) emit_sub_rsp(adjusted_frame);
      },
      
      // Windows
      {
        if(frame_size) emit_sub_rsp(frame_size);
      }
    )
    
    out("lea r12,[rcx+8192]");
    u8(0x4C); u8(0x8D); u8(0xA1); u32(8192);
    out("mov r13,r12");
    u8(0x4D); u8(0x89); u8(0xE5);
  }

  void emit_func_end(int frame_size){
    QQ(
      // Linux: восстановление стека
      {
        int total_push = 8 + 8 + 8 + 8;
        int adjusted_frame = (frame_size + 15) & ~15;
        if ((adjusted_frame + total_push) % 16 != 0) adjusted_frame += 8;
        if(adjusted_frame) emit_add_rsp(adjusted_frame);
      },
      
      // Windows
      {
        if(frame_size) emit_add_rsp(frame_size);
      }
    )
    
    emit_pop_r13();
    emit_pop_r12();
    emit_pop_rbp();
    emit_pop_stack_reg();
    emit_ret();
  }

  void emit_mov_rbp_rsp(){
    out("mov rbp,rsp");
    u8(0x48); u8(0x89); u8(0xE5);
  }

  void emit_sub_rsp(int n){
    QapAssert(n>=0);
    out("sub rsp,"+IToS(n));
    if(n<=127){ u8(0x48); u8(0x83); u8(0xEC); u8(n); }
    else { u8(0x48); u8(0x81); u8(0xEC); u32(n); }
    rsp_bias+=n;
  }

  void emit_add_rsp(int n){
    QapAssert(n>=0);
    out("add rsp,"+IToS(n));
    if(n<=127){ u8(0x48); u8(0x83); u8(0xC4); u8(n); }
    else { u8(0x48); u8(0x81); u8(0xC4); u32(n); }
    rsp_bias-=n;
  }

  int calc_call_bytes()const{
    QQ(
      // Linux: только выравнивание
      {
        int mis = (rsp_bias + 8) & 15;
        int pad = (16 - mis) & 15;
        return pad;
      },
      
      // Windows: shadow space + выравнивание
      {
        int shadow = 32;
        int mis = (rsp_bias + 8) & 15;
        int pad = (16 - mis) & 15;
        return shadow + pad;
      }
    )
  }

  void emit_call_rax(){
    int bytes = calc_call_bytes();
    if(bytes) emit_sub_rsp(bytes);
    out("call rax");
    u8(0xFF); u8(0xD0);
    if(bytes) emit_add_rsp(bytes);
  }

  void emit_mov_arg1_r13(){
    QQ(
      // Linux: первый аргумент в RDI
      out("mov rdi,r13");
      u8(0x4C); u8(0x89); u8(0xEF),
      
      // Windows: первый аргумент в RCX
      out("mov rcx,r13");
      u8(0x4C); u8(0x89); u8(0xE9)
    )
  }

  void emit_call_user(t_function*target){
    emit_mov_arg1_r13();
    out("mov rax,"+target->name);
    u8(0x48); u8(0xB8);
    auto pos=code.size();
    u64(0);
    patches.push_back({pos,target->name,target});
    emit_call_rax();
  }

  void emit_call(const string&name){
    if(!fn_addr.count(name)){cerr<<"fatal error! function not found: "<<name<<endl;exit(-1);}
    auto addr=(uint64_t)fn_addr[name];
    out("mov rax,"+name);
    u8(0x48); u8(0xB8); u64(addr);
    emit_call_rax();
  }

  void emit_alloc_call_frame(int bytes){
    int aligned_bytes = (bytes + 15) & ~15;
    out("mov rdx,r12");
    u8(0x4C); u8(0x89); u8(0xE2);
    out("add r12,"+IToS(aligned_bytes));
    u8(0x49); u8(0x81); u8(0xC4); u32(aligned_bytes);
  }

  void emit_free_call_frame(int bytes){
    int aligned_bytes = (bytes + 15) & ~15;
    out("sub r12,"+IToS(aligned_bytes));
    u8(0x49); u8(0x81); u8(0xEC); u32(aligned_bytes);
  }

  void emit_store_call_arg(int slot){
    int disp=slot*8;
    out("movsd [rdx+"+IToS(disp)+"],xmm0");
    u8(0xF2); u8(0x0F); u8(0x11); u8(0x82); u32(disp);
  }

  void emit_mov_rcx_r13(){
    out("mov rcx,r13");
    u8(0x4C); u8(0x89); u8(0xE9);
  }

  void emit_push_rbp(){out("push rbp"); u8(0x55); rsp_bias+=8;}
  void emit_pop_rbp(){out("pop rbp"); u8(0x5D); rsp_bias-=8;}
     
  void emit_push_r12(){out("push r12"); u8(0x41); u8(0x54); rsp_bias+=8;}
  void emit_pop_r12(){out("pop r12"); u8(0x41); u8(0x5C); rsp_bias-=8;}
     
  void emit_push_r13(){out("push r13"); u8(0x41); u8(0x55); rsp_bias+=8;}
  void emit_pop_r13(){out("pop r13"); u8(0x41); u8(0x5D); rsp_bias-=8;}
     
  void emit_push_rcx(){out("push rcx"); u8(0x51); rsp_bias+=8;}
  void emit_pop_rcx(){out("pop rcx"); u8(0x59); rsp_bias-=8;}
     
  void push_r8(){out("push r8"); u8(0x41); u8(0x50); rsp_bias+=8;}
  void pop_r8(){out("pop r8"); u8(0x41); u8(0x58); rsp_bias-=8;}

  struct t_call_patch{size_t offset;string name;t_function*target=nullptr;};
  vector<t_call_patch> patches;

  unordered_map<string,void*> fn_addr;

  void init_fn_addr(){
    fn_addr["sin"]=(void*)sin_;
    fn_addr["cos"]=(void*)cos_;
    fn_addr["sqrt"]=(void*)sqrt_;
    fn_addr["atan2"]=(void*)atan2_;
    fn_addr["min"]=(void*)min_;
    fn_addr["max"]=(void*)max_;
    fn_addr["floor"]=(void*)floor_;
    fn_addr["ceil"]=(void*)ceil_;
    fn_addr["exp"]=(void*)exp_;
    fn_addr["pow"]=(void*)pow_;
    fn_addr["dist"]=(void*)dist_;
    fn_addr["abs"]=(void*)abs_;
    fn_addr["log"]=(void*)log_;
    fn_addr["tan"]=(void*)tan_;
    fn_addr["ifgtone"]=(void*)ifgtone_;
    #define F(CODE)cout<<#CODE"="<<(CODE)<<endl;
    F(sin_(10))F(cos_(10))F(sqrt_(14))F(atan2_(10,20))F(min_(10,20))F(max_(10,20))
    #undef F
  }
};

struct t_ast2x64:t_calc::i_term::i_visitor,t_calc::i_stat::i_visitor{
  vector<t_function*> all_funcs;
  t_function*cur_func=nullptr;
  vector<t_scope> scopes;
  t_function*find_func(t_function*start,const string&name){
    auto*cur=start;
    while(cur){
      auto it=cur->funcs.find(name);
      if(it!=cur->funcs.end())return it->second.get();
      cur=cur->parent;
    }
    return nullptr;
  }
  static double to_double(const string&s){return atof(s.c_str());}
  template<class TYPE>void Do(TYPE&r){r.Use(*this);}
  template<class TYPE>void Do(vector<TYPE>&arr){for(auto&ex:arr)Do(&ex);}
  template<class TYPE>void Do(vector<TAutoPtr<TYPE>>&arr){for(auto&ex:arr)if(ex)Do(*ex.get());}
  template<class TYPE>void Do(TAutoPtr<TYPE>&r){if(r)Do(*r.get());}
  void Do(t_calc::t_term& r){
    Do(r.value);
  }
  void Do(t_calc::t_number& r){
    jit.emit_load_const(to_double(r.value));
  }
  void Do(t_calc::t_scope& r){
    Do(r.value);
  }
  void Do(t_calc::t_divmul::t_elem& r){
    jit.emit_stack_push(XMM0);
    Do(r.expr);
    jit.emit_stack_pop(XMM1);
    if(r.oper=="*"){
      jit.emit_mulsd();
    }else{
      jit.emit_divsd();
    }
  }
  void Do(t_calc::t_divmul& r){
    Do(r.first);
    for(auto& e : r.arr){
      Do(e);
    }
  }
  void Do(t_calc::t_addsub::t_elem& r){
    jit.emit_stack_push(XMM0);
    Do(r.expr);
    jit.emit_stack_pop(XMM1);
    if(r.oper=="+"){
      jit.emit_addsd();
    }else{
      jit.emit_subsd();
    }
  }
  void Do(t_calc::t_addsub& r){
    if(PASS_CODEGEN!=pass)return;
    Do(r.first);
    for(auto& e : r.arr){
      Do(e);
    }
  }
  enum e_pass{
    PASS_REGISTER_FUNCS,
    PASS_COLLECT_LOCALS,
    PASS_CODEGEN
  };
  e_pass pass;
  t_codegen jit;
  void Do(t_calc::t_varcall& r){
    const string&name=r.name;
    if(!r.params){
      if(PASS_COLLECT_LOCALS==pass){
        cur_func->get_slot(r.name);
        return;
      }
      if(PASS_CODEGEN!=pass)return;
      auto id=cur_func->get_slot(r.name);
      jit.out("//var "+r.name);
      jit.emit_load_var(id);
      return;
    }
    if(PASS_CODEGEN!=pass)return;
    auto*target=find_func(cur_func,name);
    for(auto&arg:r.params->arr){
      Do(arg.body);
      jit.emit_stack_push(XMM0);
    }
    if(target)
    {
      jit.emit_push_r13();
      jit.emit_alloc_call_frame(target->frame_size_aligned);
      for(int i=int(r.params->arr.size())-1;i>=0;i--)
      {
        jit.emit_stack_pop(XMM0);
        auto slot=target->get_slot(target->args[i]);
        jit.emit_store_call_arg(slot);
      }
      jit.emit_push_rcx();
      #ifdef __linux__
      jit.emit_mov_rdi_r13();  // Linux: указатель в RDI
      #else
      jit.emit_mov_rcx_r13();  // Windows: указатель в RCX
      #endif
      jit.emit_call_user(target);
      jit.emit_pop_rcx();
      jit.emit_free_call_frame(target->frame_size_aligned);
      jit.emit_pop_r13();
    }else{
      jit.emit_push_r13();
      jit.emit_push_r12();
      jit.emit_push_rcx();
      for(int i=int(r.params->arr.size())-1;i>=0;i--){
        jit.emit_stack_pop(XMM0+i);
      }
      jit.push_r8();
      jit.emit_call(name);
      jit.pop_r8();
      jit.emit_pop_rcx();
      jit.emit_pop_r12();
      jit.emit_pop_r13();
    }
  }
  void Do(t_calc::t_func_stat&r){
    if(PASS_COLLECT_LOCALS==pass){
      for(auto&a:r.args){
        cur_func->get_slot(a.name);
      }
      return;
    }
    if(PASS_REGISTER_FUNCS==pass){
      auto fn=std::make_unique<t_function>();
      fn->name=r.func;
      for(auto&a:r.args){
        fn->args.push_back(a.name);
      }
      fn->parent=cur_func;
      fn->body=&r.body;
      auto*ptr=fn.get();
      cur_func->funcs[r.func]=std::move(fn);
      all_funcs.push_back(ptr);
      cur_func=ptr;
      for(auto&a:r.args){
        cur_func->get_slot(a.name);
      }
      Do(*r.body);
      cur_func=cur_func->parent;
      return;
    }
  }
  void Do(t_calc::t_assign_stat&r){
    if(PASS_COLLECT_LOCALS==pass){
      cur_func->get_slot(r.var);
      Do(r.expr);
      return;
    }
    Do(r.expr);
    auto id=cur_func->get_slot(r.var);
    if(PASS_CODEGEN!=pass)return;
    jit.emit_store_var(id);
  }
  void Do(t_calc&r){
    for(auto&st:r.arr){
      Do(*st.get());
    }
  }
  void Do(t_calc::t_block_stat&r){
    scopes.push_back({});
    for(auto&st:r.arr){
      Do(*st.get());
    }
    scopes.pop_back();
  }
  void Do(t_calc::t_solo_stat&r){
    Do(r.expr);
  }
  t_function root;
  void compile(t_function&fn){
    if(fn.compiled)return;
    fn.compiled=true;
    cur_func=&fn;
    pass=PASS_COLLECT_LOCALS;
    Do(*fn.body);
    pass=PASS_CODEGEN;
    fn.frame_size=fn.next_slot*8;
    fn.frame_size_aligned=AlignUp16(fn.frame_size);
    fn.code_offset=jit.code.size();
    bool like_rgb=cur_func->parent==&root&&string("rgb").find(fn.name)!=string::npos;
    if(like_rgb){
      jit.emit_root_func_begin(fn.frame_size_aligned);
    }else{
      jit.emit_func_begin(fn.frame_size_aligned);
    }
    Do(*fn.body);
    if(like_rgb){
      jit.emit_root_func_end(fn.frame_size_aligned);
    }else{
      jit.emit_func_end(fn.frame_size_aligned);
    }
  }
  double parse_ms=0;
  string main(const string&data,const string&out_fn={}){
    TAutoPtr<t_calc> c;QapClock clock;string errmsg;
    scopes.reserve(4096);
    scopes.push_back({});
    //init_builtin_vars();
    root.name="<root>";
    cur_func=&root;
    jit.init_fn_addr();
    //jit.emit_prolog();
    auto&ctx=scopes.back();
    auto r=load_obj_full(c,data,true,&errmsg);
    parse_ms=clock.MS();
    std::cerr<<"{\"parse_ms\":"<<parse_ms<<"}"<<endl;
    if(!r.ok){QapDebugMsg(r.msg);return {};}
    auto&tar=*c;
    pass=PASS_REGISTER_FUNCS;
    Do(tar);
    pass=PASS_CODEGEN;
    //Do(tar);
    {
      for(auto*f:all_funcs){
        compile(*f);
      }
    }
    void*base=nullptr;
    {
      #ifndef WIN32
      // Вычисляем размер страницы для выравнивания
      long page_size = sysconf(_SC_PAGESIZE);
      size_t code_size = jit.code.size();
    
      // Выравниваем размер до границы страницы
      size_t aligned_size = (code_size + page_size - 1) & ~(page_size - 1);
    
      // Выделяем память с правами на запись и чтение
      base = mmap(nullptr, aligned_size, PROT_READ | PROT_WRITE,
                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    
      if (base == MAP_FAILED) {
        std::cerr << "mmap failed: " << strerror(errno) << std::endl;
        return {};
      }
    
      // Копируем код
      memcpy(base, jit.code.data(), code_size);
    
      // Меняем права на исполнение и чтение (защита от записи)
      if (mprotect(base, aligned_size, PROT_READ | PROT_EXEC) == -1) {
        std::cerr << "mprotect failed: " << strerror(errno) << std::endl;
        munmap(base, aligned_size);
        return {};
      }
      #else
      base=VirtualAlloc(
          nullptr,
          jit.code.size(),
          MEM_COMMIT|MEM_RESERVE,
          PAGE_EXECUTE_READWRITE
        );
      memcpy(
        base,
        jit.code.data(),
        jit.code.size()
      );
      #endif
      auto*pb=(uint8_t*)base;
      for(auto&ex:jit.patches){
        auto addr=pb+ex.target->code_offset;
        *(uint64_t*)(&pb[ex.offset])=(uint64_t)addr;
      }
      stringstream ss;//string str;str.resize(jit.code.size());
      for(int i=0;i<jit.code.size();i++){
        ss<<std::hex<<std::setw(2)<<std::setfill('0')<<(int)pb[i];
      }
      cout<<"mashkod:\n"<<ss.str()<<endl;
      cout<<base<<endl;
      //cin.get();
      cout<<base<<endl;
      cout<<join(jit.asm_text,"\n")<<endl;
    }
    auto mstar=clock.MS();std::cerr<<"{\"Do(tar) done in \":"<<(mstar-parse_ms)<<"}"<<endl;
    int cx=1920;int cy=1200;vector<QapColor> arr(cx*cy);
    auto msarr=clock.MS();std::cerr<<"{\"arr.resize done in \":"<<(msarr-mstar)<<"}"<<endl;
    struct t_runtime{
      array<double,1024> slots;
      array<double,1024*1024> frames;
    };
    unique_ptr<t_runtime> urt=make_unique<t_runtime>();
    t_runtime&rt=*urt;double t=10;auto*pbase=(uint8_t*)base;
    typedef double(*t_jit_func)(t_runtime*);
    // ВАЖНО: Ищем функции в корне, а не в cur_func!
    // Сохраняем указатель на root перед компиляцией
    t_function* root_ptr = &root;
  
    auto r_func = find_func(root_ptr, "r");  // Ищем в root
    if (!r_func) {
      cerr << "Function 'r' not found!" << endl;
      return {};
    }
    auto*pr = (t_jit_func)(&pbase[r_func->code_offset]);
  
    auto g_func = find_func(root_ptr, "g");
    if (!g_func) {
      cerr << "Function 'g' not found!" << endl;
      return {};
    }
    auto*pg = (t_jit_func)(&pbase[g_func->code_offset]);
  
    auto b_func = find_func(root_ptr, "b");
    if (!b_func) {
      cerr << "Function 'b' not found!" << endl;
      return {};
    }
    auto*pb = (t_jit_func)(&pbase[b_func->code_offset]);  // Переименовал чтобы не конфликтовать с pb (указатель на base)

    //auto r_func=find_func(cur_func,"r");auto*pr=(t_jit_func)(&pbase[r_func->code_offset]);
    //auto g_func=find_func(cur_func,"g");auto*pg=(t_jit_func)(&pbase[g_func->code_offset]);
    //auto b_func=find_func(cur_func,"b");auto*pb=(t_jit_func)(&pbase[b_func->code_offset]);
    auto&X=rt.slots[0];auto&Y=rt.slots[1];auto&T=rt.slots[2];T=10;rt.slots[0]=0;auto*p=&arr[0];

    for(int y=0;y<cy;y++){
      Y=y;
      for(int x=0;x<cx;x++){
        auto&out=*p;
        X=x;
        out.r=pr(&rt)*255;
        out.g=pg(&rt)*255;
        out.b=pb(&rt)*255;
        p++;
      }
      if(y%100==0)cerr<<"y="<<y<<"\n";
    }
    auto mst=clock.MS();std::cerr<<"{\"render done in \":"<<(mst-msarr)<<"}"<<endl;
    lodepng_save_to_png(cx,cy,arr,string(out_fn.size()?out_fn:"out.png").c_str());
    auto msl=clock.MS();std::cerr<<"{\"lodepng_save_to_png done in \":"<<(msl-mst)<<"}"<<endl;
    return "done";//+string((char*)&jit.code[0]); // render done in ":2230.5
  }
};
};

static void test_2025_06_10(/*IEnvRTTI&Env*/string fn,bool dontoptimize=false)
{
  QapClock clock;
  string inp;
  if(fn.empty()&&inp.empty()){
    string line;
    while(std::getline(std::cin,line)){
      inp+=line+"\n";
    }
  }else if(inp.empty()){
    inp=file_get_contents(fn);
  }
  //return shader_gen_main(inp);
  if(inp.size()&&inp.back()=='\n')inp.pop_back();
  t_calc_test::t_ast2x64 v5;
  auto str2=v5.main(inp);
  string out="// "+::FToS(clock.MS())+" ms\n"+str2+"\n";
  std::cout<<out;
  std::cerr<<"{\"parse_ms\":"<<v5.parse_ms<<",\"total_ms\":"<<clock.MS()<<",\"g_unique_pool_ptr_counter\":"<<g_unique_pool_ptr_counter<<"}"<<endl;
  int gg=2;
}