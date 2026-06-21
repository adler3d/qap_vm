//#include "StdAfx.h"

struct t_meta_lexer;
#include <unordered_map>
#include <random>
#include <iostream>
#include <math.h>

#include "lodepng.h"
#include "lodepng.cpp"

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
struct t_ast2value_conv :
  t_calc::i_term::i_visitor,
  t_calc::i_stat::i_visitor
{
  struct t_value {
    double num = 0.0;
  };
  struct t_function {
    vector<string> args;
    TAutoPtr<t_calc::i_stat>*body=nullptr;
    struct t_sys_func{virtual double main(const vector<t_value>&args){return 0;}};
    t_sys_func*pfunc=nullptr;
  };
  struct t_scope
  {
    unordered_map<string,t_value> vars;
    unordered_map<string,t_function> funcs;
  };
  vector<t_scope> scopes;
  t_value*find_var(const string&name)
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
  }
  //t_context ctx;
  //vector<t_context> ctxs;
  t_value last;
  bool eval_mode = false;
  double as_num() const { return last.num; }
  void set(double v) { last.num = v; }
  static double to_double(const string& s){
    return atof(s.c_str());
  }
  template<class TYPE>
  void Do(TYPE&r){r.Use(*this);}
  template<class TYPE>
  void Do(vector<TYPE>&arr){for(auto&ex:arr)Do(&ex);}
  template<class TYPE>
  void Do(vector<TAutoPtr<TYPE>>&arr){for(auto&ex:arr)if(ex)Do(*ex.get());}
  template<class TYPE>
  void Do(TAutoPtr<TYPE>&r){if(r)Do(*r.get());}
  void Do(t_calc::t_term& r){
    Do(r.value);
  }
  void Do(t_calc::t_number& r){
    set(to_double(r.value));
  }
  void Do(t_calc::t_scope& r){
    Do(r.value);
  }
  void Do(t_calc::t_divmul::t_elem& r){
    double lhs = as_num();
    Do(r.expr);
    double rhs = as_num();

    if(r.oper == "*") set(lhs * rhs);
    else set(lhs / rhs);
  }
  void Do(t_calc::t_divmul& r){
    Do(r.first);
    for(auto& e : r.arr){
      Do(e);
    }
  }
  void Do(t_calc::t_addsub::t_elem& r){
    double lhs = as_num();
    Do(r.expr);
    double rhs = as_num();

    if(r.oper == "+") set(lhs + rhs);
    else set(lhs - rhs);
  }
  void Do(t_calc::t_addsub& r){
    Do(r.first);
    for(auto& e : r.arr){
      Do(e);
    }
  }
  void Do(t_calc::t_varcall& r){
    const string& name = r.name;
    if(!r.params){
      set(find_var(name)->num);
      return;
    }
    auto*pfunc=find_func(name);
    if(!pfunc){cerr<<"fatal error! function not found: "<<name<<endl;exit(-1);}
    auto&fn=*pfunc;
    if(!fn.body){
      vector<t_value> args(fn.args.size());
      for(size_t i=0;i<fn.args.size();i++){
        Do(r.params->arr[i].body);
        args[i]=last;
      }
      last.num=fn.pfunc->main(args);
    }else{
      scopes.push_back({});
      for(size_t i=0;i<fn.args.size();i++){
        Do(r.params->arr[i].body);
        scopes.back().vars[fn.args[i]] = last;
      }
      Do(*fn.body);
      scopes.pop_back();
    }
  }
  //string to(const t_calc::t_varcall::t_impl&r){string out=" ";out[0]=r.A;out+=r.B;return out;}
  void Do(t_calc::t_func_stat& r){
    t_function fn;
    for(auto& a : r.args){
      fn.args.push_back(a.name);
    }
    fn.body = &r.body;
    scopes.back().funcs[r.func] = fn;
  }
  void Do(t_calc::t_assign_stat& r){
    Do(r.expr);
    scopes.back().vars[r.var] = last;
  }
  void Do(t_calc& r){
    for(auto& st : r.arr){
      Do(*st.get());
    }
  }
  void Do(t_calc::t_block_stat&r){
    scopes.push_back({});
    for(auto& st : r.arr){
      Do(*st.get());
    }
    scopes.pop_back();
  }
  void Do(t_calc::t_solo_stat&r){
    Do(r.expr);
  }
  double parse_ms=0;
  string main(const string&data){
    t_calc tar;QapClock clock;string errmsg;
    scopes.resize(4096);
    scopes.push_back({});
    auto&ctx=scopes.back();
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
    }
    //noise(x,y)=sin(x*127.1+y*311.7)*43758.5453-floor(sin(x*127.1+y*311.7)*43758.5453);
    auto r=load_obj_full(tar,data,true,&errmsg);
    parse_ms=clock.MS();
    std::cerr<<"{\"parse_ms\":"<<parse_ms<<"}"<<endl;
    if(!r.ok){QapDebugMsg(r.msg);return {};}
    Do(tar);
    auto mstar=clock.MS();std::cerr<<"{\"Do(tar) done in \":"<<(mstar-parse_ms)<<"}"<<endl;
    int cx=1920;int cy=1200;vector<QapColor> arr(cx*cy);
    auto msarr=clock.MS();std::cerr<<"{\"arr.resize done in \":"<<(msarr-mstar)<<"}"<<endl;
    auto&rbody=*ctx.funcs["r"].body;auto&gbody=*ctx.funcs["g"].body;auto&bbody=*ctx.funcs["b"].body;
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
    }
    auto mst=clock.MS();std::cerr<<"{\"render done in \":"<<(mst-msarr)<<"}"<<endl;
    lodepng_save_to_png(cx,cy,arr,"out.png");
    auto msl=clock.MS();std::cerr<<"{\"lodepng_save_to_png done in \":"<<(msl-mst)<<"}"<<endl;
    return to_string(last.num);
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
  t_calc_test::t_ast2value_conv v5;
  auto str2=v5.main(inp);
  string out="// "+FToS(clock.MS())+" ms\n"+str2+"\n";
  std::cout<<out;
  std::cerr<<"{\"parse_ms\":"<<v5.parse_ms<<",\"total_ms\":"<<clock.MS()<<",\"g_unique_pool_ptr_counter\":"<<g_unique_pool_ptr_counter<<"}"<<endl;
  int gg=2;
}