//#include "StdAfx.h"

struct t_meta_lexer;
#include <unordered_map>
#include <random>
#include <iostream>
#include <math.h>

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

namespace t_calc_test{
#include "t_calc_impl.hpp"

struct t_jit{
  vector<string> asm_text;
  vector<uint8_t> code;
  void emit(const string&s){
    asm_text.push_back(s);
  }
  void push_xmm0(){
    emit("push_xmm0");
  }
  void pop_xmm1(){
    emit("pop_xmm1");
  }
  void load_const(double v){
    emit("movsd xmm0,const("+FToS(v)+")");
  }
  void load_var(const string&name){
    emit("movsd xmm0,["+name+"]");
  }
  void store_var(const string&name){
    emit("movsd ["+name+"],xmm0");
  }
  void add(){
    emit("addsd xmm0,xmm1");
  }
  void sub(){
    emit("subsd xmm1,xmm0");
    emit("movapd xmm0,xmm1");
  }
  void mul(){
    emit("mulsd xmm0,xmm1");
  }
  void div(){
    emit("divsd xmm1,xmm0");
    emit("movapd xmm0,xmm1");
  }
  void call(const string&fn){
    emit("call "+fn);
  }
  void init(){
    #define F(...)
    /*F("addsd xmm0,xmm1","F2 0F 58 C1",[](){});
    F("mulsd xmm0,xmm1","F2 0F 59 C1",[](){});
    F("divsd xmm1,xmm0","F2 0F 5E C8",[](){});
    F("subsd xmm1,xmm0","F2 0F 5C C8",[](){});
    F("ret","C3",[](){});
    F("mov rax,imm64","48 B8 ?? ?? ?? ?? ?? ?? ?? ??",[&](uint64_t value){emit_u64(value);});
    F("call rel32","E8 ?? ?? ?? ??",[&](void*target){
      auto rel=(char*)target-(char*)(curr_ip+5);
      emit_i32(rel);
    });*/
    F("movsd xmm0,[rcx+disp32]","F2 0F 10 81 ?? ?? ?? ??",[&](int disp){emit_i32(disp);});
    F("movsd [rcx+disp32],xmm0","F2 0F 11 81 ?? ?? ?? ??",[&](int disp){emit_i32(disp);});
    F("ret",                          "C3",                         [](){});

    F("push rax",                     "50",                         [](){});
    F("pop rax",                      "58",                         [](){});

    F("sub rsp,8",                    "48 83 EC 08",               [](){});
    F("add rsp,8",                    "48 83 C4 08",               [](){});

    F("movsd xmm0,[rcx+disp32]",      "F2 0F 10 81 ?? ?? ?? ??",   [&](int disp){emit_i32(disp);});
    F("movsd [rcx+disp32],xmm0",      "F2 0F 11 81 ?? ?? ?? ??",   [&](int disp){emit_i32(disp);});

    F("movsd xmm1,[rcx+disp32]",      "F2 0F 10 89 ?? ?? ?? ??",   [&](int disp){emit_i32(disp);});

    F("addsd xmm0,xmm1",              "F2 0F 58 C1",               [](){});
    F("subsd xmm1,xmm0",              "F2 0F 5C C8",               [](){});
    F("mulsd xmm0,xmm1",              "F2 0F 59 C1",               [](){});
    F("divsd xmm1,xmm0",              "F2 0F 5E C8",               [](){});

    F("movq xmm0,rax",                "66 48 0F 6E C0",            [](){});

    F("mov rax,imm64",                "48 B8 ?? ?? ?? ?? ?? ?? ?? ??",
      [&](uint64_t v){emit_u64(v);}
    );

    F("call rel32",                   "E8 ?? ?? ?? ??",
      [&](void* ptr){emit_rel32(ptr);}
    );
    #undef F
  }
};
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
struct t_codegen{
  vector<string> asm_text;
  vector<uint8_t> code;

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
  void emit_push_r12(){
    out("push r12");
    u8(0x41);
    u8(0x54);
  }
  void emit_pop_r12(){
    out("pop r12");
    u8(0x41);
    u8(0x5C);
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
    u8(8*(reg-XMM0));/*
    0:  f2 41 0f 10 00          movsd  xmm0,QWORD PTR [r8]
    5:  f2 41 0f 10 08          movsd  xmm1,QWORD PTR [r8]
    a:  f2 41 0f 10 10          movsd  xmm2,QWORD PTR [r8]
    f:  f2 41 0f 10 18          movsd  xmm3,QWORD PTR [r8]
    */
  }
  /*
  void emit_call(const string&name)
  {
    auto addr=(uint64_t)fn_addr[name];

    out("sub rsp,32");
    u8(0x48);u8(0x83);u8(0xEC);u8(0x20);

    out("mov rax,"+name);

    u8(0x48);
    u8(0xB8);
    u64(addr);

    out("call rax");

    u8(0xFF);
    u8(0xD0);

    out("add rsp,32");
    u8(0x48);u8(0x83);u8(0xC4);u8(0x20);
  }
   
Но тут есть баг

На Windows перед вызовом надо выделить shadow space:

sub rsp,32
call rax
add rsp,32

Поэтому реально:

out("sub rsp,32");

48 83 EC 20

mov rax,addr

call rax

out("add rsp,32");

48 83 C4 20
   
   */
  struct t_call_patch{size_t offset;string name;t_function*target=nullptr;};
  vector<t_call_patch> patches;
  /*void emit_call_user(const string&name)
  {
    out("call "+name);
    u8(0x48);
    u8(0xB8);
    auto pos=code.size();
    u64(0);
    u8(0xFF);
    u8(0xD0);
    patches.push_back({
      pos,
      name
    });
  }*/
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
  void emit_push_rbp(){
    out("push rbp");
    u8(0x55);
  }
  void emit_pop_rbp(){
    out("pop rbp");
    u8(0x5D);
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
  }
  void emit_call_rax(){
    out("sub rsp,32");
    u8(0x48);u8(0x83);u8(0xEC);u8(0x20);
    out("call rax");
    u8(0xFF);u8(0xD0);
    out("add rsp,32");
    u8(0x48);u8(0x83);u8(0xC4);u8(0x20);
  }
  void emit_call(const string&name){
    auto addr=(uint64_t)fn_addr[name];
    out("mov rax,"+name);
    u8(0x48);
    u8(0xB8);
    u64(addr);
    emit_call_rax();
  }
  void emit_call_user(const string&name)
  {
    out("mov rax,"+name);
    u8(0x48);
    u8(0xB8);
    auto pos=code.size();
    u64(0);
    patches.push_back({
      pos,
      name
    });
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
  }
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
  void emit_push_rcx(){
    out("push rcx");
    u8(0x51);
  }
  void emit_pop_rcx(){
    out("pop rcx");
    u8(0x59);
  }
  void emit_push_r13(){
    out("push r13");
    u8(0x41);
    u8(0x55);
  }
  void emit_pop_r13(){
    out("pop r13");
    u8(0x41);
    u8(0x5D);
  }
  void emit_mov_rcx_r13(){out("mov rcx,r13");u8(0x4C);u8(0x89);u8(0xE9);}
  void push_r8(){out("push r8");u8(0x41);u8(0x50);}
  void pop_r8(){out("pop r8");u8(0x41);u8(0x58);}
  unordered_map<string,void*> fn_addr;
  static double sin_(double x){return sin(x);}
  static double cos_(double x){return cos(x);}
  static double sqrt_(double x){return sqrt(x);}
  static double atan2_(double y,double x){return atan2(y,x);}
  static double min_(double y,double x){
    //cout<<"passed"<<endl;cin.get();
    return min(y,x);
  }
  static double max_(double y,double x){
    return max(y,x);
  }
  void init_fn_addr(){
    fn_addr["sin"]=(void*)sin_;
    fn_addr["cos"]=(void*)cos_;
    fn_addr["sqrt"]=(void*)sqrt_;
    fn_addr["atan2"]=(void*)atan2_;
    fn_addr["min"]=(void*)min_;
    fn_addr["max"]=(void*)max_;
    #define F(CODE)cout<<#CODE"="<<(CODE)<<endl;
    F(sin_(10))F(cos_(10))F(sqrt_(14))F(atan2_(10,20))F(min_(10,20))F(max_(10,20))
    #undef F
  }
};
struct t_ast2x64:t_calc::i_term::i_visitor,t_calc::i_stat::i_visitor{
  vector<t_function*> all_funcs;
  t_function*cur_func=nullptr;
  vector<t_scope> scopes;
  /*int get_var_id(const string&name){
    auto it=var2id.find(name);
    if(it!=var2id.end())return it->second;
    int id=id2var.size();
    var2id[name]=id;
    id2var.push_back(name);
    return id;
  }
  void init_builtin_vars(){
    get_var_id("x"); // это надо переделывать? теперь у нас все переменные локальные!
    get_var_id("y");
    get_var_id("t");
  }
  double*find_var(const string&name)
  {
    for(int i=int(scopes.size())-1;i>=0;i--){
      auto it=scopes[i].vars.find(name);
      if(it!=scopes[i].vars.end())
        return &it->second;
    }
    return nullptr;
  }
  t_function*find_func(const string&name)
  {
    for(int i=int(scopes.size())-1;i>=0;i--){
      auto it=scopes[i].funcs.find(name);
      if(it!=scopes[i].funcs.end())
        return &it->second;
    }
    return nullptr;
  }*/
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
    /*auto*pfunc=find_func(name);
    if(!pfunc){cerr<<"fatal error! function not found: "<<name<<endl;exit(-1);}
    auto&fn=*pfunc;*/
    auto*target=find_func(cur_func,name);
    /*for(size_t i=0;i<r.params->arr.size();i++){
      Do(r.params->arr[i].body);
      if(target){
        jit.emit_store_var(i);
      }else{
        if(i)jit.emit_set_call_arg(i,XMM0);
      }
    }
    if(target){
      jit.emit_call_user(target);
    }*/
    for(auto&arg:r.params->arr)
    {
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
      jit.emit_mov_rcx_r13();
      jit.emit_call_user(target);
      jit.emit_pop_rcx();
      jit.emit_free_call_frame(target->frame_size_aligned);
      jit.emit_pop_r13();
    }else{
      jit.push_r8();
      jit.emit_push_r13();
      jit.emit_push_r12();
      jit.emit_push_rcx();
      for(int i=int(r.params->arr.size())-1;i>=0;i--){
        jit.emit_stack_pop(XMM0+i);
      }
      jit.emit_call(name);
      jit.emit_pop_rcx();
      jit.emit_pop_r12();
      jit.emit_pop_r13();
      jit.pop_r8();
    }
    /*if(target){
      jit.emit_push_r13();
      jit.emit_alloc_call_frame(target->frame_size_aligned);
      for(size_t i=0;i<r.params->arr.size();i++){
        Do(r.params->arr[i].body);
        auto slot=target->get_slot(target->args[i]);
        jit.emit_store_call_arg(slot);
      }
      jit.emit_push_rcx();
      jit.emit_mov_rcx_r13();
      jit.emit_call_user(target);
      jit.emit_pop_rcx();
      jit.emit_free_call_frame(target->frame_size_aligned);
      jit.emit_pop_r13();
    }else{
      jit.push_r8();
      jit.emit_push_r13();
      jit.emit_push_r12();
      jit.emit_push_rcx();
      for(size_t i=0;i<r.params->arr.size();i++){
        Do(r.params->arr[i].body);
        if(i)jit.emit_set_call_arg(i,XMM0);
      }
      jit.emit_call(name);
      jit.emit_pop_rcx();
      jit.emit_pop_r12();
      jit.emit_pop_r13();
      jit.pop_r8();
    }*/
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
    if(cur_func->parent==&root){
      jit.emit_root_func_begin(fn.frame_size_aligned);
    }else{
      jit.emit_func_begin(fn.frame_size_aligned);
    }
    Do(*fn.body);
    if(cur_func->parent==&root){
      jit.emit_root_func_end(fn.frame_size_aligned);
    }else{
      jit.emit_func_end(fn.frame_size_aligned);
    }
  }
  double parse_ms=0;
  string main(const string&data){
    t_calc tar;QapClock clock;string errmsg;
    scopes.resize(4096);
    scopes.push_back({});
    //init_builtin_vars();
    root.name="<root>";
    cur_func=&root;
    jit.init_fn_addr();
    jit.emit_prolog();
    auto&ctx=scopes.back();/*
    {
      auto&s=ctx.funcs["sin"];s.args={"x"};
      struct t_sin:t_function::t_sys_func{double main(const vector<t_value>&args)override{return sin(args[0].num);}};
      static t_sin f;s.pfunc=&f;
    }
    {
      auto&s=ctx.funcs["cos"];s.args={"x"};
      struct t_cos:t_function::t_sys_func{double main(const vector<t_value>&args)override{return cos(args[0].num);}};
      static t_cos f;s.pfunc=&f;
    }
    {
      auto&s=ctx.funcs["sqrt"];s.args={"x"};
      struct t_sqrt:t_function::t_sys_func{double main(const vector<t_value>&args)override{return sqrt(args[0].num);}};
      static t_sqrt f;s.pfunc=&f;
    }
    {
      auto&s=ctx.funcs["min"];s.args={"x","y"};
      struct t_min:t_function::t_sys_func{double main(const vector<t_value>&args)override{return min(args[0].num,args[1].num);}};
      static t_min f;s.pfunc=&f;
    }
    {
      auto&s=ctx.funcs["max"];s.args={"x","y"};
      struct t_max:t_function::t_sys_func{double main(const vector<t_value>&args)override{return max(args[0].num,args[1].num);}};
      static t_max f;s.pfunc=&f;
    }
    {
      auto&s=ctx.funcs["floor"];s.args={"x"};
      struct t_floor:t_function::t_sys_func{double main(const vector<t_value>&args)override{return floor(args[0].num);}};
      static t_floor f;s.pfunc=&f;
    }
    {
      auto&s=ctx.funcs["atan2"];s.args={"y","x"};
      struct t_atan2:t_function::t_sys_func{double main(const vector<t_value>&args)override{return atan2(args[0].num,args[1].num);}};
      static t_atan2 f;s.pfunc=&f;
    }
    {
      auto&s=ctx.funcs["abs"];s.args={"x"};
      struct t_abs:t_function::t_sys_func{double main(const vector<t_value>&args)override{return abs(args[0].num);}};
      static t_abs f;s.pfunc=&f;
    }
    {
      auto&s=ctx.funcs["noise"];s.args={"x","y"};
      struct t_noise:t_function::t_sys_func{double main(const vector<t_value>&args)override{return sin(args[0].num*127.1+args[1].num*311.7)*43758.5453;}};
      static t_noise f;s.pfunc=&f;
    }*/
    //noise(x,y)=sin(x*127.1+y*311.7)*43758.5453-floor(sin(x*127.1+y*311.7)*43758.5453);
    auto r=load_obj_full(tar,data,true,&errmsg);
    parse_ms=clock.MS();
    std::cerr<<"{\"parse_ms\":"<<parse_ms<<"}"<<endl;
    if(!r.ok){QapDebugMsg(r.msg);return {};}
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
    auto r_func=find_func(cur_func,"r");auto*pr=(t_jit_func)(&pbase[r_func->code_offset]);
    auto g_func=find_func(cur_func,"g");auto*pg=(t_jit_func)(&pbase[g_func->code_offset]);
    auto b_func=find_func(cur_func,"b");auto*pb=(t_jit_func)(&pbase[b_func->code_offset]);
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
    /*auto&rbody=*ctx.funcs["r"].body;auto&gbody=*ctx.funcs["g"].body;auto&bbody=*ctx.funcs["b"].body;
    auto&Y=ctx.vars["y"];auto&X=ctx.vars["x"];auto&T=ctx.vars["t"];T.num=10;
    for(int y=0;y<cy;y++){
      Y.num=y;
      for(int x=0;x<cx;x++){
        X.num=x;
        auto&out=arr[x+cx*y];
        Do(rbody);
        out.r=last.num*255;
        Do(gbody);
        out.g=last.num*255;
        Do(bbody);
        out.b=last.num*255;
        out.a=255;
        //out=0xFFFF0000;
      }
      if(y%10==0)cerr<<"y="<<y<<"\n";
    }*/
    auto mst=clock.MS();std::cerr<<"{\"render done in \":"<<(mst-msarr)<<"}"<<endl;
    lodepng_save_to_png(cx,cy,arr,"out.png");
    auto msl=clock.MS();std::cerr<<"{\"lodepng_save_to_png done in \":"<<(msl-mst)<<"}"<<endl;
    return "done";//+string((char*)&jit.code[0]); // render done in ":2230.5
  }
};
};

