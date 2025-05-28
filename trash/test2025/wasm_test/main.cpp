#include <emscripten.h>
#include <vector>
#include <map>
#include <functional>
#include <stdlib.h>
#include <time.h>
using namespace std;
#include "thirdparty/sweepline/sweepline.hpp"
#define QAP_DEBUG
#ifdef _WIN32
class QapClock{
public:
  typedef long long int int64;
  int64 freq,beg,tmp;
  bool run;
public:
  QapClock(){QueryPerformanceFrequency((LARGE_INTEGER*)&freq);run=false;tmp=0;Start();}
  void Start(){QueryPerformanceCounter((LARGE_INTEGER*)&beg);run=true;}
  void Stop(){QueryPerformanceCounter((LARGE_INTEGER*)&tmp);run=false;tmp-=beg;}
  double Time(){if(run)QueryPerformanceCounter((LARGE_INTEGER*)&tmp);return run?double(tmp-beg)/double(freq):double(tmp)/double(freq);}
  double MS()
  {
    double d1000=1000.0;
    if(run)QueryPerformanceCounter((LARGE_INTEGER*)&tmp);
    if(run)return (double(tmp-beg)*d1000)/double(freq);
    if(!run)return (double(tmp)*d1000)/double(freq);
    return 0;
  }
  static int64 qpc(){int64 tmp;QueryPerformanceCounter((LARGE_INTEGER*)&tmp);return tmp;}
};
#else
#ifndef __EMSCRIPTEN__
//https://github.com/copilot/share/c05603ac-4240-8476-b813-be01a48a201d
#include <chrono>
class QapClock{
public:
  typedef long long int int64;
  int64 freq,beg,tmp;
  bool run;
  typedef std::chrono::high_resolution_clock clock;
  typedef std::chrono::time_point<clock> time_point;
  time_point t_beg,t_tmp;
public:
  QapClock(){Start();}
  #define F(diff)std::chrono::duration_cast<std::chrono::microseconds>(diff).count()
  void Start(){t_beg=clock::now();run=true;}
  void Stop(){t_tmp=clock::now();run=false;tmp=F(t_tmp-t_beg);}
  double MS(){
    if(run){t_tmp=clock::now();return F(t_tmp-t_beg)*0.001;}
    return double(tmp)*0.001;
  }
  static int64 qpc(){return F(clock::now().time_since_epoch());}
  #undef F
};
#else
class QapClock{
public:
  typedef long long int int64;
  int64 freq,beg,tmp;
  bool run;
public:
  QapClock(){Start();}
  void Start(){beg=qpc();run=true;}
  void Stop(){run=false;tmp=qpc()-beg;}
  double MS(){
    if(run){tmp=qpc();return (tmp-beg)*0.001;}
    return tmp*0.001;
  }
  static int64 qpc(){return EM_ASM_INT({return (1000*performance.now())|0;});}
};
#define QAP_EM_LOG(TEXT)EM_ASM({console.log(UTF8ToString($0));},string(TEXT).c_str());
#endif //__EMSCRIPTEN__
#endif //_WIN32
#ifndef _WIN32
#define VK_SLEEP          0x5F
#define VK_NUMPAD0        0x60
#define VK_NUMPAD1        0x61
#define VK_NUMPAD2        0x62
#define VK_NUMPAD3        0x63
#define VK_NUMPAD4        0x64
#define VK_NUMPAD5        0x65
#define VK_NUMPAD6        0x66
#define VK_NUMPAD7        0x67
#define VK_NUMPAD8        0x68
#define VK_NUMPAD9        0x69
#define VK_MULTIPLY       0x6A
#define VK_ADD            0x6B
#define VK_SEPARATOR      0x6C
#define VK_SUBTRACT       0x6D
#define VK_DECIMAL        0x6E
#define VK_DIVIDE         0x6F
#define VK_F1             0x70
#define VK_F2             0x71
#define VK_F3             0x72
#define VK_F4             0x73
#define VK_F5             0x74
#define VK_F6             0x75
#define VK_F7             0x76
#define VK_F8             0x77
#define VK_F9             0x78
#define VK_F10            0x79
#define VK_F11            0x7A
#define VK_F12            0x7B
#define VK_F13            0x7C
#define VK_F14            0x7D
#define VK_F15            0x7E
#define VK_F16            0x7F
#define VK_F17            0x80
#define VK_F18            0x81
#define VK_F19            0x82
#define VK_F20            0x83
#define VK_F21            0x84
#define VK_F22            0x85
#define VK_F23            0x86
#define VK_F24            0x87
#define VK_ESCAPE         0x1B

#define VK_CONVERT        0x1C
#define VK_NONCONVERT     0x1D
#define VK_ACCEPT         0x1E
#define VK_MODECHANGE     0x1F

#define VK_SPACE          0x20
#define VK_PRIOR          0x21
#define VK_NEXT           0x22
#define VK_END            0x23
#define VK_HOME           0x24
#define VK_LEFT           0x25
#define VK_UP             0x26
#define VK_RIGHT          0x27
#define VK_DOWN           0x28
#define VK_SELECT         0x29
#define VK_PRINT          0x2A
#define VK_EXECUTE        0x2B
#define VK_SNAPSHOT       0x2C
#define VK_INSERT         0x2D
#define VK_DELETE         0x2E
#define VK_HELP           0x2F
#define VK_CLEAR          0x0C
#define VK_RETURN         0x0D
#define VK_BACK           0x08
#define VK_TAB            0x09
#define VK_SHIFT          0x10
#define VK_CONTROL        0x11
#define VK_MENU           0x12
#define VK_PAUSE          0x13
#define VK_CAPITAL        0x14

