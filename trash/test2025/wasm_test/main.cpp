#include <emscripten.h>
#include <vector>
#include <stdlib.h>
#include <time.h>

using namespace std;
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
  vec2d(const real&x,const real&y):x(x),y(y){}
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
  //QapDX::BlendType BlendMode;
  //QapDX::AlphaMode AlphaMode;
  bool Batching;
  bool Textured;
  b2Transform xf,txf;
  //static const DWORD FVF=D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1;
public:
  QapDev():color(0xFFFFFFFF),VB(NULL),IB(NULL),VBA(NULL),IBA(NULL),VPos(0),IPos(0),MaxVPos(0),MaxIPos(0),Batching(false)/*,BlendMode(QapDX::BT_SUB),AlphaMode(QapDX::AM_NONE)*/{}
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
    VB={};IB={};VPos=0;IPos=0;Batching=false;//BlendMode=QapDX::BT_SUB;AlphaMode=QapDX::AM_NONE;
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
          qDev.carr[i*4+0]=HEAPU8[g_VB+i*6*4+3*4+0]*inv_255;
          qDev.carr[i*4+1]=HEAPU8[g_VB+i*6*4+3*4+1]*inv_255;
          qDev.carr[i*4+2]=HEAPU8[g_VB+i*6*4+3*4+2]*inv_255;
          qDev.carr[i*4+3]=HEAPU8[g_VB+i*6*4+3*4+3]*inv_255;
          qDev.tarr[i*2+0]=HEAPF32[(g_VB>>2)+i*6+3+1+0];
          qDev.tarr[i*2+1]=HEAPF32[(g_VB>>2)+i*6+3+1+1];
        }
        qDev.iarr.length=g_IBN;
        for(let i=0;i<g_IBN;i++){
          qDev.iarr[i]=HEAP32[(g_VI>>2)+i];
        }
        if(qDev.gl&&qDev.prog/*&&qDev.parr_buff&&qDev.carr_buff&&qDev.tarr_buff&&qDev.iarr_buff*/)qDev.DIP(qDev);
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
  //virtual void BindTex(int Stage,QapDX::QapTex*Tex){Sys.pDev->SetTexture(Stage,Tex?Tex->Tex:NULL);txf.set_ident();}
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
  //QapDX::BlendType GetBlendMode(){return BlendMode;}
  //QapDX::AlphaMode GetAlphaMode(){return AlphaMode;}
  //void SetBlendMode(QapDX::BlendType Mode){QapDX::Blend(BlendMode=Mode);}
  //void SetAlphaMode(QapDX::AlphaMode Mode){QapDX::Alpha(AlphaMode=Mode);}
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
        int a(i),b(H-i-1);
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
    emscripten_run_script(string("g_font=initFont("+to_string(Size)+",'"+Name+"',"+to_string(TexSize)+");").c_str());
    EM_ASM({
      initFont_v2(g_font,$0);
    },int(pix));
    QapTexMem*pMem=new QapTexMem("Font_"+Name+"_"+to_string(TexSize),TexSize,TexSize,(QapColor*)pix);
    return pMem;
  }
  #endif
};
#ifdef _WIN32
QapTex*GenTextureMipMap(QapTexMem*&Tex,int MaxLevelCount)//only D3DFMT_A8R8G8B8
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
QapTex*GenTextureMipMap(QapTexMem*&Tex,int MaxLevelCount){
  int&W=Tex->W;int&H=Tex->H;QapColor*&pBits=Tex->pBits;
  auto tex=EM_ASM_INT({
    return makeTexture(qDev,$0,$1,$2);
  },W,H,int(pBits));
  auto*pTex=new QapTex(Tex,tex);
  return pTex;
}
#endif
struct t_rec{
  vec2d pos;
  double wh;
  QapColor c;
  double ang;
  double dang;
};
vector<t_rec> rarr;
QapDev qDev;
QapTex*NormFont=nullptr;
void bindTex(QapDev&qDev,int Tex){
  EM_ASM({bindTex(qDev,$0);},Tex);
}
extern "C" {
  int update(int nope){
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
    bindTex(qDev,NormFont->Tex);
    qDev.color=0xFFffFFff;
    qDev.DrawQuad(-500,0,512,512,0);
    return 0;
  }
}
extern "C" {
int qap_main(int nope) {
  EM_ASM(document.body.innerHTML='<canvas id="glcanvas" width="1920" height="1024"></canvas>V5';);
  vector<int> V={10,20,30};
  qDev.Init(1024*64,1024*64*3);
  qDev.color=0xFFffFFff;
  QapFont NF;
  auto*pTexMem=NF.CreateFontMem("Arial",14,false,512);
  NormFont=GenTextureMipMap(pTexMem,16);
  srand(time(NULL));
  {
    QapDev::BatchScope Scope(qDev);
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
      qDev.color.r=rand()%255;
      qDev.color.g=rand()%255;
      qDev.color.b=rand()%255;
      //qDev.DrawQuad(rand()%1000-500,rand()%1000-500,512,512,);
    }
  }
  EM_ASM({
    console.log('I received: ' + $0);
    g_data=$0;
    {
      let a=HEAP32[(g_data>>2)+0];
      let b=HEAP32[(g_data>>2)+1];
      let c=HEAP32[(g_data>>2)+2];
      console.log({a,b,c});
      let x=HEAP8[g_data+0];
      let y=HEAP8[g_data+4];
      let z=HEAP8[g_data+8];
      console.log({x,y,z});
    }
  }, int(V.data()));
  EM_ASM(main(););
  EM_ASM({
    g_VB=$0;g_VI=$1;g_VBN=$2;g_IBN=$3;
    //console.log({g_VB,g_VI,g_VBN,g_IBN});
    g_draw=()=>
    {
      //qDev_old=JSON.parse(JSON.stringify(qDev,0,2));
      qDev.parr.length=g_VBN*2;
      qDev.carr.length=g_VBN*4;
      qDev.tarr.length=g_VBN*2;
      let inv_255=1.0/255;
      for(let i=0;i<g_VBN;i++){
        // vec3f,uint,vec2f -> 32*3,32,32*2 -> 32*6
        qDev.parr[i*2+0]=HEAPF32[(g_VB>>2)+i*6+0];
        qDev.parr[i*2+1]=HEAPF32[(g_VB>>2)+i*6+1];
        //qDev.parr[i*3+2]=HEAPF32[(g_VB>>2)+i*6+2];
        qDev.carr[i*4+0]=HEAPU8[g_VB+i*6*4+3*4+0]*inv_255;
        qDev.carr[i*4+1]=HEAPU8[g_VB+i*6*4+3*4+1]*inv_255;
        qDev.carr[i*4+2]=HEAPU8[g_VB+i*6*4+3*4+2]*inv_255;
        qDev.carr[i*4+3]=HEAPU8[g_VB+i*6*4+3*4+3]*inv_255;
        qDev.tarr[i*2+0]=HEAPF32[(g_VB>>2)+i*6+3+1+0];
        qDev.tarr[i*2+1]=HEAPF32[(g_VB>>2)+i*6+3+1+1];
      }
      qDev.iarr.length=g_IBN;
      for(let i=0;i<g_IBN;i++){
        qDev.iarr[i]=HEAP32[(g_VI>>2)+i];
      }
    };
    g_draw();
  },int(qDev.VB.data()),int(qDev.IB.data()),qDev.VPos,qDev.IPos);
  return 0;
}
}