static void test_2025_06_10(/*IEnvRTTI&Env*/string fn,bool dontoptimize=false)
{
  QapClock clock;
  string inp;
  //fn.clear();
  /*inp="r(x,y,t)=(sin(x*0.01+t*0.05)*0.5+0.5)*(cos(y*0.01+t*0.03)*0.5+0.5);"
      "g(x,y,t)=(sin(y*0.01+t*0.07)*0.5+0.5)*(cos(x*0.01+t*0.02)*0.5+0.5);"
      "b(x,y,t)=(sin((x+y)*0.005+t*0.04)*0.5+0.5)*(cos(x*0.02-y*0.02+t*0.06)*0.5+0.5);";

      
  inp="r(x,y,t)=1;g(x,y,t)=0.5;b(x,y,t)=(x/1920+y/1200+t/100)*0.33;";*/
  if(fn.empty()&&inp.empty()){
    string line;
    while(std::getline(std::cin,line)){
      inp+=line+"\n";
    }
  }else if(inp.empty()){
    inp=file_get_contents(fn);
  }
  if(inp.size()&&inp.back()=='\n')inp.pop_back();
  t_calc_test::t_ast2x64 v5;
  auto str2=v5.main(inp);
  string out="// "+FToS(clock.MS())+" ms\n"+str2+"\n";
  std::cout<<out;
  std::cerr<<"{\"parse_ms\":"<<v5.parse_ms<<",\"total_ms\":"<<clock.MS()<<",\"g_unique_pool_ptr_counter\":"<<g_unique_pool_ptr_counter<<"}"<<endl;
  int gg=2;
}