#endif
#ifdef __EMSCRIPTEN__
static string file_get_contents(const string&fn){
  QAP_EM_LOG("file_get_contents:"+fn);
  int length=EM_ASM_INT({
    let key=UTF8ToString($0);
    let val=localStorage.getItem(key);
    if(val===null)return 0;
    return lengthBytesUTF8(val);
  },fn.c_str());
  if(!length)return {};
  string out;out.resize(length);
  EM_ASM({
    let key=UTF8ToString($0);
    let val=localStorage.getItem(key);
    if(val!==null)stringToUTF8(val,$1,$2+1);
  },fn.c_str(),out.data(),length);
  return out;
}
static bool file_put_contents(const string&fn,const string&mem){
  QAP_EM_LOG("file_put_contents:"+fn);
  EM_ASM({
    let key=UTF8ToString($0);
    let val=UTF8ToString($1);
    localStorage.setItem(key,val);
  },fn.c_str(),mem.c_str());
  return true;
}
#else
static bool file_put_contents(const string&FN,const string&mem){std::fstream f(FN,std::ios::out|std::ios::trunc);f<<mem;return true;}
static string file_get_contents(const string&fn){std::ifstream file(fn);return std::string((std::istreambuf_iterator<char>(file)),(std::istreambuf_iterator<char>()));}
#endif
template<class TYPE>
void QapPopFront(vector<TYPE>&arr)
{
  int last=0;
  for(int i=1;i<arr.size();i++)
  {
    auto&ex=arr[i];
    if(last!=i)
    {
      auto&ax=arr[last];
      ax=std::move(ex);
    }
    last++;
  }
  if(last==arr.size())return;
  arr.resize(last);
}
static vector<string> split(const string&s,const string&needle)
{
  vector<string> arr;
  if(s.empty())return arr;
  size_t p=0;
  for(;;){
    auto pos=s.find(needle,p);
    if(pos==std::string::npos){arr.push_back(s.substr(p));return arr;}
    arr.push_back(s.substr(p,pos-p));
    p=pos+needle.size();
  }
  return arr;
}
static string join(const vector<string>&arr,const string&glue)
{
  string out;
  size_t c=0;
  size_t dc=glue.size();
  for(int i=0;i<arr.size();i++){if(i)c+=dc;c+=arr[i].size();}
  out.reserve(c);
  for(int i=0;i<arr.size();i++){if(i)out+=glue;out+=arr[i];}
  return out;
}
struct TSys{int UPS=128;struct TSM{int W=1920,H=1024;};TSM SM;bool UPS_enabled=true;void ResetClock(){}}; TSys Sys;
static const int Sys_UPD=64;
inline string IToS(const int&val){return to_string(val);}
inline string FToS(const double&val){return to_string(val);}
inline string FToS(const float&val){return to_string(val);}
#ifdef __EMSCRIPTEN__
#define __debugbreak()EM_ASM({throw new Error("__debugbreak");});
#endif
inline bool SysQapAssert(const string&exp,bool&ignore,const string&filename,const int line,const string&funcname);
inline bool SysQapDebugMsg(const string&msg,bool&ignore,const string&filename,const int line,const string&funcname);
#if(defined(_DEBUG)||defined(QAP_DEBUG))
#define QapAssert(_Expression)if(!bool(_Expression)){static bool ignore=false;if(SysQapAssert((#_Expression),ignore,__FILE__,__LINE__,__FUNCTION__))__debugbreak();}
#else
#define QapAssert(_Expression)if(bool(_Expression)){};
#endif
#if(defined(_DEBUG)||defined(QAP_DEBUG))
#define QapDebugMsg(_Message){static bool ignore=false;if(SysQapDebugMsg((_Message),ignore,__FILE__,__LINE__,__FUNCTION__))__debugbreak();}
#else
#define QapDebugMsg(_Message)
#endif
#if(defined(_DEBUG)||defined(QAP_DEBUG))
#define QapNoWay(){QapDebugMsg("no way!");}
#else
#define QapNoWay()
#endif
enum QapMsgBoxRetval{qmbrSkip,qmbrBreak,qmbrIgnore};
inline int WinMessageBox(const string&caption,const string&text)
{
  #ifdef _WIN32
  string full_text=text+"\n\n    [Skip]            [Break]            [Ignore]";
  const int nCode=MessageBoxA(NULL,full_text.c_str(),caption.c_str(),MB_CANCELTRYCONTINUE|MB_ICONHAND|MB_SETFOREGROUND|MB_TASKMODAL);
  QapMsgBoxRetval retval=qmbrSkip;
  if(IDCONTINUE==nCode)retval=qmbrIgnore;
  if(IDTRYAGAIN==nCode)retval=qmbrBreak;
  return retval;
  #else
  #ifdef __EMSCRIPTEN__
  EM_ASM({alert(UTF8ToString($0)+"\n"+UTF8ToString($1));},int(caption.c_str()),int(text.c_str()));
  return qmbrBreak;
  #endif
  #endif
}
typedef int(*TQapMessageBox)(const string&caption,const string&text);
struct TMessageBoxCaller
{
  static int Call(const string&caption,const string&text)
  {
    return Get()(caption,text);
  }
  static TQapMessageBox&Get()
  {
    static TQapMessageBox func=WinMessageBox;
    return func;
  }
  struct t_hack
  {
    TQapMessageBox old;
    t_hack(TQapMessageBox func)
    {
      old=Get();
      Get()=func;
    }
    ~t_hack()
    {
      Get()=old;
    }
  };
};
inline bool SysQapAssert(const string&exp,bool&ignore,const string&filename,const int line,const string&funcname)
{
  if(ignore)return false;
  std::string text="Source file :\n"+filename
      +"\n\nLine : "+std::to_string(line)
      +"\n\nFunction :\n"+funcname
      +"\n\nAssertion failed :\n"+exp;
  auto retval=(QapMsgBoxRetval)TMessageBoxCaller::Call("Assertion failed",text);
  if(qmbrIgnore==retval)ignore=true;
  return qmbrBreak==retval;
}
inline bool SysQapDebugMsg(const string&msg,bool&ignore,const string&filename,const int line,const string&funcname)
{
  if(ignore)return false;
  std::string text="Source file :\n"+filename
      +"\n\nLine : "+std::to_string(line)
      +"\n\nFunction :\n"+funcname
      +"\n\nDebug message :\n"+msg;
  auto retval=(QapMsgBoxRetval)TMessageBoxCaller::Call("Debug message",text);
  if(qmbrIgnore==retval)ignore=true;
  return qmbrBreak==retval;
}
template<typename TYPE>
class QapPool{
public:
  struct Rec
  {
    bool used;
    TYPE data;
    Rec():used(false){}
  };
  vector<Rec>Arr;
  int Size;
  int MaxSize;
public:
  QapPool(int MaxSize=0):Size(0),MaxSize(MaxSize){Arr.resize(MaxSize);}
  void NewInstance(TYPE*&pVar)
  {
    QapAssert(Size<MaxSize);
    for(int i=0;i<Arr.size();i++)
    {
      if(!Arr[i].used)
      {
        Arr[i].used=true;
        Size++;
        pVar=&Arr[i].data;
        return;
      }
    }
  }
  void FreeInstance(TYPE*&pVar){
    QapAssert(Size>0);
    int id=int((int)pVar-(int)&Arr[0].data)/sizeof(Arr[0]);
    for(int i=id;i<Arr.size();i++)
    {
      if(&Arr[i].data==pVar)
      {
        Arr[i].used=false;
        Size--;
        pVar=NULL;
        return;
      }
    }
    QapAssert(pVar=NULL);
  }
  template<typename FUNC>
  void ForEach(FUNC&Func)
  {
    int c=Size;
    for(int i=0;i<Arr.size();i++)
    {
      if(!c)break;
      Rec&it=Arr[i];
      if(it.used)
        Func(&it.data);
    }
  }
};
typedef double real;
template<typename TYPE>inline TYPE Lerp(const TYPE&A,const TYPE&B,const real&v){return A+(B-A)*v;}
template<class TYPE>inline TYPE Clamp(const TYPE&v,const TYPE&a,const TYPE&b){return max(a,min(v, b));}
const real Pi=3.14159265;
const real Pi2=Pi*2;
const real PiD2=Pi/2;
const real PiD4=Pi/4;
template<typename TYPE>TYPE Sign(const TYPE&value){if(value>0){return TYPE(+1);}else{return TYPE(value<0?-1:0);};};
template<typename TYPE>bool InDip(const TYPE&min,const TYPE&val,const TYPE&max){return (val>=min)&&(val<=max);};
struct vec2d{
public:
  real x,y;
  vec2d():x(0),y(0){}
  vec2d(real x,real y):x(x),y(y){}
  vec2d(const vec2d&v):x(v.x),y(v.y){}
  vec2d&operator=(const vec2d&v){x=v.x;y=v.y;return *this;}
  vec2d operator+()const{return *this;}
  vec2d operator-()const{return vec2d(-x,-y);}
  vec2d&operator+=(const vec2d&v){x+=v.x;y +=v.y;return *this;}
  vec2d&operator-=(const vec2d&v){x-=v.x; y-=v.y;return *this;}
  vec2d&operator*=(const real&f){x*=f;y*=f;return *this;}
  vec2d&operator/=(const real&f){x/=f;y/=f;return *this;}
public:
  vec2d Rot(const vec2d&OX)const{real M=OX.Mag();return vec2d(((x*+OX.x)+(y*OX.y))/M,((x*-OX.y)+(y*OX.x))/M);}
  vec2d UnRot(const vec2d&OX)const{real M=OX.Mag();if(M==0.0f){return vec2d(0,0);};return vec2d(((x*OX.x)+(y*-OX.y))/M,((x*OX.y)+(y*+OX.x))/M);}
  vec2d Ort()const{return vec2d(-y,x);}
  vec2d Norm()const{if((x==0)&&(y==0)){return vec2d(0,0);}return vec2d(x/this->Mag(),y/this->Mag());}
  vec2d SetMag(const real&val)const{return this->Norm()*val;}
  vec2d Mul(const vec2d&v)const{return vec2d(x*v.x,y*v.y);}
  vec2d Div(const vec2d&v)const{return vec2d(v.x!=0?x/v.x:x,v.y!=0?y/v.y:y);}
  real GetAng()const{return atan2(y,x);}
  real Mag()const{return sqrt(x*x+y*y);}
public:
  friend vec2d operator+(const vec2d&u,const vec2d&v){return vec2d(u.x+v.x,u.y+v.y);}
  friend vec2d operator-(const vec2d&u,const vec2d&v){return vec2d(u.x-v.x,u.y-v.y);}
  friend vec2d operator*(const vec2d&u,const real&v){return vec2d(u.x*v,u.y*v);}
  friend vec2d operator*(const real&u,const vec2d&v){return vec2d(u*v.x,u*v.y);}
  friend bool operator==(const vec2d&u,const vec2d&v){return (u.x==v.x)||(u.y==v.y);}
  friend bool operator!=(const vec2d&u,const vec2d&v){return (u.x!=v.x)||(u.y!=v.y);}
public:
  //friend inline static vec2d Vec2dEx(const real&ang,const real&mag){return vec2d(cos(ang)*mag,sin(ang)*mag);}
public:
  #ifdef BOX2D_H
    operator b2Vec2()const{return b2Vec2(x,y);}
    vec2d(const b2Vec2& v):x(v.x),y(v.y){}
  #endif
};
inline vec2d Vec2dEx(const real&ang,const real&mag){return vec2d(cos(ang)*mag,sin(ang)*mag);}
inline int round(const real&val){return int(val>=0?val+0.5:val-0.5);}//{return int(val);}
inline static real dot(const vec2d&a,const vec2d&b){return a.x*b.x+a.y*b.y;}
inline static real cross(const vec2d&a,const vec2d&b){return a.x*b.y-a.y*b.x;}
typedef unsigned char uchar;
class QapColor{
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
  QapColor(uchar A,uchar R,uchar G,uchar B):a(A),r(R),g(G),b(B) {}
  QapColor(uchar R,uchar G,uchar B):a(255),r(R),g(G),b(B) {}
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
      b=uchar(Clamp(real(b)*f,0.0,255.0));
    }
    ;
    {
      g=uchar(Clamp(real(g)*f,0.0,255.0));
    }
    ;
    {
      r=uchar(Clamp(real(r)*f,0.0,255.0));
    }
    ;
    return *this;
  }
  QapColor&operator/=(real r)
  {
    real f=1.0/r;
    {
      b=uchar(Clamp(real(b)*f,0.0,255.0));
    }
    ;
    {
      g=uchar(Clamp(real(g)*f,0.0,255.0));
    }
    ;
    {
      r=uchar(Clamp(real(r)*f,0.0,255.0));
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
  uchar GetLuminance()const
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
      O.b=uchar(Clamp(real(A.b)*tA+real(B.b)*tB,0.0,255.0));
    }
    {
      O.g=uchar(Clamp(real(A.g)*tA+real(B.g)*tB,0.0,255.0));
    }
    {
      O.r=uchar(Clamp(real(A.r)*tA+real(B.r)*tB,0.0,255.0));
    }
    {
      O.a=uchar(Clamp(real(A.a)*tA+real(B.a)*tB,0.0,255.0));
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
class vec2f{
public:
  typedef vec2f SelfClass;
public:
  float x;
  float y;
public:
  vec2f():x(0),y(0) {}
  vec2f(const vec2d&v)  {
    x=v.x;
    y=v.y;
  }
  vec2f(float x,float y):x(x),y(y) {};
  void set_zero()  {
    x=0.0f;
    y=0.0f;
  }
public:
  friend vec2f operator*(const vec2f&u,const float&v){
    return vec2f(u.x*v,u.y*v);
  }
  friend vec2f operator*(const float&u,const vec2f&v){
    return vec2f(u*v.x,u*v.y);
  }
  friend vec2f operator+(const vec2f&u,const vec2f&v){
    return vec2f(u.x+v.x,u.y+v.y);
  }
  friend vec2f operator-(const vec2f&u,const vec2f&v){
    return vec2f(u.x-v.x,u.y-v.y);
  }
  friend void operator*=(vec2f&ref,float r){ref.x*=r;ref.y*=r;}
public:
  void operator+=(const vec2d&v){
    x+=v.x;
    y+=v.y;
  }
  void operator-=(const vec2d&v){
    x-=v.x;
    y-=v.y;
  }
public:
  friend vec2f operator*(float u,const vec2f&v){
    return vec2f(u*v.x,u*v.y);
  }
  operator vec2d()const{
    return vec2d(x,y);
  }
public:
  friend bool operator==(const vec2f&u,const vec2f&v){
    return (u.x==v.x)&&(u.y==v.y);
  }
  friend bool operator!=(const vec2f&u,const vec2f&v){
    return (u.x!=v.x)||(u.y!=v.y);
  }
  vec2f operator-()const{
    return vec2f(-x,-y);
  }
};
inline static real dot(const vec2f&a,const vec2f&b){return a.x*b.x+a.y*b.y;}
inline static real cross(const vec2f&a,const vec2f&b){return a.x*b.y-a.y*b.x;}
class QapMat22
{
public:
  vec2f col1;
  vec2f col2;
public:
  QapMat22():col1(1,0),col2(0,1) {}
  QapMat22(const vec2f&c1,const vec2f&c2)
  {
    col1=c1;
    col2=c2;
  }
  QapMat22(float a11,float a12,float a21,float a22)
  {
    col1.x=a11;
    col1.y=a21;
    col2.x=a12;
    col2.y=a22;
  }
  explicit QapMat22(float ang)
  {
    float c=cosf(ang);
    float s=sinf(ang);
    col1.x=c;
    col2.x=-s;
    col1.y=s;
    col2.y=+c;
  }
  void set(const vec2f&c1,const vec2f&c2)
  {
    col1=c1;
    col2=c2;
  }
  void set(float ang)
  {
    float c=cosf(ang);
    float s=sinf(ang);
    col1.x=c;
    col2.x=-s;
    col1.y=s;
    col2.y=+c;
  }
  void set_ident()
  {
    col1.x = 1.0f;
    col2.x = 0.0f;
    col1.y = 0.0f;
    col2.y = 1.0f;
  }
  void set_zero()
  {
    col1.x = 0.0f;
    col2.x = 0.0f;
    col1.y = 0.0f;
    col2.y = 0.0f;
  }
  float GetAngle()const
  {
    return atan2(col1.y, col1.x);
  }
  void mul(real r){col1*=r;col2*=r;}
};
class transform2f
{
public:
  vec2f p;
  QapMat22 r;
public:
  transform2f() {}
  transform2f(const vec2f&p,const QapMat22&r):p(p),r(r) {}
  explicit transform2f(const vec2f&p):p(p) {}
  void set_ident()
  {
    p.set_zero();
    r.set_ident();
  }
  void set(const vec2d&p,float ang)
  {
    this->p=p;
    this->r.set(ang);
  }
  float getAng()const
  {
    return atan2(r.col1.y,r.col1.x);
  }
public:
  friend vec2f operator*(const transform2f&T,const vec2f&v)
  {
    float x=(+T.r.col1.x*v.x+T.r.col2.x*v.y)+T.p.x;
    float y=(+T.r.col1.y*v.x+T.r.col2.y*v.y)+T.p.y;
    return vec2f(x,y);
  }
};
typedef transform2f b2Transform;
  
inline transform2f MakeZoomTransform(const vec2d&zoom)
{
  transform2f tmp(vec2f(0,0),QapMat22(vec2f(zoom.x,0.f),vec2f(0.f,zoom.y)));
  return tmp;
}
class vec3f{
public:
  float x,y,z;
  //vec3f(const D3DVECTOR&v):D3DVECTOR(v){}
  vec3f(){x=0;y=0;z=0;}
  vec3f(float x,float y,float z)
  {
    #define F(a)this->a=a;
    F(x);F(y);F(z);
    #undef F
  }
public:
  bool dist_to_point_less_that_r(const vec3f&p,real r)const{return (p-*this).SqrMag()<r*r;}
  bool dist_to_point_less_that_r(const vec3f&p,float r)const{return (p-*this).SqrMag()<r*r;}
  friend vec3f operator*(const float&u,const vec3f&v)
  {
    return vec3f(v.x*u,v.y*u,v.z*u);
  }
  friend vec3f operator*(const vec3f&v,const float&u)
  {
    return vec3f(v.x*u,v.y*u,v.z*u);
  }
  friend vec3f operator+(const vec3f&v,const vec3f&u)
  {
    return vec3f(v.x+u.x,v.y+u.y,v.z+u.z);
  }
  friend vec3f operator-(const vec3f&v,const vec3f&u)
  {
    return vec3f(v.x-u.x,v.y-u.y,v.z-u.z);
  }
  void operator*=(const float&k)
  {
    x*=k;
    y*=k;
    z*=k;
  }
  bool operator==(const vec3f&v)const
  {
    auto&a=*this;
    bool xok=a.x==v.x;
    bool yok=a.y==v.y;
    bool zok=a.z==v.z;
    return xok&&yok&&zok;
  }
  bool operator!=(const vec3f&v)const
  {
    return !operator==(v);
  }
  void operator+=(const vec3f&v)
  {
    x+=v.x;
    y+=v.y;
    z+=v.z;
  }
  void operator-=(const vec3f&v)
  {
    x-=v.x;
    y-=v.y;
    z-=v.z;
  }
  vec3f operator+()const{return *this;}
  vec3f operator-()const{return *this*-1;}
  vec3f RawMul(const vec3f&b)const
  {
    auto&a=*this;
    return vec3f(a.x*b.x,a.y*b.y,a.z*b.z);
  }
  vec3f RawMul(float x,float y,float z)const
  {
    auto&a=*this;
    return vec3f(a.x*x,a.y*y,a.z*z);
  }
  vec3f Mul(const vec3f&b)const
  {
    auto&a=*this;
    return vec3f(a.x*b.x,a.y*b.y,a.z*b.z);
  }
  vec3f Mul(float x,float y,float z)const
  {
    auto&a=*this;
    return vec3f(a.x*x,a.y*y,a.z*z);
  }
  float Mag()const
  {
    return sqrt(x*x+y*y+z*z);
  }
  float SqrMag()const
  {
    return x*x+y*y+z*z;
  }
  vec3f Norm()const
  {
    if((x==0)&&(y==0)&&(z==0))
    {
      return vec3f(0,0,0);
    }
    auto k=1.0f/Mag();
    return vec3f(x*k,y*k,z*k);
  }
  vec3f cross(const vec3f&b)const
  {
    auto&a=*this;
    return vec3f(
      +(a.y*b.z-a.z*b.y),
      -(a.x*b.z-a.z*b.x),
      +(a.x*b.y-a.y*b.x)
    );
  }
  float dot(const vec3f&b)const
  {
    auto&a=*this;
    return (
      a.x*b.x+
      a.y*b.y+
      a.z*b.z
    );
  }
};
inline float dot(const vec3f&a,const vec3f&b){return a.dot(b);}
//inline float dot(const vec3d&a,const vec3d&b){return a.dot(b);}
inline vec3f cross(const vec3f&a,const vec3f&b){return a.cross(b);}
//inline vec3d cross(const vec3d&a,const vec3d&b){return a.cross(b);}
class vec2i
{
public:
public:
  typedef vec2i SelfClass;
public:
  int x;
  int y;
public:
public:
  vec2i():x(0),y(0) {}
  vec2i(int x,int y):x(x),y(y) {};
  friend vec2i operator*(int u,const vec2i&v)
  {
    return vec2i(u*v.x,u*v.y);
  }
  friend vec2i operator*(const vec2i&v,int u)
  {
    return vec2i(u*v.x,u*v.y);
  }
  friend vec2i operator/(const vec2i&v,int d)
  {
    return vec2i(v.x/d,v.y/d);
  }
  friend vec2i operator+(const vec2i&u,const vec2i&v)
  {
    return vec2i(u.x+v.x,u.y+v.y);
  }
  friend vec2i operator-(const vec2i&u,const vec2i&v)
  {
    return vec2i(u.x-v.x,u.y-v.y);
  }
  void operator+=(const vec2i&v)
  {
    x+=v.x;
    y+=v.y;
  }
  void operator-=(const vec2i&v)
  {
    x-=v.x;
    y-=v.y;
  }
  int SqrMag()
  {
    return x*x+y*y;
  }
  float Mag()
  {
    return sqrt(float(x*x+y*y));
  }
  operator vec2d()const
  {
    return vec2d(x,y);
  }
  operator vec2f()const
  {
    return vec2f(x,y);
  }
  vec2i operator+()const
  {
    return vec2i(+x,+y);
  }
  vec2i operator-()const
  {
    return vec2i(-x,-y);
  }
  friend bool operator==(const vec2i&u,const vec2i&v)
  {
    return (u.x==v.x)&&(u.y==v.y);
  }
  friend bool operator!=(const vec2i&u,const vec2i&v)
  {
    return (u.x!=v.x)||(u.y!=v.y);
  }
  static vec2i fromVec2d(const vec2d&v){return vec2i(v.x,v.y);}
};
struct Dip2i
{
public:
  int a,b;
  Dip2i(int a,int b):a(a),b(b) {};
  void Take(int x)
  {
    a=min(a,x);
    b=max(b,x);
  }
  Dip2i Norm()const
  {
    return Dip2i(min(a,b),max(a,b));
  }
  int Mag()const
  {
    return b-a;
  }
public:
  struct Transform
  {
    float x,s;
    Transform(float x,float s):x(x),s(s) {}
    Transform(const Dip2i&from,const Dip2i&to)
    {
      s=float(to.Norm().Mag())/float(from.Norm().Mag());
      x=to.a-from.a;
    }
    float operator*(int v)
    {
      return x+v*s;
    }
  };
};
struct vec4f{
public:
  float b,g,r,a;
  //struct{float x,y,z,w;};
  vec4f(){}
  vec4f(float b,float g,float r,float a):b(b),g(g),r(r),a(a){}
  vec4f(const QapColor&ref):b(ref.b/255.f),g(ref.g/255.f),r(ref.r/255.f),a(ref.a/255.f){}
  vec4f&operator+=(const vec4f&v){b+=v.b;g+=v.g;r+=v.r;a+=v.a;return *this;}
  vec4f&operator*=(const float&k){b*=k;g*=k;r*=k;a*=k;return *this;}
  friend vec4f operator*(const float&u,const vec4f&v){return vec4f(u*v.b,u*v.g,u*v.r,u*v.a);}
  friend vec4f operator+(const vec4f&u,const vec4f&v){return vec4f(u.b+v.b,u.g+v.g,u.r+v.r,u.a+v.a);}
  #define F(r)Clamp(int(r*255),int(0),int(255))
  QapColor GetColor(){return QapColor(F(a),F(r),F(g),F(b));}
  #undef F
};
union vec4i{
public:
  struct{int x,y,z,w;};
  struct{int b,g,r,a;};
  vec4i(int b,int g,int r,int a):b(b),g(g),r(r),a(a){}
  vec4i(const QapColor&ref):b(ref.b),g(ref.g),r(ref.r),a(ref.a){}
  vec4i&operator+=(const vec4i&v){b+=v.b;g+=v.g;r+=v.r;a+=v.a;return *this;}
  vec4i operator*(const int&v){return vec4i(x*v,y*v,z*v,w*v);}
  vec4i operator/(const int&v){return vec4i(x/v,y/v,z/v,w/v);}
  vec4i operator+(const vec4i&v){return vec4i(x+v.x,y+v.y,z+v.z,w+v.w);}
  #define F(r)Clamp(int(r),int(0),int(255))
  QapColor GetColor(){return QapColor(F(a),F(r),F(g),F(b));}
  #undef F
};
inline bool CD_Rect2Point(vec2d A,vec2d B,vec2d P)
{
  vec2d &p=P;vec2d a(min(A.x,B.x),min(A.y,B.y)),b(max(A.x,B.x),max(A.y,B.y));
  return InDip(a.x,p.x,b.x)&&InDip(a.y,p.y,b.y);
}
void bindTex(/*QapDev&qDev,*/int Tex){
  EM_ASM({bindTex(qDev,$0);},Tex);
}
#ifdef _WIN32
#else
void RegTexMem(...){}
void UnRegTexMem(...){}
typedef unsigned int DWORD;
#endif
class QapTexMem
{
public:
  QapColor*pBits;
  int W,H;
  string Name;
  int ID;
public:
  QapTexMem(const string&name):pBits(NULL),W(0),H(0),Name(name){RegTexMem(this);};
  QapTexMem(const string&name,int w,int h,QapColor *pbits):pBits(pbits),W(w),H(h),Name(name){RegTexMem(this);};
  void SaveToFile(const string&FN);
  ~QapTexMem(){UnRegTexMem(this);delete[] pBits;};
  QapTexMem*Clone(){
    return new QapTexMem(Name+".Clone",W,H,(QapColor*)memcpy((void*)new QapColor[W*H],pBits,sizeof(QapColor)*W*H));
  }
  QapColor get_color_at(int x,int y)const{return pBits[x+y*W];}
public:
  class IPixelVisitor{
  public:
    virtual void Visit(const vec2i&p){}
  };
  class ITraceVisitor{
  public:
    virtual bool Visit(const vec2i&p){return true;}
    virtual bool NextWave(){return true;}
  };
  inline bool IsValid(const vec2i&p){return InDip(0,p.x,W-1)&&InDip(0,p.y,H-1);}
  inline QapColor&operator[](const vec2i&p){return pBits[p.x+p.y*W];}
  void Accept(const vec2i&p,IPixelVisitor&Visitor)
  {
    #define N(A,B)
    #define F(A,B){vec2i np=p+vec2i(A,B);if(IsValid(np))Visitor.Visit(np);};;;
    N(-1,-1)F(+0,-1)N(+1,-1)
    F(-1,+0)N(+0,+0)F(+1,+0)
    N(-1,+1)F(+0,+1)N(+1,+1)
    #undef F
    #undef N
  }
  QapTexMem*GenEdge(const vec2i&p,const QapColor&color=0xFF000000)
  {
    //15:27 06.07.2011
    QapTexMem*pDest=Clone()->Clear(0xffffffff);
    class TraceVisitor:public ITraceVisitor{
    public:
      QapTexMem*pSrc;
      QapTexMem*pDest;
      QapColor color;
      TraceVisitor(QapTexMem*pSrc,QapTexMem*pDest,const QapColor&color):pSrc(pSrc),pDest(pDest){}
    public:
      bool Visit(const vec2i&p){
        QapColor&S=pSrc->operator[](p);
        QapColor&D=pDest->operator[](p);
        //if(&D==&S)exit(0);
        bool flag=S==color;
        if(flag)D=0xffffffff;
        if(!flag)D=0xff000000;
        return flag;
      }
    }TV(this,pDest,color);
    Accept(p,TV);
    return pDest->InvertRGB();
  }
  void Accept(const vec2i&p,ITraceVisitor&Visitor)
  {  
    class Tracer{
    public:
      vector<vec2i>NArr,DArr,Arr,IA;
      vector<int>Check;
    public:
      ITraceVisitor&Visitor;
      QapTexMem*pMem;
      Tracer(ITraceVisitor&Visitor,QapTexMem*pMem,const vec2i&p):pMem(pMem),Visitor(Visitor){NArr.push_back(p);Init();}
      void Init()
      {
        Check.resize(pMem->W*pMem->H);
        for(int i=0;i<Check.size();i++)Check[i]=1;
      }
    public:
      class PixelVisitor:public IPixelVisitor{
      public:
        Tracer*pT;
        PixelVisitor(Tracer*pT):pT(pT){}
        ~PixelVisitor(){}
        void Visit(const vec2i&p){
          vector<vec2i>*Arr[2]={&pT->DArr,&pT->NArr};
          bool Accepted=!!pT->Visitor.Visit(p);
          Arr[Accepted]->push_back(p);
        }
      };
      int GetID(const vec2i&p){return p.x+p.y*pMem->W;}
      void Run()
      {
        while(!NArr.empty()&&Visitor.NextWave())
        {
          Arr=NArr;NArr.clear();DArr.clear();
          for(int j=0;j<Arr.size();j++)
          {
            vec2i&p=Arr[j];
            int ID=GetID(p);
            if(ID<0||!Check[ID])continue;
            PixelVisitor PV(this);
            pMem->Accept(p,PV);
            Check[ID]=false;
          }
        }
      }
    } tracer(Visitor,this,p);
    tracer.NArr.push_back(p);
    tracer.Run();  
  }
public:
  QapTexMem*FillBorder(int x,int y,QapTexMem*Source,int n=4)
  {
    if(!Source||!Source->pBits)return this;
    int sW=Source->W;
    int sH=Source->H;
    int mX=min(W,x+sW);
    int mY=min(H,y+sH);
    QapColor*pT=this->pBits;
    QapColor*pS=Source->pBits;
    #define F(i,j)pS[(i-x)+(j-y)*sW]
    for(int k=1;k<=n;k++)
    {
      {int j=00+y;if(InDip(0,j-k,H-1))for(int i=max(0,x);i<mX;i++){pT[i+(j-k)*W]=F(i,j);}}
      {int j=mY-1;if(InDip(0,j+k,H-1))for(int i=max(0,x);i<mX;i++){pT[i+(j+k)*W]=F(i,j);}}
      {int i=00+x;if(InDip(0,i-k,W-1))for(int j=max(0,y);j<mY;j++){pT[(i-k)+j*W]=F(i,j);}}
      {int i=mX-1;if(InDip(0,i+k,W-1))for(int j=max(0,y);j<mY;j++){pT[(i+k)+j*W]=F(i,j);}}
    }
    #undef F
    return this;
  }
  QapTexMem*FillMem(int x,int y,QapTexMem*Source)
  {
    if(!Source||!Source->pBits)return this;
    int sW=Source->W;
    int sH=Source->H;
    int mX=min(W,x+sW);
    int mY=min(H,y+sH);
    QapColor*pT=this->pBits;
    QapColor*pS=Source->pBits;
    for(int j=max(0,y);j<mY;j++){
      for(int i=max(0,x);i<mX;i++){
        pT[i+j*W]=pS[(i-x)+(j-y)*sW];
      }
    }
    return this;
  }
  QapTexMem*Clear(const QapColor&color=0xFF000000){
    for(int i=0;i<W*H;i++)pBits[i]=color;
     return this;
  }
  QapTexMem*FillLine(const int Line,const QapColor&Color){
    for(int i=0;i<W;i++){QapColor&pix=pBits[Line*W+i];pix=Color;}
    return this;
  }
  QapTexMem*Circle(const QapColor&color,int x,int y,int r){
    vec2i pos(x,y);
    int rr=r*r;
    int i0=max(0,x-r);
    int iz=min(W-1,x+r);
    int j0=max(0,y-r);
    int jz=min(H-1,y+r);
    for(int j=j0;j<=jz;j++)
      for(int i=i0;i<=iz;i++)
      {
        int mm=abs(r-(vec2i(i,j)-pos).SqrMag());
        if(mm<rr)
        {
          QapColor&FragColor=pBits[j*W+i];
          FragColor=color;
        }
      }
    return this;
  }
  QapTexMem*Draw(int x,int y,QapTexMem*Source,const QapColor&color){
    if(!Source||!Source->pBits)return this;
    int sW=Source->W;
    int sH=Source->H;
    int mX=min(W,x+sW);
    int mY=min(H,y+sH);
    QapColor*pT=this->pBits;
    QapColor*pS=Source->pBits;
    for(int j=max(0,y);j<mY;j++){
      for(int i=max(0,x);i<mX;i++){
        QapColor src=Source->pBits[(i-x)+(j-y)*sW]*color;
        QapColor&dest=pBits[j*W+i];
        dest=QapColor::Mix(dest,src,real(src.a)/255.0);
        //dest.a=;
      }
    }
    return this;
  }
  QapTexMem*Circle(const QapColor&color,int x,int y,int r,int hs){
    vec2i pos(x,y);
    int minr=r-hs;
    int maxr=r+hs;
    int r1=minr*minr;
    int r2=maxr*maxr;
    int i0=max(0,x-r-hs);
    int iz=min(W-1,x+r+hs);
    int j0=max(0,y-r-hs);
    int jz=min(H-1,y+r+hs);
    for(int j=j0;j<=jz;j++)
      for(int i=i0;i<=iz;i++)
      {
        int mm=abs(r-(vec2i(i,j)-pos).SqrMag());
        if(InDip(r1,mm,r2))
        {
          QapColor&FragColor=pBits[j*W+i];
          FragColor=color;
        }
      }
    return this;
  }
  QapTexMem*FillColomn(const int Colomn,const QapColor&Color){
    for(int i=0;i<H;i++)pBits[W*i+Colomn]=Color;
    return this;
  }
  QapTexMem*CalcAlphaToRGB_and_set_new_alpha(uchar new_alpha=uchar(0xff)){
    for(int i=0;i<W*H;i++){auto a=pBits[i].a;pBits[i]=QapColor(new_alpha,a,a,a);}
    return this;
  }
  QapTexMem*CalcAlphaToRGB_and_keep_alpha(){
    for(int i=0;i<W*H;i++){auto a=pBits[i].a;pBits[i]=QapColor(a,a,a,a);}
    return this;
  }
  QapTexMem*CalcAlpha(){
    for(int i=0;i<W*H;i++)pBits[i].a=pBits[i].GetLuminance();
    return this;
  }
  template<class FUNC>
  QapTexMem*ForEachPixel(FUNC&&func){
    for(int i=0;i<W*H;i++)func(pBits[i]);
    return this;
  }
  QapTexMem*CopyAlpha(QapTexMem*Alpha){
    if(Alpha->W!=W||Alpha->H!=H)return this;
    for(int i=0;i<W*H;i++)pBits[i].a=Alpha->pBits[i].a;
    return this;
  }
  QapTexMem*CalcAlpha(const QapColor&Color,int threshold=0){
    for(int i=0;i<W*H;i++)pBits[i].a=pBits[i]==Color?0:255;
    const QapColor&c=Color;
    int t=threshold;
    int t3=t*3;
    for(int i=0;i<W*H;i++){
      QapColor&p=pBits[i];
      #define F(r)int f##r=abs(int(p.r)-int(c.r));
      F(r);F(g);F(b);
      #undef F
      //int esqr=fr*fr+fg*fg+fb*fb;
      int sum=fr+fg+fb;
      p.a=sum>t3?255:t3?255*sum/t3:0;
    }
    return this;
  }
  QapTexMem*FillChannel(const QapColor&Source,DWORD BitMask){
    for(int i=0;i<W*H;i++){QapColor&C=pBits[i];C=(~BitMask&C)|(BitMask&Source);};
    return this;
  }
  QapTexMem*InvertRGB(){for(int i=0;i<W*H;i++){QapColor&C=pBits[i];C.r=~C.r;C.g=~C.g;C.b=~C.b;}return this;}
  Dip2i GetDipAlpha(){Dip2i Dip(256,-1);for(int i=0;i<W*H;i++)Dip.Take(pBits[i].a);return Dip;}
  Dip2i GetDipR(){Dip2i Dip(256,-1);for(int i=0;i<W*H;i++)Dip.Take(pBits[i].r);return Dip;}
  Dip2i GetDipG(){Dip2i Dip(256,-1);for(int i=0;i<W*H;i++)Dip.Take(pBits[i].g);return Dip;}
  Dip2i GetDipB(){Dip2i Dip(256,-1);for(int i=0;i<W*H;i++)Dip.Take(pBits[i].b);return Dip;}
  QapTexMem*NormRGB(){
    typedef Dip2i::Transform D2iXF;
    Dip2i Cur[4]={GetDipR(),GetDipG(),GetDipB(),GetDipAlpha()};
    Dip2i cur(256,-1);for(int i=0;i<4;i++){cur.Take(Cur[i].a);cur.Take(Cur[i].b);}
    Dip2i norm(0,255);
    D2iXF xf(cur,norm);
    D2iXF arr[4]={D2iXF(Cur[0],norm),D2iXF(Cur[1],norm),D2iXF(Cur[2],norm),D2iXF(Cur[3],norm)};
    for(int i=0;i<W*H;i++){
      QapColor&C=pBits[i];
      C.r=uchar(xf*C.r);
      C.g=uchar(xf*C.g);
      C.b=uchar(xf*C.b);
    }
    return this;
  }
  QapTexMem*NormAlpha(){
    Dip2i Cur=GetDipAlpha();
    Dip2i Norm(0,255);
    Dip2i::Transform xf(Cur,Norm);
    for(int i=0;i<W*H;i++)
    {
      QapColor&C=pBits[i];
      C.a=uchar(xf*C.a);
    }
    return this;
  }
  QapTexMem*InvertY(){
    QapColor*line=new QapColor[W];
    size_t Size=sizeof(QapColor)*W;
    for(int i=0;i<H/2;i++)
    {
      int a(W*(i)),b(W*(H-i-1));
      memcpy((void*)line,&pBits[a],Size);
      memcpy((void*)&pBits[a],&pBits[b],Size);
      memcpy((void*)&pBits[b],line,Size);
    }
    delete[] line;
    return this;
  }
  QapTexMem*InvertX(){
    for(int i=0;i<H;i++)
    {
      QapColor*line=&pBits[i];
      for(int j=0;j<W/2;j++)
      {
        int a(j),b(H-j-1);
        std::swap(line[a],line[b]);
      }
    }
    return this;
  }
  static float gauss(float x,float sigma)
  {
    float x_sqr=x*x;
    float sigma_sqr=sigma*sigma;
    float sqrt_value=1.0/sqrt(2.0*M_PI*sigma_sqr);
    float exp_value=exp(-x_sqr/(2.0*sigma_sqr));
    return sqrt_value*exp_value;
  }/*
  static void PascalRow(IntArray&arr,int n)
  {
    arr.resize(n+1);
    for(int i=0;i<arr.size();i++)arr[i]=0;
    int*c=&arr[0];
    c[0]=1;
    for(int j=1;j<n;j++)for(int i=j;i>=1;i--)c[i]=c[i-1]+c[i];
  }*/
  private:
    inline vec4f tex2Df(vec4f*C,const vec2i&uv){return C[Clamp(uv.x,int(0),int(W-1))+W*Clamp(uv.y,int(0),int(H-1))];}
    inline vec4f tex2D(QapColor*C,const vec2i&uv){return vec4f(C[Clamp(uv.x,int(0),int(W-1))+W*Clamp(uv.y,int(0),int(H-1))]);}
    inline vec4i tex2Di(QapColor*C,const vec2i&uv){return vec4i(C[Clamp(uv.x,int(0),int(W-1))+W*Clamp(uv.y,int(0),int(H-1))]);}
    void DirBlur(vec4f*SysMem,vec4f*SysOut,float radius,const vec2i&texrad){
      float sigma=radius/3.0;
      vector<float> garr;
      for(int x=0;x<=radius;x++){
        garr.push_back(gauss(float(x),sigma));
      }
      float*g=&garr[0];
      for(int j=0;j<H;j++)
        for(int i=0;i<W;i++)
        {
          vec4f&FragColor=SysOut[j*W+i];
          vec2i texcoord(i,j);
          float sum=0.0;
          int x=1;
          vec4f value(0,0,0,0);
          for(int x=-radius;x<+radius-1;x++)if(x)
          {
            float currentScale=g[abs(x)];
            sum+=currentScale;
            vec2i dudv=x*texrad;
            value+=currentScale*tex2Df(SysMem,texcoord+dudv);
          }
          value+=(1.f-sum)*tex2Df(SysMem,texcoord);
          FragColor=value;
        }
    }
  public:
  QapTexMem*Blur(const float&radius)
  {
    static vec4f SysMem[1024*1024*4];
    vec4f*tmp=&SysMem[W*H];
    for(int i=0;i<W*H;i++)SysMem[i]=vec4f(pBits[i].b,pBits[i].g,pBits[i].r,pBits[i].a);
    //memcpy_s(SysMem,sizeof(SysMem),pBits,W*H*sizeof(QapColor));//copy image
    DirBlur(SysMem,tmp,radius,vec2i(1,0));
    DirBlur(tmp,SysMem,radius,vec2i(0,1));
    for(int i=0;i<W*H;i++)pBits[i]=QapColor(SysMem[i].a,SysMem[i].r,SysMem[i].g,SysMem[i].b);
    return this;
  }
};
class QapTex{
public:
  #ifdef _WIN32
  IDirect3DTexture9*Tex;
  #else
  int Tex;
  #endif
  int W,H;
  string Name;
  int ID;
public:
  #ifdef _WIN32
  QapTex(QapTexMem*TexMem,IDirect3DTexture9*Tex):Tex(Tex){W=TexMem->W;H=TexMem->H;Name=TexMem->Name;RegTex(this);};
  ~QapTex(){
    UnRegTex(this);
    Tex->Release();
    Tex=0;
  };
  #else
  QapTex(QapTexMem*TexMem,int Tex):Tex(Tex){W=TexMem->W;H=TexMem->H;Name=TexMem->Name;/*RegTex(this);*/};
  ~QapTex(){
  };
  #endif
};
class QapDev{
public:
  QapColor color=0;
public:
  struct Ver
  {
    float x,y,z;
    QapColor color;
    float tu,tv;
    Ver():x(0),y(0),z(0),tu(0),tv(0) {}
    Ver(float x,float y,const QapColor&color,float u=0.0,float v=0.0):x(x),y(y),z(0),color(color),tu(u),tv(v) {}
    Ver(const vec2f&pos,const QapColor&color,float u=0.0,float v=0.0):x(pos.x),y(pos.y),z(0),color(color),tu(u),tv(v) {}
    Ver(const vec2f&pos,const QapColor&color,const vec2f&texcoord):x(pos.x),y(pos.y),z(0),color(color),tu(texcoord.x),tv(texcoord.y) {}
    vec3f&get_vec3f()const{
      return *(vec3f*)&x;
    }
    vec2f&get_pos()
    {
      return *(vec2f*)&x;
    }
    vec2f&get_pos()const
    {
      return *(vec2f*)&x;
    }
    vec2f&get_tpos()
    {
      return *(vec2f*)&tu;
    }
    vec2f&get_tpos()const
    {
      return *(vec2f*)&tu;
    }
  };
  struct BatchScope{
    QapDev&RD;
    bool flag=false;
    BatchScope(QapDev&RD):RD(RD){flag=!RD.Batching;if(flag)RD.BeginBatch();}
    ~BatchScope(){if(flag)RD.EndBatch();}
  };
public:
  //IDirect3DVertexBuffer9 *VB;
  //IDirect3DIndexBuffer9 *IB;
  vector<Ver> VB;
  vector<int> IB;
  Ver*VBA;
  int*IBA;
  int VPos;
  int IPos;
  int MaxVPos;
  int MaxIPos;
  int DIPs;
  int Verts;
  int Tris;
  //BlendType BlendMode;
  //AlphaMode AlphaMode;
  bool Batching;
  bool Textured;
  b2Transform xf,txf;
  //static const DWORD FVF=D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1;
public:
  QapDev():color(0xFFFFFFFF),VB(NULL),IB(NULL),VBA(NULL),IBA(NULL),VPos(0),IPos(0),MaxVPos(0),MaxIPos(0),Batching(false)/*,BlendMode(BT_SUB),AlphaMode(AM_NONE)*/{}
  ~QapDev(){Free();}
public:
  void ReInit(){Init(MaxVPos,MaxIPos);}
  void Init(int VCount,int ICount)
  {
    if(VB.size())return;
    MaxVPos=VCount; MaxIPos=ICount;
    //Sys.pDev->CreateVertexBuffer(VCount*sizeof(Ver),D3DUSAGE_DYNAMIC,FVF,D3DPOOL_DEFAULT,&VB,NULL);
    //Sys.pDev->CreateIndexBuffer(ICount*sizeof(int),D3DUSAGE_DYNAMIC,D3DFMT_INDEX32,D3DPOOL_DEFAULT,&IB,NULL);
    VB.resize(VCount);
    IB.resize(ICount);
    VBA=0; IBA=0; VPos=0; IPos=0; DIPs=0; Verts=0; Tris=0;
    xf.set_ident();
    txf.set_ident();
  }
  void Free(){
    VB={};IB={};VPos=0;IPos=0;Batching=false;//BlendMode=BT_SUB;AlphaMode=AM_NONE;
  };
public:
  void BeginBatch()
  {
    Batching=true;Textured=true;
    VBA=0;IBA=0;VPos=0;IPos=0;//Test it
    if(!IB.size()||!VB.size())return;
    IBA=IB.data();
    VBA=VB.data();
    //IB->Lock(0,sizeof(int)*MaxIPos,(void **)&IBA,0);
    //VB->Lock(0,sizeof(Ver)*MaxVPos,(void **)&VBA,0);
  };
  void EndBatch()
  {
    Batching=false;
    if(!IB.size()||!VB.size())return;
    //IB->Unlock(); VB->Unlock();  
    DIP();
    VBA=0;IBA=0;
  }
  void DIP()
  {
    //Sys.pDev->SetFVF(FVF);
    //Sys.pDev->SetStreamSource(0,VB,0,sizeof(Ver));
    //Sys.pDev->SetIndices(IB);
    //Sys.pDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,VPos,0,IPos/3);
    auto&qDev=*this;
    EM_ASM({
      g_VB=$0;g_VI=$1;g_VBN=$2;g_IBN=$3;
      g_draw2=()=>{
        qDev.parr.length=g_VBN*2;
        qDev.carr.length=g_VBN*4;
        qDev.tarr.length=g_VBN*2;
        let inv_255=1.0/255;
        for(let i=0;i<g_VBN;i++){
          // vec3f,uint,vec2f -> 32*3,32,32*2 -> 32*6
          qDev.parr[i*2+0]=HEAPF32[(g_VB>>2)+i*6+0];
          qDev.parr[i*2+1]=HEAPF32[(g_VB>>2)+i*6+1];
          //qDev.parr[i*3+2]=HEAPF32[(g_VB>>2)+i*6+2];
          qDev.carr[i*4+2]=HEAPU8[g_VB+i*6*4+3*4+0]*inv_255;
          qDev.carr[i*4+1]=HEAPU8[g_VB+i*6*4+3*4+1]*inv_255;
          qDev.carr[i*4+0]=HEAPU8[g_VB+i*6*4+3*4+2]*inv_255;
          qDev.carr[i*4+3]=HEAPU8[g_VB+i*6*4+3*4+3]*inv_255;
          qDev.tarr[i*2+0]=HEAPF32[(g_VB>>2)+i*6+3+1+0];
          qDev.tarr[i*2+1]=HEAPF32[(g_VB>>2)+i*6+3+1+1];
        }
        qDev.iarr.length=g_IBN;
        for(let i=0;i<g_IBN;i++){
          qDev.iarr[i]=HEAP32[(g_VI>>2)+i];
        }
        if(qDev.gl&&qDev.prog)qDev.DIP(qDev);
      };
      g_draw2();
    },int(qDev.VB.data()),int(qDev.IB.data()),qDev.VPos,qDev.IPos);
    DIPs++;Verts+=VPos;Tris+=IPos/3;
  }
  bool IsBatching(){return Batching;}
  int GetIPos(){return IPos;}
  int GetVPos(){return VPos;}
  int GetDIPs(){return DIPs;}
  int GetVerts(){return Verts;}
  int GetTris(){return Tris;}
  const QapColor&GetColor(){return color;}
  void NextFrame(){DIPs=0;Verts=0;Tris=0;/*SetBlendMode(BlendMode);SetAlphaMode(AlphaMode);*/}
public:
  void HackMode(bool Textured){this->Textured=Textured;}
  //virtual void BindTex(int Stage,QapTex*Tex){Sys.pDev->SetTexture(Stage,Tex?Tex->Tex:NULL);txf.set_ident();}
  void BindTex(int Stage,QapTex*pTex){bindTex(pTex?pTex->Tex:0);txf.set_ident();}
public:
  inline Ver&AddVertexRaw(){return VBA[VPos++];}
  inline int AddVertex(const Ver&Source)
  {
    Ver&Dest=VBA[VPos];
    Dest=Source;
    if(1){auto&v=Dest.get_pos();v=xf*v;}
    if(Textured){auto&v=Dest.get_tpos();v=txf*v;}
    return VPos++;
  }
  inline void AddTris(int A,int B,int C)
  {
    IBA[IPos++]=A;
    IBA[IPos++]=B;
    IBA[IPos++]=C;
  };
public:
  inline void SetColor(const QapColor&C){color=C;}
  inline void SetTransform(b2Transform const&val){xf=val;}
  inline b2Transform GetTransform(){return xf;}
  inline void SetTextureTransform(b2Transform const&val){txf=val;}
  inline b2Transform GetTextureTransform(){return txf;}
  inline real GetZoom(){return vec2d(xf.r.col1).Mag();}
public:
  //BlendType GetBlendMode(){return BlendMode;}
  //AlphaMode GetAlphaMode(){return AlphaMode;}
  //void SetBlendMode(BlendType Mode){Blend(BlendMode=Mode);}
  //void SetAlphaMode(AlphaMode Mode){Alpha(AlphaMode=Mode);}
public:
  inline int AddVertex(float x,float y,const QapColor&c,float u,float v)
  {
    QapDev::Ver tmp;
    tmp.x=x;
    tmp.y=y;
    tmp.z=0;
    tmp.color=c;
    tmp.tu=u;
    tmp.tv=v;
    auto id=AddVertex(tmp);
    return id;
  }
  inline int AddVertex(const vec2f&pos,const QapColor&c,float u,float v)
  {
    QapDev::Ver tmp;
    tmp.get_pos()=pos;
    tmp.z=0;
    tmp.color=c;
    tmp.tu=u;
    tmp.tv=v;
    auto id=AddVertex(tmp);
    return id;
  }
  inline int AddVertex(const vec2f&pos,const QapColor&c,const vec2f&tpos)
  {
    QapDev::Ver tmp;
    tmp.get_pos()=pos;
    tmp.z=0;
    tmp.color=c;
    tmp.get_tpos()=tpos;
    auto id=AddVertex(tmp);
    return id;
  }
public:
  void DrawCircleOld(const vec2d&pos,real r,real ang,real ls,int seg)
  {
    static vector<vec2d> PA;
    PA.resize(seg);
    for(int i=0;i<seg;i++)
    {
      vec2d v=Vec2dEx((real)i/(real)seg*2.0*Pi,r);
      PA[i]=pos+v;
    };
    DrawPolyLine(PA,ls,true);
  }
  void DrawCircle(const vec2d&pos,real r,real ang,real ls,int seg)
  {
    DrawCircleEx(pos,r-ls*0.5,r+ls*0.5,seg,ang);
  }
  void DrawCircleEx(const vec2d&pos,real r0,real r1,int seg,real ang)
  {
    int n=seg;
    if(n<=0)return;
    BatchScope Scope(*this);
    static vector<int> VID;
    VID.resize(n*2);
    for(int i=0;i<n;i++)
    {
      vec2d v=Vec2dEx(ang+Pi2*(real(i)/real(n)),1);
      VID[0+i]=AddVertex(pos+v*r0,color,0,0);
      VID[n+i]=AddVertex(pos+v*r1,color,0,0);
    }
    for(int i=0;i<n;i++)
    {
      int a=VID[0+(i+0)%n];
      int b=VID[0+(i+1)%n];
      int c=VID[n+(i+0)%n];
      int d=VID[n+(i+1)%n];
      AddTris(a,b,d);
      AddTris(d,c,a);
    }
  }
public:
  void DrawQuad(float x,float y,float w, float h)
  {  
    BatchScope Scope(*this);
    {
      #define F(X,Y,U,V){AddVertex(Ver(x+(X)*w,y+(Y)*h,color,U,V));}
      vec2d O(x,y);
      int n=VPos;
      F(-0.5f,-0.5f,0.0f,1.0f);
      F(+0.5f,-0.5f,1.0f,1.0f);
      F(+0.5f,+0.5f,1.0f,0.0f);
      F(-0.5f,+0.5f,0.0f,0.0f); 
      AddTris(n+1,n+0,n+3);
      AddTris(n+3,n+2,n+1);
      #undef F
    }
  }
  void DrawQuad(float x,float y,float w, float h, float a)
  {
    BatchScope Scope(*this);
    {
      #define F(X,Y,U,V){AddVertex(Ver(O+vec2d(X*w,Y*h).UnRot(OZ),color,U,V));}
      vec2d OZ=Vec2dEx(a,1.0);
      vec2d O(x,y);
      int n=VPos;
      F(-0.5f,-0.5f,0.0f,1.0f);
      F(+0.5f,-0.5f,1.0f,1.0f);
      F(+0.5f,+0.5f,1.0f,0.0f);
      F(-0.5f,+0.5f,0.0f,0.0f); 
      AddTris(n+1,n+0,n+3);
      AddTris(n+3,n+2,n+1);
      #undef F
    }
  }
  void DrawTrigon(const vec2d&A,const vec2d&B,const vec2d&C)
  {
    BatchScope Scope(*this);
    {
      AddTris(
        AddVertex(Ver(A,color)),
        AddVertex(Ver(B,color)),
        AddVertex(Ver(C,color))
      );
    }
  }
  void DrawConvex(const vector<vec2d>&Points)
  {
    BatchScope Scope(*this);
    {
      if(Points.empty())return;
      int c=Points.size();
      int n=VPos;
      for(int i=0;i<c;i++)AddVertex(Ver(Points[i],color,0.5,0.5));
      for(int i=2;i<c;i++)AddTris(n,n+i-1,n+i-0);
    }
  }
public:
  template<typename TYPE>
  void DrawPolyLine(const vector<TYPE>&PA,const real&LineSize,const bool&Loop)
  {
    if(PA.empty())return;
    BatchScope Scope(*this);
    {
      int Count=PA.size();
      int c=Loop?Count:Count-1;
      for(int i=0;i<c;i++)
      {
        TYPE const&a=PA[(i+0)%Count];
        TYPE const&b=PA[(i+1)%Count];
        TYPE n=vec2d(b-a).Ort().SetMag(LineSize);
        int A[4]={
        #define F(pos)AddVertex(Ver(pos,color,0.5f,0.5f))
          F(a+n),
          F(b-n),
          F(a-n),
          F(b+n),
        #undef F
        };
        AddTris(A[0],A[1],A[2]);
        AddTris(A[0],A[1],A[3]);
      }
    }
  }
  template<typename TYPE>
  void DrawLineList(const vector<TYPE>&PA,const real&LineSize)
  {
    if(PA.empty())return;
    BatchScope Scope(*this);
    {
      int Count=PA.size();
      for(int i=0;i<Count;i+=2)
      {
        TYPE const&a=PA[i+0];
        TYPE const&b=PA[i+1];
        TYPE n=vec2d(b-a).Ort().SetMag(LineSize);
        int A[4]={
        #define F(A)AddVertex(Ver(A,color,0.5,0.5))
          F(a+n),
          F(b-n),
          F(a-n),
          F(b+n),
        #undef F
        };
        AddTris(A[0],A[1],A[2]);
        AddTris(A[0],A[1],A[3]);
      }
    }
  }
  /*template<typename TYPE>
  void DrawMesh(const vector<TYPE>&VA,const vector<int>&IA)
  {
    if(VA.empty())return;
    BatchScope Scope(*this);
    {
      int base=GetVPos();
      static vector<int> VID;VID.resize(VA.size());
      for(int i=0;i<VA.size();i++)VID[i]=AddVertex(MakeVer(VA[i],color,p.x,p.y));
      for(int i=0;i<IA.size();i+=3)AddTris(VID[IA[i+0]],VID[IA[i+1]],VID[IA[i+2]]);
    }
  }*/
};
#ifdef _WIN32
class IResource
{
public:
  virtual void OnLost()=0;
  virtual void OnReset()=0;
  static IResource* Reg(IResource *A);
  static IResource* UnReg(IResource *A);
};
class QapBitmapInfo{
public:
  BITMAPINFO BI;
  BITMAPINFOHEADER&BH;
public:
  QapBitmapInfo(int W,int H):BH(BI.bmiHeader){
    ZeroMemory(&BI,sizeof(BI));
    BH.biSize=sizeof(BI.bmiHeader);
    BH.biWidth=W;BH.biHeight=H;
    BH.biPlanes=1;BH.biBitCount=32;
    BH.biSizeImage=W*H*4;
  }
};
#else
#endif
struct QapFont
{
  QapTex*Tex;
  int W[256],H[256],Size;
  QapFont():Tex(NULL),Size(0){};
  QapFont(QapTex*Tex,int Size):Tex(Tex),Size(Size){};
  ~QapFont(){
    if(Tex)delete Tex;
  };
  void Transmit(QapFont&ref)
  {
    if(Tex)delete Tex;
    Tex=ref.Tex;ref.Tex=NULL;
    for(int i=0;i<256;i++)W[i]=ref.W[i];
    for(int i=0;i<256;i++)H[i]=ref.H[i];
    Size=ref.Size;
  }
  QapFont(QapFont&ref):Tex(NULL),Size(0){Transmit(ref);}
  QapFont&operator=(QapFont&ref){Transmit(ref);return *this;}
  #ifdef _WIN32
  QapTexMem*CreateFontMem(string Name,int Size,bool Bold,int TexSize)
  {
    QapColor*pix=new QapColor[TexSize*TexSize];
    this->Size=TexSize;
    {
      HDC DC=GetDC(Sys.hWnd);
      int W=Bold?FW_BOLD:FW_NORMAL;
      int H=-MulDiv(Size,GetDeviceCaps(DC,LOGPIXELSY),72);
      {
        HFONT FNT=CreateFontA(H,0,0,0,W,0,0,0,RUSSIAN_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,DEFAULT_PITCH,Name.c_str());
        {
          HDC MDC=CreateCompatibleDC(DC);
          {
            HBITMAP BMP=CreateCompatibleBitmap(DC,TexSize,TexSize);
            SelectObject(MDC,BMP);
            RECT Rect; SetRect(&Rect,0,0,TexSize,TexSize);
            FillRect(MDC,&Rect,(HBRUSH)GetStockObject(BLACK_BRUSH));
            SelectObject(MDC,FNT);
            SetBkMode(MDC,TRANSPARENT);
            SetTextColor(MDC,0xFFFFFF);
            for(int i=0;i<256;i++)TextOutA(MDC,i%16*(TexSize/16),i/16*(TexSize/16),(LPCSTR)&i,1);
            {QapBitmapInfo QBI(TexSize,TexSize);GetDIBits(MDC,BMP,0,TexSize,pix,&QBI.BI,DIB_RGB_COLORS);}
            for(int i=0;i<TexSize*TexSize;i++){pix[i].a=pix[i].r;pix[i].r=255;pix[i].g=255;pix[i].b=255;}
            for(int i=0;i<256;i++){SIZE cs;GetTextExtentPoint32A(MDC,(LPCSTR)&i,1,&cs);this->W[i]=cs.cx;this->H[i]=cs.cy;}
            DeleteObject(BMP);
          }
          DeleteDC(MDC);
        }
        DeleteObject(FNT); 
      }
      ReleaseDC(Sys.hWnd,DC);
    }
    QapTexMem*pMem=new QapTexMem("Font_"+Name+"_"+to_string(TexSize),TexSize,TexSize,(QapColor*)pix);
    return pMem;
  }
  #else
  QapTexMem*CreateFontMem(string Name,int Size,bool Bold,int TexSize)
  {
    QapColor*pix=new QapColor[TexSize*TexSize];
    this->Size=TexSize;
    emscripten_run_script(string("g_font=initFont("+to_string(Size)+",'"+Name+"',"+to_string(TexSize)+","+to_string(int(Bold))+");").c_str());
    EM_ASM({
      initFont_v2(g_font,$0,$1,$2);
    },int(pix),int(&W[0]),int(&H[0]));
    QapTexMem*pMem=new QapTexMem("Font_"+Name+"_"+to_string(TexSize),TexSize,TexSize,(QapColor*)pix);
    pMem->InvertY();
    return pMem;
  }
  #endif
};
#ifdef _WIN32
QapTex*GenTextureMipMap(QapTexMem*&Tex,int MaxLevelCount=16)//only D3DFMT_A8R8G8B8
{
  if(!Tex)return NULL;
  int &W=Tex->W;int &H=Tex->H;QapColor *&pBits=Tex->pBits;
  if(0){fail:MACRO_ADD_LOG("NPOT texture \""+Tex->Name+"\"("+IToS(W)+"x"+IToS(H)+")",lml_WARNING);return NULL;}
  int SW=1,SWC=0;for(int &i=SWC;SW<W;i++)SW*=2; if(SW>W)goto fail;
  int SH=1,SHC=0;for(int &i=SHC;SH<H;i++)SH*=2; if(SH>H)goto fail;
  int Levels=min(MaxLevelCount,min(SWC,SHC));
  IDirect3DTexture9 *tex;
  Sys.pDev->CreateTexture(SW,SH,Levels,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&tex,NULL);
  D3DLOCKED_RECT rect[16]; struct QapARGB{uchar B,G,R,A;}; QapARGB* pBitsEx[16];
  for(int i=0;i<Levels;i++){tex->LockRect(i,&rect[i],NULL,0);pBitsEx[i]=(QapARGB*)rect[i].pBits;};
  {
    QapColor* pDestBits=(QapColor*)rect[0].pBits;
    memcpy_s(pDestBits,SW*SH*sizeof(QapColor),pBits,SW*SH*sizeof(QapColor));
    SW/=2; SH/=2;
  }
  for(int k=1;k<Levels;k++)
  {
    QapARGB *PC=(QapARGB*)rect[k].pBits;
    for(int j=0;j<SH;j++)
      for(int i=0;i<SW;i++)
      {
        #define F(X,Y)pBitsEx[k-1][X+2*Y*SW+2*i+4*j*SW]
          QapARGB A[4]={F(0,0),F(0,1),F(1,0),F(1,1)};
        #undef F
        float AF[4]={0,0,0,0};
        for(int t=0;t<4;t++)
        {
          AF[0]+=A[t].R;
          AF[1]+=A[t].G;
          AF[2]+=A[t].B;
          AF[3]+=A[t].A;
        };
        for(int t=0;t<4;t++)AF[t]*=0.25*(1.0/255.0);
        QapColor PCC=D3DCOLOR_COLORVALUE(AF[0],AF[1],AF[2],AF[3]);
        *PC=*((QapARGB*)&PCC);
        PC++;
      }
    SW/=2; SH/=2;
  }
  for(int i=0;i<Levels;i++)tex->UnlockRect(i);
  //MACRO_ADD_LOG("Loaded \""+Tex->Name+"\"",lml_HINT);
  QapTex*pTex=new QapTex(Tex,tex);
  delete Tex;Tex=NULL;
  return pTex;
}
#else
QapTex*GenTextureMipMap(QapTexMem*&pMem,int MaxLevelCount=16){
  int&W=pMem->W;int&H=pMem->H;QapColor*&pBits=pMem->pBits;
  auto tex=EM_ASM_INT({
    return makeTexture(qDev,$0,$1,$2);
  },W,H,int(pBits));
  auto*pTex=new QapTex(pMem,tex);
  delete pMem;pMem=NULL;
  return pTex;
}
#endif
QapTexMem*BlurTexture(QapTexMem*Tex,int PassCount)//only D3DFMT_A8R8G8B8
{
  int &W=Tex->W;int &H=Tex->H;QapColor *&pBits=Tex->pBits;
  #define BlurLog(X,Y)//MACRO_ADD_LOG(X,Y)
  BlurLog("Blur \""+Tex->Name+"\" x"+IToS(PassCount),lml_HINT);
  struct QapARGB{uchar B,G,R,A;};
  static QapARGB VoidMem[2048*2048*4];
  #define qap_memcpy_s(DEST,DSIZE,SRC,SIZE)memcpy(DEST,SRC,SIZE)
  qap_memcpy_s(VoidMem,sizeof(VoidMem),pBits,W*H*sizeof(QapARGB));
  static int BBM[9]={
    1,2,1, 
    2,4,2, 
    1,2,1};
  #define INIT_STATIC_VAR(TYPE,VALUE,INIT_CODE)static TYPE VALUE;{static bool _STATIC_SYS_FLAG=true;if(_STATIC_SYS_FLAG){INIT_CODE;_STATIC_SYS_FLAG=false;};};
    INIT_STATIC_VAR(int,MartixSum,for(int i=0;i<9;i++)MartixSum+=BBM[i];);
  #undef INIT_STATIC_VAR
  int PosRange[9]={
    -W-1,-W,-W+1,
    -1,0,+1,
    +W-1,+W,+W+1};
  float inv_255=1.0/255;
  for(int PassId=0;PassId<PassCount;PassId++)
  {
    QapARGB *PC=0;
    QapARGB *VM=0;
    for(int j=1;j<H-1;j++)
      for(int i=1;i<W-1;i++)
      {
        PC=(QapARGB*)pBits+j*W+i; VM=(QapARGB*)VoidMem+j*W+i;
        float AF[4]={0,0,0,0};
        for(int t=0;t<9;t++)
        {
          QapARGB T=*(VM+PosRange[t]);
          AF[0]+=T.R*BBM[t];
          AF[1]+=T.G*BBM[t];
          AF[2]+=T.B*BBM[t];
          AF[3]+=T.A*BBM[t];
        };    
        for(int i=0;i<4;i++)AF[i]/=MartixSum*255.0;
        #define F(r,g,b,a)QapColor((DWORD)((r)*255.f),(DWORD)((g)*255.f),(DWORD)((b)*255.f),(DWORD)((a)*255.f))
        QapColor PCC=F(AF[3],AF[0],AF[1],AF[2]);
        #undef F
        *PC=*((QapARGB*)&PCC);
      }
    //PassId++;
    qap_memcpy_s(VoidMem,sizeof(VoidMem),pBits,W*H*sizeof(QapARGB));
    #undef qap_memcpy_s
  }
  BlurLog("Blur \""+Tex->Name+"\" x"+IToS(PassCount),lml_HINT);
  #undef BlurLog
  return Tex;
}
void DrawQapText(QapDev*RD,QapFont&Font,float X,float Y,const string&Text)
{
  static QapColor CT[]={
    0xFF252525,0xFFFF0000,0xFF00FF00,0xFFFFFF00,
    0xFF0000FF,0xFFFF00FF,0xFF00FFFF,0xFFFFFFFF,
    0xFFFFFFA8,0xFFFFA8FF,
    0xFFFF8000,0xFF0080FF,0xFFA0A0A0,0xFF808080,0xFFF0F000,0xFF00F0F0,
  };
  bool _4it=!RD->IsBatching();
  if(_4it)RD->BeginBatch();
  int QuadCount=0;
  int VPos=RD->GetVPos();
  {
    float xp=0; int i=0;
    while(i<(int)Text.length())
    {
      if(Text[i]!='^')
      {
        int I=(uchar)Text[i];
        float s=((float)(I%16))/16,t=((float)(I/16))/16;
        float cx=(float)Font.W[I],cy=(float)Font.H[I],ts=(float)Font.Size;
        #define F(var,x,y,z,color,u,v)int var=RD->AddVertex(QapDev::Ver(X+x,Y+y,color,u,v));
          F(A,xp+0,-cy,0,RD->GetColor(),s,1-t-cy/ts);
          F(B,xp+cx,-cy,0,RD->GetColor(),s+cx/ts,1-t-cy/ts);
          F(C,xp+cx,0,0,RD->GetColor(),s+cx/ts,1-t);
          F(D,xp+0,0,0,RD->GetColor(),s,1-t);
        #undef F
        RD->AddTris(A,B,C);
        RD->AddTris(C,D,A);
        xp+=cx; QuadCount++; i++; continue;
      };
      i++; if(i>(int)Text.length())continue;
      if((Text[i]>='0')&&(Text[i]<='9')){RD->SetColor(CT[Text[i]-'0']); i++; continue;};
      if((Text[i]>='A')&&(Text[i]<='F')){RD->SetColor(CT[Text[i]-'A'+10]); i++; continue;};
    }
  };
  if(_4it)RD->EndBatch();
}
string Q3TextToNormal(const string&Text)
{
  string s; int i=0;
  while(i<(int)Text.length())
  {
    if(Text[i]!='^'){s.push_back(Text[i++]);continue;}
    i++;if(i>(int)Text.length())continue;
    if((Text[i]>='0')&&(Text[i]<='9')){i++;continue;};
    if((Text[i]>='A')&&(Text[i]<='F')){i++;continue;};
  }
  return s;
}
int GetQ3TextLength(const QapFont &Font,const string &Text)
{
  float xp=0; int i=0;
  while(i<(int)Text.size())
  {
    if(Text[i]!='^')
    {
      int I=(uchar)Text[i];
      float cx=(float)Font.W[I];
      xp+=cx; i++; continue;
    };
    i++; if(i>(int)Text.size())continue;
    if((Text[i]>='0')&&(Text[i]<='9')){i++; continue;};
    if((Text[i]>='A')&&(Text[i]<='F')){i++; continue;};
  }
  return xp;
}
class TextRender{
public:
  QapDev*RD;
  struct TextLine{
  public:
    string text;
    int x,y;
  public:
    TextLine(int x,int y,const string&text):x(x),y(y),text(text){}
  public:
    void DrawRaw(QapDev*RD,QapFont*Font,int dv){DrawQapText(RD,*Font,x+dv+0.5,y-dv+0.5,Q3TextToNormal(text));}
    void DrawSys(QapDev*RD,QapFont*Font,int dv){DrawQapText(RD,*Font,x+dv+0.5,y-dv+0.5,text);}
  };
  vector<TextLine> LV;
  TextRender(QapDev*RD):RD(RD){}
public:
  int x,y,ident,bx;
  QapFont*NormFont;
  QapFont*BlurFont;
public:
  void BeginScope(int X,int Y,QapFont*NormFont,QapFont*BlurFont){
    bx=X;x=X;y=Y;ident=24;this->NormFont=NormFont;this->BlurFont=BlurFont;
  }
  void BR(){y-=ident;x=bx;}
  void AddText(const string&text)
  {
    LV.push_back(TextLine(x,y,text));BR();
  }
  int text_len(const string&text){return GetQ3TextLength(*NormFont,text);}
  void AddTextNext(const string&text)
  {
    LV.push_back(TextLine(x,y,text));x+=GetQ3TextLength(*NormFont,text);
  }
  void EndScope(){
    //RD->SetBlendMode(BT_SUB);
    //RD->SetAlphaMode(AM_NONE);
    {
      RD->BindTex(0,BlurFont->Tex);
      RD->SetColor(0xff000000);
      RD->BeginBatch();
      for(int i=0;i<LV.size();i++)LV[i].DrawRaw(RD,BlurFont,1.0);
      RD->EndBatch();
    }
    {
      RD->BindTex(0,NormFont->Tex);
      RD->SetColor(0xffffffff);
      RD->BeginBatch();
      for(int i=0;i<LV.size();i++)LV[i].DrawSys(RD,NormFont,0.0);
      RD->EndBatch();
    }
    //RD->SetAlphaMode(AM_NONE);
  }
};
struct t_rec{
  vec2d pos;
  double wh;
  QapColor c;
  double ang;
  double dang;
};
vector<t_rec> rarr;
QapDev qDev;
QapFont NormFont,BlurFont;
//QapTex*NormFontTex=nullptr;
//QapTex*BlurFontTex=nullptr;
class QapAtlas{
public:
  int W,H;
  int X,Y;
  int Ident,dY;
  QapTexMem*pMem{};
  QapTex*pTex{};
  struct TFrame{
    QapAtlas*atlas;
    int x,y,w,h;
    TFrame():atlas(NULL),x(0),y(0),w(0),h(0){}
    TFrame(QapAtlas*atlas,QapTexMem*Mem,int x,int y):atlas(atlas),x(x),y(y),w(Mem->W),h(Mem->H){}
    void Bind(QapDev*RD){atlas->Bind(RD,this);}
  };
  QapPool<TFrame>pool;
  vector<TFrame*>frames;
public:
  QapAtlas():pMem(NULL),pTex(NULL),W(1024*2),H(1024*2),X(0),Y(0),Ident(8),dY(0),pool(256){
    pMem=new QapTexMem("Atlas.qap",W,H,new QapColor[W*H]);
  }
  TFrame*AddFrame(QapTexMem*Mem)
  {
    if(!Mem||!Mem->pBits)return NULL;
    if(X<Ident||Ident+X+Mem->W>W)
    {
      QapAssert(W>=Mem->W);
      X=Ident;Y+=dY+Ident;dY=Mem->H;
      pMem->FillLine(Y-Ident/2,0xff000000);
    }
    {
      TFrame*pFrame;
      pool.NewInstance(pFrame);
      *pFrame=TFrame(this,Mem,X,Y);
      frames.push_back(pFrame);
      pMem->FillBorder(X,Y,Mem);
      pMem->FillMem(X,Y,Mem);
      X+=Mem->W+Ident;dY=max(dY,Mem->H);
      QAP_EM_LOG("AddFrame done");
      return pFrame;
    }
  }
  QapTex*AddTex(QapTexMem*Mem)
  {
    AddFrame(Mem);
    return GenTextureMipMap(Mem);
  }
  QapTex*GenTex(){return pTex=GenTextureMipMap(pMem);}
  void Bind(QapDev*RD,TFrame*frame)
  {
    b2Transform xf;
    float inv_w=1.f/float(W);
    float inv_h=1.f/float(H);
    xf.p=vec2f(float(frame->x)*inv_w,float(frame->y)*inv_h);
    xf.r=MakeZoomTransform(vec2d(float(frame->w)*inv_w,float(frame->h)*inv_h)).r;
    RD->SetTextureTransform(xf);
  }
};
enum TMouseButton
{
  mbLeft=257,
  mbRight=258,
  mbMiddle=259,
};
class QapKeyboard
{
public:
  struct TKeyState
  {
    static const int MAX_KEY=263;
    bool data[MAX_KEY];
    TKeyState()
    {
      SetToDef();
    }
    void SetToDef()
    {
      for(int i=0;i<MAX_KEY;i++)data[i]=false;
    }
    bool&operator[](int index)
    {
      if(!InDip<int>(0,index,MAX_KEY-1))
      {
        QapDebugMsg("bad index");
        static bool tmp;
        return tmp;
      }
      return data[index];
    }
    bool&operator()(int index)
    {
      if(!InDip<int>(0,index,MAX_KEY-1))
      {
        QapDebugMsg("bad index");
        static bool tmp;
        return tmp;
      }
      return data[index];
    }
    const bool&operator()(int index)const
    {
      if(!InDip<int>(0,index,MAX_KEY-1))
      {
        QapDebugMsg("bad index");
        static bool tmp;
        return tmp;
      }
      return data[index];
    }
    const bool&operator[](int index)const
    {
      if(!InDip<int>(0,index,MAX_KEY-1))
      {
        QapDebugMsg("bad index");
        static bool tmp;
        return tmp;
      }
      return data[index];
    }
  };
public:
public:
  typedef QapKeyboard SelfClass;
public:
  int LastKey=0;
  char LastChar=0;
  bool News=false;
  TKeyState Down;
  TKeyState Changed;
  vec2d MousePos;
public:
  void KeyUpdate(int Key,bool Value)
  {
    if(Value)LastKey=Key;
    Down[Key]=Value;
    Changed[Key]=true;
  }
  void CharUpdate(char c)
  {
    LastChar=c;
    News=true;
  }
  void Sync()
  {
    this->News=false;
    Changed.SetToDef();
  }
  bool OnDown(int index)const
  {
    return Changed[index]&&Down[index];
  }
  bool OnUp(int index)const
  {
    return Changed[index]&&!Down[index];
  }
};
QapKeyboard kb;
struct t_global_img{
  string fn;
  std::function<void(const string&,int,int,int)> on_load;
  bool done=false;
};
struct t_global_url{
  string url;
  std::function<void(const string&,int,int)> on_load;
  bool done=false;
};
map<string,t_global_img> g_global_imgs;
map<string,t_global_url> g_global_urls;
extern "C" {
  int qap_on_load_img(char*pfn,int ptr,int w,int h){
    string fn=pfn;
    QAP_EM_LOG("on_load:"+fn);
    auto it=g_global_imgs.find(fn);
    if(it==g_global_imgs.end())return 0;
    QAP_EM_LOG("on_load_bef:"+fn);
    it->second.on_load(fn,ptr,w,h);
    QAP_EM_LOG("on_load_aft:"+fn);
    it->second.done=true;
    return 0;
  }
  int qap_on_load_url(char*purl,int ptr,int size){
    string url=purl;
    QAP_EM_LOG("on_load_url:"+url);
    auto it=g_global_urls.find(url);
    if(it==g_global_urls.end())return 0;
    QAP_EM_LOG("on_load_url_bef:"+url);
    it->second.on_load(url,ptr,size);
    QAP_EM_LOG("on_load_url_aft:"+url);
    it->second.done=true;
    return 0;
  }
}
template<class FUNC>
QapTexMem*LoadTexture(string fn,FUNC&&func){
  auto&m=g_global_imgs[fn];
  m.fn=fn;
  m.on_load=std::move(func);
  EM_ASM({
    loadTexture_v2(UTF8ToString($0));
  },int(fn.c_str()));
  return nullptr;
}
string g_host="";
class TGame{
public:
  typedef QapAtlas::TFrame TFrame;
public:
  class ILevel{
  public:
    virtual void Render(QapDev*RD)=0;
    virtual void Update(TGame*Game)=0;
    virtual bool Win()=0;
    virtual bool Fail()=0;
    virtual void AddText(TextRender*TR){}
    virtual ~ILevel(){}
  };
  class ILevelFactory{
  public:
    virtual ILevel*Build(TGame*Game)=0;
  };
  struct TLevelInfo{
    string Name;
    ILevelFactory&Factory;
    TLevelInfo(const string&Name,ILevelFactory&Factory):Name(Name),Factory(Factory){}
  };
  template<typename TYPE>
  class TLevelFactory:public ILevelFactory{
  public:
    virtual ILevel*Build(TGame*Game){
      auto*Result=new TYPE();
      *Result={};
      Result->Init(Game);
      return Result;
    }
  };/*
  template<typename TYPE>
  class AutoPtr:public std::auto_ptr<TYPE>{
  public:
    TYPE*operator->(){return get();}
    operator bool(){return 
  };*/
public:
  class TCounterInc{
  public:
    int Value=0;
    int Minimum=0;
    int Maximum=32;
  public:
    TCounterInc(){;}
    TCounterInc(int Value,int Minimum,int Maximum){this->Value=Value;this->Minimum=Minimum;this->Maximum=Maximum;}
    void Start(){Value=Minimum;}
    void Stop(){Value=Maximum;}
    operator bool(){return Value<Maximum;}
    void operator++(){Value++;}
    void operator++(int){Value++;}
    int DipSize(){return Maximum-Minimum;}
    operator string(){return IToS(Value)+"/"+IToS(DipSize());}
  };  
  class TCounterIncEx{
  public:
    int Value=0;
    int Minimum=0;
    int Maximum=32;
    bool Runned=false;
  public:
    TCounterIncEx(){;}
    TCounterIncEx(int Value,int Minimum,int Maximum){;this->Value=Value;this->Minimum=Minimum;this->Maximum=Maximum;}
    void Start(){Value=Minimum;Runned=true;}
    void Stop(){Value=Minimum;Runned=false;}
    operator bool(){return Value<Maximum;}
    void operator++(){if(Runned)Value++;}
    void operator++(int){if(Runned)Value++;}
    int DipSize(){return Maximum-Minimum;}
    operator string(){return IToS(Value)+"/"+IToS(DipSize());}
  };
class Level_MarketGame:public TGame::ILevel{
public:
  struct t_item{
    int id=0;
    int amount=0;
    int price=0;
  };
  struct t_market{
    vec2d pos;
    vector<t_item> items;
  };
  struct t_city{
    vector<t_market> arr;
  };
  struct t_bookmark{
    vector<t_item> items;
  };
  struct t_cargo_item{
    int id=0;
    int amount=0;
  };
  struct t_cargo{
    vector<t_cargo_item> items;
  };
  struct t_car{
    t_cargo cargo;
    int money=1000;
    vec2d pos;
    vec2d v;
    bool deaded=false;
  };
  struct t_dynamic_obstacle{
    vec2d pos;
    real ang=0;
    real len=100;
    real speed=1;
    real r=8;
    bool circle=false;
    real dt=0;
    real gang=0;
    real gspd=0;
  };
  struct t_world{
    int t=0;
    real obstacle_r=48;
    real tank_r=48;
    real market_r=32;
    t_car car;
    t_city city;
    vector<t_dynamic_obstacle> dyn_obs;
    vector<vec2d> obstacles;
  };
  bool sell(t_world&w,int market_id,const t_cargo_item&item){
    auto&cit=w.car.cargo.items[item.id];
    if(cit.amount<item.amount)return false;
    cit.amount-=item.amount;
    auto&it=w.city.arr[market_id].items[item.id];
    it.amount+=item.amount;
    w.car.money+=it.price*item.amount;
    return true;
  }
  bool buy(t_world&w,int market_id,const t_cargo_item&item){
    auto&it=w.city.arr[market_id].items[item.id];
    if(it.amount<item.amount)return false;
    auto dm=it.price*item.amount;
    if(w.car.money<dm)return false;
    it.amount-=item.amount;
    w.car.cargo.items[item.id].amount+=item.amount;
    w.car.money-=dm;
    return true;
  }
public:
  t_world w;
public:
  TGame*Game=nullptr;
public:
  bool init_city(){
    vector<int> base_price;
    base_price.resize(5);
    for(int i=0;i<5;i++){
      base_price[i]=25+rand()%100+pow(3,i+1);
    }
    int try_count=0;
    for(int i=0;i<10;i++){
      t_market m;
      m.pos=vec2d(rand()%1000-500,rand()%1000-500);
      bool ignore=false;
      for(auto&market:w.city.arr){
        if((market.pos-m.pos).Mag()<w.market_r*10)ignore=true;
      }
      if(ignore){i--;try_count++;if(try_count>2000)return false;continue;}
      m.items.resize(5);
      for(int i=0;i<5;i++){m.items[i].id=i;m.items[i].price=base_price[i]+rand()%(50+int(pow(3,i+1)));m.items[i].amount=50+rand()%200;}
      w.city.arr.push_back(m);
    }
    return true;
  }
  void init_cargo_items(){
    w.car.cargo.items.resize(5);
    for(int i=0;i<5;i++)w.car.cargo.items[i].id=i;
  }
  void init_obstacles(){
    for(int i=0;i<20;i++){
      vec2d p=vec2d(rand()%1000-500,rand()%1000-500);
      if((p-w.car.pos).Mag()<w.obstacle_r+w.tank_r)continue;
      bool ignore=false;
      for(auto&ex:w.city.arr){
        if((ex.pos-p).Mag()<w.obstacle_r+w.market_r)ignore=true;
      }
      if(ignore)continue;
      w.obstacles.push_back(p);
    }
  }
  bool init_dyn_obs_v2(){
    vector<vec2d> PA;
    for(auto&ex:w.city.arr){PA.push_back(ex.pos);}
    auto EA=get_voronoi_edges(PA);edges=EA;
    int try_count=0;
    for(int i=0;i<EA.size();i++){
      for(int j=0;j<3;j++){
        auto&it=EA[i];
        real k=(rand()%1000)/1000.0-0.5;
        vec2d p=((it.a+it.b)*0.5+(it.b-it.a)*k);//(it.a+it.b)*0.5;
        real r=18;//rand()%8+10;
        t_dynamic_obstacle ex;
        ex.circle=false;
        ex.pos=p;
        ex.ang=(rand()%(314*2))/100.0;
        ex.len=ex.circle?rand()%100+w.tank_r*2+r:rand()%500+200;
        ex.speed=(rand()%1000)/1000.0+1;
        ex.r=r;
        ex.gang=(rand()%1000)*Pi*2/1000.0;
        ex.gspd=(rand()%2000)/1000.0-1;
        ex.dt=(rand()%32000)*3.14*2*40/32000;
        bool ignore=is_dangerous(ex,w.car.pos,w.tank_r*1.5);
        for(auto&market:w.city.arr){
          if(ignore)break;
          if(is_dangerous(ex,market.pos,w.tank_r*1.5))ignore=true;
        }
        if(ignore){j--;try_count++;if(try_count>2000){j++;bad_edges.push_back(EA[i]);return false;}continue;}
        w.dyn_obs.push_back(ex);
      }
    }
    return bad_edges.empty();
  }
  void init_dyn_obs(){
    int try_count=0;
    for(int i=0;i<7+3+4;i++){
      vec2d p=vec2d(rand()%500-250,rand()%500-250);
      real r=rand()%8+10;
      t_dynamic_obstacle ex;
      ex.circle=i%2;
      ex.pos=p;
      ex.ang=(rand()%(314*2))/100.0;
      ex.len=ex.circle?rand()%500+w.tank_r*2+r:rand()%500+200;
      ex.speed=(rand()%1000)/1000.0+1;
      ex.r=r;
      ex.dt=rand();
      bool ignore=is_dangerous(ex,w.car.pos,w.tank_r*1.5);
      for(auto&market:w.city.arr){
        if(is_dangerous(ex,market.pos,w.tank_r*1.5))ignore=true;
      }
      if(ignore){i--;try_count++;if(try_count>1000)i++;continue;}
      w.dyn_obs.push_back(ex);
    }
  }
  t_world world_at_begin;
  void reinit_the_same_level(){
    w=world_at_begin;
    Game->ReloadWinFail();
  }
  bool init_attempt(){
    static QapClock clock;
    srand(seed=int(clock.MS()*1000));
    bad_edges.clear();
    w={};
    if(!init_city())return false;
    init_cargo_items();
    init_obstacles();
    if(!init_dyn_obs_v2())return false;
    //init_dyn_obs();
    world_at_begin=w;
    return true;
  }
  int init_attempts=1;
  bool inited=false;
  struct t_rec{
    int place;
    string user;
    real sec;
    string date;
    int seed;
    string game;
    vector<string> to_str()const{
      return {IToS(place),user,FToS(sec),date,IToS(seed)};
    }
  };
  vector<t_rec> tops;
  string ref="github";
  void reinit_top20(){
    tops={};
    auto s=TGame::wget(g_host,"/c/game_players_table.js?unique&csv&game=market&n=15&ref="+ref+"&user="+Game->user_name,[&](const string&url,int p,int size){
      string s;
      s.resize(size);
      for(int i=0;i<size;i++)s[i]=((char*)p)[i];
      auto arr=split(s,"\n");
      if(arr.size())QapPopFront(arr);
      for(auto&ex:arr){
        auto t=split(ex,",");
        QapAssert(t.size()>=6);
        t_rec r;
        r.place=stoi(t[0]);
        r.user=t[1];
        r.sec=stof(t[2]);
        r.date=t[3];
        r.seed=stoi(t[4]);
        r.game=t[5];
        tops.push_back(r);
      }
    });
  }
  void Init(TGame*Game){
    this->Game=Game;
    inited=init_attempt();
    if(!inited){Sys.UPS_enabled=false;}
    reinit_top20();
    //for(;;init_attempts++){
    //  bool ok=init_attempt();
    //  if(ok)break;
    //}
  }
  bool is_dangerous(const t_dynamic_obstacle&ex,vec2d pos,real r){
    for(int i=0;i<Sys.UPS*20;i++){
      if((dyn_pos(ex,i)-pos).Mag()<ex.r+r)return true;
    }
    return false;
  }
  bool Win(){return w.car.money>60000/*||w.car.pos.x>10*/;}
  bool Fail(){return w.car.deaded;}
public:
  struct t_edge{
    vec2d a,b;
  };
  vector<t_edge> edges,bad_edges;
  static vector<t_edge> get_voronoi_edges(vector<vec2d>&points){
    double eps=1e-12;
    using value_type=double;
    struct t_hacked_vec2d:vec2d{
      t_hacked_vec2d():vec2d(){}
      t_hacked_vec2d(real x,real y):vec2d(x,y){}
      bool operator<(const t_hacked_vec2d&p)const{return std::tie(x,y)<std::tie(p.x,p.y);}
    };
    using point=t_hacked_vec2d;
    using t_points=std::vector<point>;
    using site=typename t_points::const_iterator;
    using sweepline_type=sweepline<site,point,value_type>;
    auto arr=(vector<t_hacked_vec2d>&)points;
    sweepline_type SL{eps};
    // fill points_ with data
    std::sort(std::begin(arr), std::end(arr));
    //quick_sort(arr);
    SL(std::cbegin(arr),std::cend(arr));
    //SL.vertices_
    vector<t_edge> edges;
    edges.clear();
    vector<vec2d> VA;
    for(auto&ex:SL.vertices_)VA.push_back(ex.c);
    for(auto&ex:SL.edges_){
      if(ex.b==SL.inf&&ex.e==SL.inf)continue;
      edges.push_back({*ex.l,*ex.r});
    }
    return edges;
  }
public:
  string id2str(int id){
    static vector<string> arr={"wood","coal","gas","stell","copper"};
    return arr[id];
  }
  int get_market_id(vec2d pos,bool ignore_r){
    auto mpos=pos;//kb.MousePos
    int market_id=0;
    for(int i=0;i<w.city.arr.size();i++){
      auto a=(mpos-w.city.arr[i].pos).Mag();
      auto b=(mpos-w.city.arr[market_id].pos).Mag();
      if(a<b)market_id=i;
    }
    auto dist=(mpos-w.city.arr[market_id].pos).Mag();
    if(!ignore_r)if(dist>w.market_r)return -1;
    return market_id;
  }
  void AddText(TextRender*TE){
    string BEG="^7";
    string SEP=" ^2: ^8";
    #define GOO(TEXT,VALUE)TE->AddText(string(BEG)+string(TEXT)+string(SEP)+string(VALUE));
    GOO("curr_t",FToS(w.t*1.0/Sys.UPS));
    GOO("prev_best_t",FToS(prev_best_t*1.0/Sys.UPS));
    GOO("curr_best_t",FToS(best_t*1.0/Sys.UPS));
    GOO("px",IToS(kb.MousePos.x));
    GOO("py",IToS(kb.MousePos.y));
    GOO("init_attempts",IToS(init_attempts));
    TE->AddText("^7---");
    /*auto market_id=get_market_id();
    for(auto&it:w.city.arr[market_id].items){
      TE->AddText(BEG+id2str(it.id)+SEP+IToS(it.price)+SEP+IToS(it.amount));
    }*/
    GOO("Money",IToS(w.car.money));
    TE->AddText("^8Need ^760000 ^8money to ^2win^8 in this game");
    TE->AddText("^7---");
    TE->AddText("^8Your cargo:");
    for(auto&it:w.car.cargo.items){
      TE->AddText(BEG+id2str(it.id)+SEP+IToS(it.amount));
    }
    TE->AddText("^7---");
    TE->AddText("TOP15:");
    vector<vector<string>> arr;
    vector<int> lens;lens.resize(5);
    for(auto&it:tops){
      arr.push_back(it.to_str());
    }
    for(auto&it:arr){
      for(int i=0;i<5;i++)lens[i]=max(lens[i],TE->text_len(it[i]));
    }
    auto seplen=TE->text_len("  ");
    vector<string> c={"^8","^7","^8","^7","^8"};
    for(auto&it:arr){
      for(int i=0;i<3;i++){auto x=TE->x;TE->AddTextNext(c[i]+it[i]);TE->x=x+lens[i]+seplen;}
      TE->BR();
    }
    #undef GOO
  }
  void RenderText(QapDev&RD)
  {
    TextRender TE(&RD);
    vec2d hs=vec2d(Sys.SM.W,Sys.SM.H)*0.5;
    hs.x=100;
    real ident=24.0;
    real Y=0;
    RD.SetColor(0xff000000);
    TE.BeginScope(-hs.x+ident,+hs.y-ident,&Game->NormFont,&Game->BlurFont);
    {
      string BEG="^7";
      string SEP=" ^2: ^8";
      //TE.AddText("");
      RenderText(&RD,&TE);
    }
    TE.EndScope();
  }
  //vector<TextRender::TextLine> tls;
  void DrawMarketMenu(QapDev*RD,TextRender*TE,real text_posx,int mid,bool buttons)
  {
    if(w.car.cargo.items.empty())return;
    if(mid<0)return;
    string BEG="^7";
    string SEP=" ^2: ^8";
    const real dy=32;
    auto&items=w.city.arr[mid].items;
    TE->bx=text_posx;
    TE->x=TE->bx;
    TE->y=+0.5*items.size()*dy;
    //RD->BindTex(0,Game->Atlas.pTex);
    //Game->FrameMenuItem->Bind(RD);
    //RD->SetColor(0x80ffffff);
    //RD->DrawQuad(0,y-dy*CurID,Game->FrameMenuItem->w,Game->FrameMenuItem->h,0);
    vector<string> tearr,idarr,pricearr,amountarr;
    for(int i=0;i<items.size();i++){
      idarr.push_back(BEG+id2str(i));
      pricearr.push_back(IToS(items[i].price));
      amountarr.push_back(IToS(items[i].amount));
      tearr.push_back(BEG+id2str(i)+SEP+IToS(items[i].price)+SEP+IToS(items[i].amount));
    }
    auto get_maxlen=[&](vector<string>&arr){
      int maxlen=0;
      for(int i=0;i<arr.size();i++){
        auto len=TE->text_len(arr[i]);
        if(len>maxlen)maxlen=len;
      }
      return maxlen;
    };
    int idlen=get_maxlen(idarr);
    int pricelen=get_maxlen(pricearr);
    int amountlen=get_maxlen(amountarr);
    int maxlen=get_maxlen(tearr);
    int seplen=TE->text_len(SEP);
    bt_buy_all={};bt_sell_all={};
    for(int i=0;i<idarr.size();i++){
      TE->AddTextNext(idarr[i]);
      TE->x=TE->bx+idlen;
      TE->AddTextNext(SEP);
      TE->x=TE->bx+idlen+seplen+pricelen-TE->text_len(pricearr[i]);
      TE->AddTextNext(pricearr[i]);
      TE->x=TE->bx+idlen+seplen+pricelen;
      TE->AddTextNext(SEP);
      TE->x=TE->bx+idlen+seplen+pricelen+seplen+amountlen-TE->text_len(amountarr[i]);
      TE->AddTextNext(amountarr[i]);
      TE->x=TE->bx+idlen+seplen+pricelen+seplen+amountlen;
      TE->AddTextNext(SEP);
      //TE->x=TE->bx+maxlen;
      if(buttons)
      {
        auto dpos=kb.MousePos-vec2d(TE->x,TE->y-TE->ident);
        auto es=vec2d(TE->text_len(" ^7[BuyAll]"),TE->ident);
        bool hovered=check_rect(dpos,es);
        TE->AddTextNext(" "+string(hovered?"^8":"^7")+"[BuyAll]");
        t_cargo_item ci;
        ci.id=i;
        ci.amount=w.car.money/items[i].price;
        if(ci.amount>items[i].amount)ci.amount=items[i].amount;
        if(hovered&&kb.Down[mbLeft]){/*buy(w,mid,ci);*/TE->LV.back().text=" ^2[BuyAll]";}
        if(hovered){
          if(kb.Down[mbLeft]){
            int gg=1;
          }
          bt_buy_all={ci,mid,hovered};
        }
      }
      if(buttons)
      {
        auto dpos=kb.MousePos-vec2d(TE->x,TE->y-TE->ident);
        auto es=vec2d(TE->text_len(" ^7[SellAll]"),TE->ident);
        bool hovered=check_rect(dpos,es);
        TE->AddTextNext(" "+string(hovered?"^8":"^7")+"[SellAll]");
        t_cargo_item ci;
        ci.id=i;
        ci.amount=w.car.cargo.items[i].amount;
        if(hovered&&kb.Down[mbLeft]){/*sell(w,mid,ci);*/TE->LV.back().text=" ^2[SellAll]";}
        if(hovered){bt_sell_all={ci,mid,hovered};}
      }
      TE->BR();
    }
    //tls=TE->LV;
  }
  struct t_button{t_cargo_item ci;int mid=0;bool hovered=false;};
  t_button bt_buy_all;
  t_button bt_sell_all;
  bool check_rect(vec2d dpos,vec2d es){
    return (dpos.x>0&&dpos.x<es.x)&&(dpos.y>0&&dpos.y<es.y);
  }
  void RenderText(QapDev*RD,TextRender*TE)
  {
    int mid=get_market_id(kb.MousePos,true);
    int pid=get_market_id(w.car.pos,false);
    DrawMarketMenu(RD,TE,400,mid,false);
    DrawMarketMenu(RD,TE,0,pid,true);
  }
  vec2d dyn_pos(const t_dynamic_obstacle&ex,int dt=0)
  {
    vec2d offset=Vec2dEx(ex.ang,ex.len*sin(M_PI*2*ex.speed*(w.t+dt)/30/Sys.UPS));
    vec2d offset2=Vec2dEx(M_PI*2*ex.speed*(w.t+dt+ex.dt)/30/Sys.UPS,ex.len);
    return ex.pos+(ex.circle?offset2:offset);
  }
  bool need_draw_dyn_obs_lines=false;
  void Render(QapDev*RD){
    if(bool need_draw_rock0_as_thrt=true){
      auto&qDev=*RD;
      if(need_draw_dyn_obs_lines/*kb.Down['L']*/){
        RD->BindTex(0,0);
        qDev.SetColor(0xffbbbbbb);
        for(auto&ex:w.dyn_obs){
          auto d=Vec2dEx(ex.ang,ex.len);
          DrawLine(qDev,ex.pos-d,ex.pos+d,ex.r*2);
        }
        qDev.SetColor(0xffbbbbff);
        for(auto&ex:w.dyn_obs){
          auto d=Vec2dEx(ex.ang,ex.len);
          qDev.DrawQuad(ex.pos.x,ex.pos.y,8,8);
        }
      }
      auto draw_shadow_quad_v2=[&](QapDev&qDev,const TGame::t_frame&f,vec2d pos,real r,QapColor c){
        real zoom=r*2/real(f.pF->w);
        draw_shadow_quad(qDev,f.pS,true,pos,vec2d(f.pS->w*zoom,f.pS->h*zoom),c);
        draw_shadow_quad(qDev,f.pF,false,pos,vec2d(1,1)*r*2,c);
      };
      if(bool need_draw_obstacles=Game->FrameObstacle){
        //RD->BindTex(0,0);
        qDev.SetColor(0xff888888);
        RD->BindTex(0,Game->Atlas.pTex);
        QapDev::BatchScope Scope(qDev);
        auto&F=*Game->FrameObstacle;
        qDev.SetColor(0xffffffff);
        F.Bind(RD);
        for(auto&ex:w.obstacles){
          qDev.DrawQuad(ex.x,ex.y,F.w,F.h);
          //qDev.DrawCircleEx(ex,0,w.obstacle_r,32,0);
          //draw_shadow_quad_v2(qDev,Game->frame_BigDot,ex,w.obstacle_r,0xff77aa77);
        }
      }
      if(bool need_draw_dyn_obs=Game->FrameEnemy){
        //qDev.SetColor(0xff777777);
        qDev.SetColor(0xffffffff);
        RD->BindTex(0,Game->Atlas.pTex);
        Game->FrameEnemy->Bind(RD);
        QapDev::BatchScope Scope(qDev);
        for(auto&m:w.dyn_obs){
          auto p=dyn_pos(m);
          qDev.DrawQuad(p.x,p.y,128,128,m.gang);
          //draw_shadow_quad_v2(qDev,Game->frame_BigDot,dyn_pos(m),m.r,0xff777777);
        }
        //for(auto&ex:w.dyn_obs){
        //  RD->DrawCircleEx(dyn_pos(ex),0,ex.r,32,0);
        //}
      }
      if(bool need_draw_tank=Game->th_rt_tex&&Game->th_rt_tex_full){
        bool cargo_empty=true;
        for(auto&ex:w.car.cargo.items)if(ex.amount>0)cargo_empty=false;
        auto*pF=cargo_empty?Game->th_rt_tex:Game->th_rt_tex_full;auto&qDev=*RD;
        qDev.BindTex(0,pF);
        qDev.SetColor(0xffffffff);
        auto scale=0.5;
        RD->DrawQuad(w.car.pos.x,w.car.pos.y,pF->W*scale,pF->H*scale,(-w.car.v.Ort()).GetAng());
      }
      if(bool need_draw_voronoi=kb.Down['V']){
        RD->BindTex(0,0);
        qDev.SetColor(0xff0000ff);
        for(auto&ex:edges){
          DrawLine(*RD,ex.a,ex.b,4);
        }
        qDev.SetColor(0xffff0000);
        for(auto&ex:bad_edges){
          DrawLine(*RD,ex.a,ex.b,4);
        }
      }
      if(bool need_draw_markets=Game->FrameMarket){
        qDev.SetColor(0xffffffff);
        RD->BindTex(0,Game->Atlas.pTex);
        QapDev::BatchScope Scope(qDev);
        Game->FrameMarket->Bind(RD);
        for(auto&m:w.city.arr){
          //RD->DrawQuad(m.pos.x,m.pos.y,40,40);
          RD->DrawQuad(m.pos.x,m.pos.y,128,128);
          //draw_shadow_quad_v2(qDev,Game->frame_Dot,m.pos,16,0xFFffFFff);
        }
      }
      int mid=get_market_id(kb.MousePos,true);
      if(mid>=0&&Game->frame_Dot){
        auto mpos=w.city.arr[mid].pos;
        RD->BindTex(0,Game->Atlas.pTex);
        qDev.SetColor(0xffff0000);
        draw_shadow_quad_v2(qDev,Game->frame_Dot,mpos,16,0xffff0000);
        //RD->DrawQuad(mpos.x,mpos.y,23,23);
      }
      int pid=get_market_id(w.car.pos,false);
      if(pid>=0&&Game->frame_Dot){
        auto mpos=w.city.arr[pid].pos;
        RD->BindTex(0,Game->Atlas.pTex);
        qDev.SetColor(0xff00ff00);
        //RD->DrawQuad(mpos.x,mpos.y,20,20);
        draw_shadow_quad_v2(qDev,Game->frame_Dot,mpos,16*0.45,0xff00ff00);
      }
      /*if(Game->Atlas.pTex){
        qDev.SetColor(0xffffffff);
        RD->BindTex(0,Game->Atlas.pTex);
        RD->DrawQuad(+500,0,1024,1024);
      }*/
      RenderText(*RD);
    }
  }
  static void draw_shadow_quad(QapDev&qDev,QapAtlas::TFrame*pF,bool shadow,vec2d pos,vec2d wh,QapColor color,real ang=0){
    qDev.color=shadow?QapColor(0xff000000):color;
    pF->Bind(&qDev);
    auto p=pos;
    if(shadow)p+=vec2d(1.0,-1.0);
    qDev.DrawQuad(p.x,p.y,wh.x,wh.y,ang);
  }
  void DrawLine(QapDev&qDev,const vec2d&a,const vec2d&b,real line_size)
  {
    auto p=(b+a)*0.5;
    qDev.DrawQuad(p.x,p.y,(b-a).Mag(),line_size,(b-a).GetAng());
  }
  bool colide(){
    for(auto&ex:w.dyn_obs){
      if((w.car.pos-dyn_pos(ex)).Mag()<ex.r+w.tank_r)return true;
    }
    return false;
  }
  void Update(TGame*Game){
    if(!inited){
      bool ok=init_attempt();
      init_attempts++;
      if(ok){inited=true;Sys.UPS_enabled=true;Sys.ResetClock();}else{return;}
    }
    {
      auto&bt=bt_buy_all;
      if(kb.OnDown(mbLeft)){
        if(bt.hovered){
          buy(w,bt.mid,bt.ci);
        }
      }
    }
    {
      auto&bt=bt_sell_all;
      if(bt.hovered&&kb.OnDown(mbLeft)){sell(w,bt.mid,bt.ci);}
    }
    for(auto&ex:w.dyn_obs){ex.gang+=Pi*0.9*ex.gspd/180;}
    if(kb.OnDown('L'))need_draw_dyn_obs_lines=!need_draw_dyn_obs_lines;
    //v+=get_dir_from_keyboard_wasd_and_arrows()*0.003;
    auto dk=0.0;auto dAng=6.28*0.33/(2*Sys_UPD);
    if(kb.OnDown(VK_F9)){reinit_the_same_level();}
    if(kb.Down[VK_LEFT]||kb.Down['A']){dk=+1;}
    if(kb.Down[VK_RIGHT]||kb.Down['D']){dk=-1;}
    if(w.car.deaded)dk=0;
    w.car.v=Vec2dEx(w.car.v.GetAng()+Clamp(dk,-5.0,+5.0)*dAng,1);
    auto v2=vec2d();
    if(kb.Down[VK_UP]||kb.Down['W'])v2=+w.car.v;
    if(kb.Down[VK_DOWN]||kb.Down['S'])v2=-w.car.v;
    auto new_pos=w.car.pos+v2;
    auto new_v2=vec2d(0,0);int n=0;
    for(auto&ex:w.obstacles){
      auto dist=w.obstacle_r+w.tank_r;
      if((new_pos-ex).Mag()<dist){
        //v2=vec2d_zero;
        new_v2+=(new_pos-ex).SetMag(dist)+ex-w.car.pos;
        n++;
      }
    }
    v2=n?new_v2*(1.0/n):v2;
    bool runned=!Win()&&!Fail();
    if(runned)w.car.pos+=v2;
    if(runned)w.t++;
    if(colide())w.car.deaded=true;
    on_win=false;
    if(Win()){if(!wined)on_win=true;wined=true;}
    if(bool need_best_t=true){
      auto fn="score.txt";
      if(w.t==1){
        auto s=file_get_contents(fn);
        prev_best_t=s.empty()?1e9:stoi(s);
        best_t=prev_best_t;
      }
      if(on_win){
        if(prev_best_t>w.t){best_t=w.t;file_put_contents(fn,IToS(w.t));}
      }
    }
    if(on_win){
      TGame::wget(g_host,"/c/game_players_table.js?game=market&user="+Game->user_name+"&sec="+FToS(w.t*1.0/Sys.UPS)+"&seed="+IToS(seed)+"&ref="+ref,[](const string&url,int ptr,int size){});
      //QapDebugMsg(s);
      reinit_top20();
    }
  }

  int seed=0;
  int prev_best_t=0;
  int best_t=0;
  bool wined=false;
  bool on_win=false;
};
class IOnClick;
class TMenu;
struct MenuItem{
public:
  string Caption;
  IOnClick*OnClick;
  MenuItem(const string&Caption,IOnClick*OnClick){this->Caption=Caption;this->OnClick=OnClick;};
};
class IOnClick{
public:
  virtual void Call(TMenu*EX)=0;
  virtual bool IsEnabled(TMenu*EX){return true;}
};
class TMenu{
private:
  bool in_game;
public:
  bool InGame(){return in_game;}
  void Up(){in_game=false;}
  void Down(){if(Game->Level.get())in_game=true;}
public:
  vector<MenuItem>Items;
  string Caption;
  TGame*Game;
  std::unique_ptr<TMenu>OldMenu;
  vec2d oldmp;
  int CurID;
  TMenu(TGame*Game,const string&Caption){this->Game=Game;this->Caption=Caption;};
  void Add(const string&s,IOnClick*p){Items.push_back(MenuItem(s,p));}
  void Back(){
    QapAssert(OldMenu.get());
    Game->Menu.release();
    Game->Menu.reset(OldMenu.release());
    //create memory leak
    delete this;
  }
  void AddText(real&y,TextRender*TE,const string&s,const real dy=32){
    real x=GetQ3TextLength(*TE->NormFont,s);
    TE->LV.push_back(TextRender::TextLine(-x*0.5,y+TE->NormFont->H[0]*0.5,s));y-=dy;
  };
  void Render(QapDev*RD,TextRender*TE)
  {
    if(!Game->FrameMenuItem||!Game->Atlas.pTex)return;
    const real dy=32;
    real y=+0.5*Items.size()*dy;
    RD->BindTex(0,Game->Atlas.pTex);
    Game->FrameMenuItem->Bind(RD);
    RD->SetColor(0x80ffffff);
    RD->DrawQuad(0,y-dy*CurID,Game->FrameMenuItem->w,Game->FrameMenuItem->h,0);
    for(int i=0;i<Items.size();i++)AddText(y,TE,string(Items[i].OnClick->IsEnabled(this)?CurID==i?"^3":"^7":"^D")+Items[i].Caption,dy);
  }
  void Update(TGame*Game)
  {
    CurID%=Items.size();
    const real dy=32;
    real y=+0.5*Items.size()*dy;
    auto mp=kb.MousePos;
    vec2d hmis=vec2d(Game->FrameMenuItem->w,dy)*0.5;
    if(mp!=oldmp)
    {
      for(int i=0;i<Items.size();i++)
      {
        if(Items[i].OnClick->IsEnabled(this))if(CD_Rect2Point(-hmis+vec2d(0,y),hmis+vec2d(0,y),mp))CurID=i;
        y-=dy;
      }
      oldmp=mp;
    }
    for(int i=CurID;i<CurID+Items.size();i++)if(Items[i%Items.size()].OnClick->IsEnabled(this)){CurID=i;break;}
    if(kb.OnDown(VK_DOWN)){
      CurID++;
      for(int i=CurID;i<CurID+Items.size();i++)if(Items[i%Items.size()].OnClick->IsEnabled(this)){CurID=i;break;}
    }
    if(kb.OnDown(VK_UP)){
      CurID--;
      for(int i=CurID+Items.size();i>CurID;i--)
        if(Items[i%Items.size()].OnClick->IsEnabled(this))
        {
          CurID=i;break;
        }
    }
    CurID+=Items.size();
    CurID%=Items.size();
    if(kb.OnDown(VK_RETURN)){
      Items[CurID].OnClick->Call(this);
    }
    if(kb.OnDown(mbLeft)){
      vec2d dY(0,0.5*Items.size()*dy-CurID*dy);
      if(Items[CurID].OnClick->IsEnabled(this))if(CD_Rect2Point(-hmis+dY,hmis+dY,mp))
      {
        Items[CurID].OnClick->Call(this);
      }
    }
  }
};
public:
  struct t_frame{
    TFrame*pF=0;
    TFrame*pS=0;
    string name,file,fn;
    int mode=0;
    operator bool()const{return pF&&pS;}
  };
public:
#define FRAMESCOPE(F)\
  F(Dot,"dot",2)\
  F(MenuItem,"MenuItem",0)\
  F(Market,"market",0)\
  F(Enemy,"enemy_v3_128",0)\
  F(Obstacle,"obstacle_v4",0)\
  //---
#define ADDFRAME(NAME,FILE,MODE)TFrame*Frame##NAME;TFrame*Frame##NAME##_s;t_frame frame_##NAME={0,0,#NAME,FILE,"",MODE};
  FRAMESCOPE(ADDFRAME)
#undef ADDFRAME
public:
  TCounterIncEx WaitWin=TCounterIncEx(0,0,Sys.UPS*2);
  TCounterIncEx WaitFail=TCounterIncEx(0,0,Sys.UPS*2);
  TCounterInc LevelCounter=TCounterInc(0,0,0);
public:
  vector<TLevelInfo>LevelsInfo;
  std::unique_ptr<ILevel>Level;
  std::unique_ptr<TMenu>Menu;
public:
  QapAtlas Atlas;
  QapDev RD;
  QapFont NormFont;
  QapFont BlurFont;
  QapTex*th_rt_tex{};
  QapTex*th_rt_tex_full{};
public:
  TGame(){}
public:
  QapTexMem*AddBorder(QapTexMem*pMem,int dHS=8,const QapColor&Color=0xffffffff)
  {
    int dS=dHS*2;
    QapTexMem*sm=new QapTexMem("ShadowBot",pMem->W+dS,pMem->H+dS,NULL);
    sm->pBits=new QapColor[sm->W*sm->H];
    sm->Clear(Color);
    sm->FillMem(dHS,dHS,pMem);
    //sm->FillBorder(dHS,dHS,pMem,dHS);
    return sm;
  }
  QapTexMem*GenShadow(QapTexMem*pMem,int dHS=8)
  {
    int dS=dHS*2;
    vec4f acc={0,0,0,0};
    for(int x:{0,1})for(int y:{0,1})acc+=pMem->get_color_at(x*(pMem->W-1),y*(pMem->H-1));
    acc*=0.25;
    QapColor c=acc.GetColor();
    auto*sm=AddBorder(pMem,dHS,c);
    sm->CalcAlpha(0xffffffff);
    sm->FillChannel(0x00ffffff,0x00ffffff);
    BlurTexture(sm,dHS);
    return sm;
  }
  TFrame*GenShadowFrame(QapTexMem*pMem,int dHS=8)
  {
    QapTexMem*sm=GenShadow(pMem,dHS);
    TFrame*FrameX=Atlas.AddFrame(sm);
    delete sm;
    return FrameX;
  }
  void LoadFrames(bool need_save_atlas=0,bool need_rewrite_tex=0)
  {
    //auto*ball=LoadTexture("GFX\\Ball.png");
    //#define F(NAME)LoadTexture("GFX\\"#NAME".png")->CopyAlpha(ball)->SaveToFile("GFX\\"#NAME".png");
    //auto LT=LoadTexture;
    auto m2=[&](QapTexMem*p){return p->CalcAlpha()->FillChannel(0xffffffff,0x00ffffff);};
    /*
    if(bool hack=false)
    {
      auto f=[&](auto NAME,string FILE,auto MODE){
        auto fn="GFX\\"+FILE+".png";
        if(MODE==2){auto*p=m2(LT(fn));if(need_rewrite_tex)p->SaveToFile(fn);}
        if(MODE==0){auto*p=LT(fn)->CalcAlpha(0xffffffff);if(need_rewrite_tex)p->SaveToFile(fn);}
        if(MODE==1){auto*p=LT(fn)->CopyAlpha(LT("GFX\\"+FILE+"_a.png")->CalcAlpha());if(need_rewrite_tex)p->SaveToFile(fn);}
      };
      #define F(NAME,FILE,MODE)f(Frame##NAME,FILE,MODE);
        FRAMESCOPE(F);
        //LoadTexture("GFX\\You.png")->CopyAlpha(GenBall(32))->SaveToFile("GFX\\You.png");
      #undef F
    }*/
    static int frames=0;static bool done=false;
    {
      static auto on_load_tex=[&](string fn){
        frames--;
        QAP_EM_LOG("on_load_tex:"+fn);
        if(frames||done)return;
        QAP_EM_LOG("on_load_tex_done_at:"+fn);
        done=true;
        Atlas.GenTex();
        QAP_EM_LOG("on_load_tex_done_at_aft_GenTex:"+fn);
      };
      #define F(NAME,FILE,MODE)frames++;
      FRAMESCOPE(F);
      #undef F
      #define F(NAME,FILE,MODE){\
        t_frame&f=frame_##NAME;f.fn="GFX\\" FILE ".png";\
        LoadTexture(f.fn,[&](const string&fn,int ptr,int w,int h){\
          QAP_EM_LOG("on_LoadTexture:"+fn+" "+IToS(w)+" "+IToS(h));\
          QapTexMem*pMem=new QapTexMem(fn+"_"+to_string(w),w,h,(QapColor*)ptr);\
          if(MODE==2)pMem=m2(pMem);\
          Frame##NAME=Atlas.AddFrame(pMem);\
          if(MODE==2)pMem->CalcAlphaToRGB_and_set_new_alpha()->InvertRGB();\
          if(MODE==2)Frame##NAME##_s=GenShadowFrame(pMem);\
          f.pF=Frame##NAME;f.pS=Frame##NAME##_s;\
          delete pMem;\
          on_load_tex(fn);\
        });\
      }
      FRAMESCOPE(F);
      #undef F
    }
    //if(need_save_atlas)Atlas.pMem->SaveToFile("Atlas.png");
    //Atlas.GenTex();
  }
  void Init()
  {
    srand(time(NULL));
    
    QAP_EM_LOG("before CreateFontMem");
    {
      QapTexMem*pNormMem=NormFont.CreateFontMem("Arial",14,false,512);
      QapTexMem*pBlurMem=pNormMem->Clone();
      //pBlurMem->Blur(10);
      //pBlurMem->Blur(4);
      BlurTexture(pBlurMem,4);
      BlurFont=NormFont;
      BlurFont.Tex=GenTextureMipMap(pBlurMem);
      NormFont.Tex=GenTextureMipMap(pNormMem);
      //SysFont=FontCreate("Arial",16,false,512);
    }
    QAP_EM_LOG("before LoadFrames");
    LoadFrames();
    
    QAP_EM_LOG("before LoadTexture");
    if(1)
    {
      LoadTexture("GFX\\market_car_v2.png",[&](const string&fn,int ptr,int w,int h){
        QapTexMem*pMem=new QapTexMem(fn+"_"+to_string(w),w,h,(QapColor*)ptr);
        th_rt_tex=GenTextureMipMap(pMem);
      });
      //th_rt_tex=GenTextureMipMap(ptm);
      
      LoadTexture("GFX\\market_car_v2_full.png",[&](const string&fn,int ptr,int w,int h){
        QapTexMem*pMem=new QapTexMem(fn+"_"+to_string(w),w,h,(QapColor*)ptr);
        th_rt_tex_full=GenTextureMipMap(pMem);
      });
      //ptm=LoadTexture("GFX\\market_car_v2_full.png");
      //th_rt_tex_full=GenTextureMipMap(ptm);
    }
    QAP_EM_LOG("before RD.Init();");
    RD.Init(1024*32,1024*32*2);
    QAP_EM_LOG("after RD.Init();");
    InitLevelsInfo();
    //RestartLevel();
    InitMenuSystem();
    update_user_name();
    RestartLevel();
    Menu->Down();
    
    QAP_EM_LOG("after init");
  }
  void InitLevelsInfo()
  {
    #define LEVEL_LIST(F)F(Level_MarketGame);
    #define ADDLEVEL(CLASS){static TLevelFactory<CLASS>tmp;LevelsInfo.push_back(TLevelInfo(#CLASS,tmp));}
    LEVEL_LIST(ADDLEVEL);
    #undef ADDLEVEL
    LevelCounter.Maximum=LevelsInfo.size();
  }
  void NewGame(){
    LevelCounter.Value=0;
    RestartLevel();
  }
  void RestartLevel()
  {
    if(!LevelCounter){
      return;
    };
    Level.reset(LevelsInfo[LevelCounter.Value].Factory.Build(this));
    WaitWin.Stop();
    WaitFail.Stop();
  }
  void InitMenuSystem(){
    static class TOnResume:public IOnClick{
    public:
      void Call(TMenu*EX){
        EX->Game->Menu->Down();
      }
      virtual bool IsEnabled(TMenu*EX){return EX->Game->Level.get();}
    } OnResume;
    static class TOnRestartLevel:public IOnClick{
    public:
      void Call(TMenu*EX){
        EX->Game->RestartLevel();
        EX->Game->Menu->Down();
      }
      virtual bool IsEnabled(TMenu*EX){return EX->Game->LevelCounter.Value;}
    } OnRestartLevel;
    static class TOnNewGame:public IOnClick{
    public:
      void Call(TMenu*EX){
        EX->Game->NewGame();
        EX->Game->Menu->Down();
      }
    } OnNewGame;
    static class TOnBack:public IOnClick{
    public:
      void Call(TMenu*EX){
        EX->Back();
      }
    } OnBack;
    static class TOnNothing:public IOnClick{
    public:
      void Call(TMenu*EX){}
      virtual bool IsEnabled(TMenu*EX){return false;}
    } OnNothing;
    static class TOnSettings:public IOnClick{
    public:
      void Call(TMenu*EX){
        auto&Menu=EX->Game->Menu;
        auto OldMenu=std::unique_ptr<TMenu>(Menu.release());
        Menu.reset(new TMenu(EX->Game,"Settings"));
        Menu->Add("Under construction",&OnNothing);
        Menu->Add("Back",&OnBack);
        Menu->OldMenu=std::unique_ptr<TMenu>(OldMenu.release());
        Menu->Up();
      }
      //virtual bool IsEnabled(TMenu*EX){return false;}
    } OnSettings;
    static class TOnLevel:public IOnClick{
    public:
      void Call(TMenu*EX){
        auto&m=*EX->Game->Menu.get();
        //auto&lvls=EX->Game->LevelsInfo;
        //m.Items[m.CurID].Caption
        EX->Game->LevelCounter.Value=m.CurID;
        EX->Game->RestartLevel();
        m.Down();
      }
      //virtual bool IsEnabled(TMenu*EX){return false;}
    } TOnLevel;
    static class TOnLevels:public IOnClick{
    public:
      void Call(TMenu*EX){
        auto&Menu=EX->Game->Menu;
        auto OldMenu=std::unique_ptr<TMenu>(Menu.release());
        Menu.reset(new TMenu(EX->Game,"Levels"));
        for(int i=0;i<EX->Game->LevelsInfo.size();i++){
          auto&ex=EX->Game->LevelsInfo[i];
          Menu->Add(ex.Name,&TOnLevel);
        }
        Menu->Add("Back",&OnBack);
        Menu->OldMenu=std::unique_ptr<TMenu>(OldMenu.release());
        Menu->Up();
      }
      //virtual bool IsEnabled(TMenu*EX){return false;}
    } OnLevels;
    static class TOnAbout:public IOnClick{
    public:
      void Call(TMenu*EX){
        auto&Menu=EX->Game->Menu;
        auto OldMenu=std::unique_ptr<TMenu>(Menu.release());
        Menu.reset(new TMenu(EX->Game,"About"));
        Menu->Add("^2Aut^2hor ^2: ^8Ad^8ler^33D",&OnNothing);
        Menu->Add("^2Co^2de ^2: ^8Ad^8ler^33D",&OnNothing);
        Menu->Add("^2A^2r^2t ^2: ^8Ad^8ler^33D",&OnNothing);
        Menu->Add("Back",&OnBack);
        Menu->OldMenu=std::unique_ptr<TMenu>(OldMenu.release());
        Menu->Up();
      }
      //virtual bool IsEnabled(TMenu*EX){return false;}
    } OnAbout;
    static class TOnExit:public IOnClick{
    public:
      void Call(TMenu*EX){
        //Sys.Quit();
      }
    } OnExit;
    Menu.reset(new TMenu(this,"Main menu"));
    Menu->Add("Resume",&OnResume);
    Menu->Add("Restart level",&OnRestartLevel);
    Menu->Add("New game",&OnNewGame);
    Menu->Add("Levels",&OnLevels);
    Menu->Add("Settings",&OnSettings);
    Menu->Add("About",&OnAbout);
    Menu->Add("Exit",&OnExit);
    LevelCounter.Value=0;
    Menu->Down();
    //Menu->Up();
  }
  void Resume()
  {

  }
  string user_name="Adler";
  bool user_name_scene=false;
  void InputUserNameRender(){
    TextRender TE(&RD);
    vec2d hs=vec2d(Sys.SM.W,Sys.SM.H)*0.5;
    real ident=24.0;
    real Y=0;
    RD.SetColor(0xff000000);
    TE.BeginScope(-hs.x+ident,+hs.y-ident,&NormFont,&BlurFont);
    {
      const string PreesR=" ^7(^3press ^2R^7)";
      const string PreesSpace=" ^7(^3press ^2Space^7)";
      const string PreesEnter=" ^7(^3press ^2Return^7)";
      string BEG="^7";
      string SEP=" ^2: ^8";
      TE.AddText("^7The ^2Market Game");
      TE.AddText("");
      TE.AddText("^7Type your ^8user_name ^7and press enter!");
      TE.AddText("^8user_name ^2: ^7"+user_name);
    }
    TE.EndScope();
  }
  bool need_init=true;
  bool check_char(char c){
    return InDip('a',c,'z')||InDip('A',c,'Z')||/*InDip('а',c,'я')||InDip('А',c,'Я')||*/InDip('0',c,'9');//||c=='ё'||c=='Ё';
  }
  void update_user_name(){
    user_name=file_get_contents(user_name_fn);
    string un;
    for(auto&c:user_name)if(check_char(c))un.push_back(c);
    user_name=un;
  }
  string user_name_fn="user_name.txt";
  void InputUserNameUpdate(){
    if(need_init&&user_name.empty()){
      need_init=false;
      update_user_name();
      user_name_scene=user_name.empty();
    }
    if(kb.News){
      auto c=kb.LastChar;
      if(check_char(c))user_name.push_back(c);
    }
    if(kb.OnDown(VK_BACK))if(user_name.size())user_name.pop_back();
    if(kb.Down[VK_RETURN]){
      if(user_name.size()){
        user_name_scene=false;
        file_put_contents(user_name_fn,user_name);
      }
    }
  }
  bool RenderScene_debug=false;
  void RenderScene()
  {
    /*
    RD.BindTex(0,0);
    RD.SetColor(0xff000000);
    RD.DrawQuad(0,0,512,512);
    if(!EndScene())return;
    Present();
    return;*/
    if(user_name_scene)return InputUserNameRender();
    if(bool need_draw_tank_hodun_rt=true)if(th_rt_tex)if(!Menu->InGame()){
      RD.BindTex(0,th_rt_tex);
      RD.SetColor(0xffffffff);
      RD.DrawQuad(-512,0,th_rt_tex->W,th_rt_tex->H,0);
      RD.BindTex(0,0);
    }
    if(RenderScene_debug)QAP_EM_LOG("before kb.A");
    if(kb.Down['A']&&!Menu->InGame()){
      if(1){
        RD.BindTex(0,0);
        RD.SetColor(0xffffffff);
        RD.DrawQuad(kb.MousePos.x,kb.MousePos.y,96,96,0);
        RD.SetColor(0xffff0000);
        RD.DrawQuad(kb.MousePos.x,kb.MousePos.y,64,64,0);
      }
      RD.BindTex(0,Atlas.pTex);
      //RD.SetBlendMode(BT_SUB);
      RD.SetColor(0xffffffff);
      RD.DrawQuad(0.5,0.5,Atlas.W,Atlas.H,0);
    }
    if(RenderScene_debug)QAP_EM_LOG("after kb.A");
    QapAssert(Menu.get());
    if(Menu->InGame())
    {
      auto check_frames=[&](){
        for(auto&ex:g_global_imgs)if(!ex.second.done){QAP_EM_LOG("inside check_frames::fail");return false;}
        return true;
      };
      if(Level.get())if(check_frames()){
        if(RenderScene_debug)QAP_EM_LOG("before Level->Render");
        Level->Render(&RD);
      }
    }else{
      TextRender TE(&RD);
      RD.SetColor(0xff000000);
      TE.BeginScope(0,0,&NormFont,&BlurFont);
      Menu->Render(&RD,&TE);
      TE.EndScope();
    };
    {
      if(RenderScene_debug)QAP_EM_LOG("before RenderText");
      RenderText(RD);
      if(RenderScene_debug)QAP_EM_LOG("after RenderText");
    }
  }
/*
  void Render()
  {
    if(!QapDX::BeginScene())return;
    QapDX::Set2D();
    //QapDX::Clear2d(1?QapColor(180,180,180):0xffc8c8c8);
    {int v=231*0+206*0+210;QapDX::Clear2d(QapColor(v,v,v));}
    RD.NextFrame();
    RenderScene();
    if(!QapDX::EndScene())return;
    QapDX::Present();
  }*/
  void RenderText(QapDev&RD)
  {
    TextRender TE(&RD);
    vec2d hs=vec2d(Sys.SM.W,Sys.SM.H)*0.5;
    real ident=24.0;
    real Y=0;
    RD.SetColor(0xff000000);
    TE.BeginScope(-hs.x+ident,+hs.y-ident,&NormFont,&BlurFont);
    {
      const string PreesR=" ^7(^3press ^2R^7)";
      const string PreesSpace=" ^7(^3press ^2Space^7)";
      const string PreesEnter=" ^7(^3press ^2Return^7)";
      string BEG="^7";
      string SEP=" ^2: ^8";
      TE.AddText("^7The ^2Game");
      TE.AddText("");
      TE.AddText("^8user_name ^2: ^7"+user_name);
      TE.AddText("");
      #define GOO(TEXT,VALUE)TE.AddText(string(BEG)+string(TEXT)+string(SEP)+string(VALUE));
      GOO("Level",string(LevelCounter)+" ["+string(LevelCounter?LevelsInfo[LevelCounter.Value].Name:"noname")+"]");
      #undef GOO
      TE.AddText("");
      if(Level.get())Level->AddText(&TE);
      TE.AddText("");
      if(WaitWin.Runned)TE.AddText("^2You win!"+PreesEnter);
      if(WaitFail.Runned)TE.AddText("^1You lose!"+PreesR);
      if(!WaitFail||!WaitWin){TE.AddText("^7game over!");}
    }
    TE.EndScope();
    if(bool need_draw_font=false){
      RD.color=0xFF000000;
      RD.BindTex(0,BlurFont.Tex);
      RD.DrawQuad(1.5,-0.5,-512,512,Pi);
      RD.color=0xFFFFFFFF;
      RD.BindTex(0,NormFont.Tex);
      RD.DrawQuad(0.5,0.5,-512,512,Pi);
    }
  }
  void Collide()
  {    
  }
  void NextLevel()
  {
    LevelCounter++;
    RestartLevel();
  }
  void OnWin(){
    //Level.reset();
  }
  void OnFail(){
    //Level.reset();
  }
  void Win(){WaitWin.Start();}
  void Fail(){WaitFail.Start();}
  void ReloadWinFail(){WaitFail.Stop();WaitWin.Stop();}
  void Update()
  {
    if(user_name_scene)return InputUserNameUpdate();
    QapAssert(Menu.get());
    if(kb.OnDown(VK_ESCAPE)){if(Menu->InGame()){Menu->Up();}else{Menu->Down();}}
    //kb.UpdateMouse();
    if(Menu->InGame())
    {
      if(Level.get())
      {
        QAP_EM_LOG("bef_update_level");
        Level->Update(this);
        QAP_EM_LOG("aft_update_level");
        {WaitFail++;WaitWin++;}
        if(!WaitWin.Runned&&!WaitFail.Runned)
        {
          bool w=Level->Win();bool f=Level->Fail();
          if(f)Fail();
          if(w&&!f)Win();
        }else{
          if(!WaitFail)OnFail();
          if(!WaitWin)OnWin();
        }
      };
      if(kb.OnDown('R')){RestartLevel();}
      if(WaitFail.Runned||WaitWin.Runned)
      {
        if(WaitWin.Runned&&kb.Down[VK_RETURN]){NextLevel();}
      }
    }else{
      Menu->Update(this);
    }
  }
  void Free()
  {
    /*FreeFont(NormFont);FreeFont(BlurFont);
    UnloadTextures();*/
  }
  template<class FUNC>
  static string wget(const string&host,const string&dir,FUNC&&cb){
    //emscripten_run_script(string("console.log('host = "+host+"');").c_str());
    static int counter=0;counter++;
    auto url=host+dir;
    auto fn=std::to_string(counter)+" "+url;
    auto&m=g_global_urls[fn];
    m.url=fn;
    m.on_load=std::move(cb);
    EM_ASM({
      fetchFile_v2(UTF8ToString($0));
    },int(fn.c_str()));
    /*DownLoader dl(host,dir,"");
    dl.port=80;
    dl.start();
    for(;dl.update();){
      Sleep(0);
    }
    auto s=dl.GetContent(dl.data,true);
    dl.stop();
    return s;*/
    return "";
  }
  static string get_host(){
    static string host="185.92.223.117";static bool need_init=true;
    if(need_init){
      //wget("adler3d.github.io","/qap_vm/trash/test2025/game_host.txt",cb);
    }
    need_init=false;
    if(host.size()&&host.back()=='\n')host.pop_back();
    return host;
  }
};
TGame Game;
void update_kb(){
  EM_ASM({update_kb($0,$1);},int(&kb.Down[0]),int(&kb.Changed[0]));
  kb.MousePos.x=+EM_ASM_INT({return g_mpos.x;})-Sys.SM.W/2;
  kb.MousePos.y=-EM_ASM_INT({return g_mpos.y;})+Sys.SM.H/2;
}
extern "C" {
  int update(int nope){
    QAP_EM_LOG("Game.RenderScene();");
    Game.RenderScene();
    QAP_EM_LOG("update_kb();");
    update_kb();
    Game.Update();
    update_kb();
    Game.Update();
    return 0;
    /*
    {
      QapDev::BatchScope Scope(qDev);
      for(auto&ex:rarr){
        qDev.color=ex.c;
        qDev.DrawQuad(ex.pos.x,ex.pos.y,ex.wh,ex.wh,ex.ang);
        ex.ang+=ex.dang;
      }
    }
    qDev.color=0xFFffFFff;
    qDev.DrawQuad(0,0,128,128,rarr.back().ang);
    qDev.color=0xFFffFFff;
    */
    qDev.color=0xFF000000;
    qDev.BindTex(0,BlurFont.Tex);
    qDev.DrawQuad(-500+1.5,-1.5,-512,512,Pi);
    qDev.BindTex(0,NormFont.Tex);
    qDev.color=0xFFffFFff;
    qDev.DrawQuad(-500+0.5,0.5,-512,512,Pi);
    /**/
    auto&RD=qDev;
    TextRender TE(&RD);
    
    vec2d hs=vec2d(1920,1024)*0.5;
    real ident=24.0;
    real Y=0;
    RD.SetColor(0xff000000);
    TE.BeginScope(-hs.x+ident,+hs.y-ident,&NormFont,&BlurFont);
    {
      const string PreesR=" ^7(^3press ^2R^7)";
      const string PreesSpace=" ^7(^3press ^2Space^7)";
      const string PreesEnter=" ^7(^3press ^2Return^7)";
      string BEG="^7";
      string SEP=" ^2: ^8";
      TE.AddText("^7The ^2Game");
      TE.AddText("");
      TE.AddText("^8user_name ^2: ^7Adler");
      TE.AddText("");
      //#define GOO(TEXT,VALUE)TE.AddText(string(BEG)+string(TEXT)+string(SEP)+string(VALUE));
      //GOO("Level",string(LevelCounter)+" ["+string(LevelCounter?LevelsInfo[LevelCounter.Value].Name:"noname")+"]");
      //#undef GOO
      //TE.AddText("");
      //if(Level.get())Level->AddText(&TE);
      //TE.AddText("");
      //if(WaitWin.Runned)TE.AddText("^2You win!"+PreesEnter);
      //if(WaitFail.Runned)TE.AddText("^1You lose!"+PreesR);
      //if(!WaitFail||!WaitWin){TE.AddText("^7game over!");}
    }
    TE.EndScope();
    //RD.BindTex(0,NormFont.Tex);
    //RD.DrawQuad(0.5,0.5,-512,512,Pi);
    return 0;
  }
}
void init(){
  qDev.Init(1024*64,1024*64*3);
  qDev.color=0xFFffFFff;
  auto*pNormMem=NormFont.CreateFontMem("Arial",14,false,512);
  auto*pBlurMem=pNormMem->Clone();
  BlurTexture(pBlurMem,4);
  BlurFont=NormFont;
  NormFont.Tex=GenTextureMipMap(pNormMem,16);
  BlurFont.Tex=GenTextureMipMap(pBlurMem,16);
  
  Game.Init();
  for(int i=0;i<5000;i++){
    rarr.push_back({});
    auto&b=rarr.back();
    b.pos=vec2d(rand()%1920-1920/2,rand()%1000-500);
    b.ang=(rand()%360)*Pi*2/360;
    b.dang=(rand()%1000-500)*0.0001;
    b.c.r=rand()%255;
    b.c.g=rand()%255;
    b.c.b=rand()%255;
    b.c.a=255;
    b.wh=(rand()%1000)*32/1000.0+16;
  }
}
extern "C" {
  int qap_main(char*phost){
    g_host=phost;
    EM_ASM({let d=document.body;d.innerHTML='<canvas id="glcanvas" width="'+window.innerWidth+'" height="'+window.innerHeight+'"></canvas>';});
    //EM_ASM({let d=document.body;d.innerHTML='<canvas id="glcanvas" width="'+d.Width+'" height="'+d.height+'"></canvas>';});
    Sys.SM.W=EM_ASM_INT({return window.innerWidth;});
    Sys.SM.H=EM_ASM_INT({return window.innerHeight;});
    srand(time(NULL));
    EM_ASM(main(););
    init();
    return 0;
  }
}
