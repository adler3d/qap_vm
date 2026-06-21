namespace t_calc_test_solo{
//#include "t_calc_impl.hpp"
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
  t_calc tar;string errmsg;
  string main(const string&data,bool verbose=false){
    QapClock clock;
    scopes.reserve(4096);
    scopes.push_back({});
    auto&ctx=scopes.back();
    if(1){//t_scope ctx;
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
    {
      auto&s=ctx.funcs["dist"];s.args={"x","y"};
      struct t_dist:t_function::t_sys_func{double main(const vector<t_value>&args)override{return sqrt(args[0].num*args[0].num+args[1].num*args[1].num);}};
      static t_dist f;s.pfunc=&f;
    }
    {
      auto&s=ctx.funcs["pow"];s.args={"x","y"};
      struct t_pow:t_function::t_sys_func{double main(const vector<t_value>&args)override{return pow(args[0].num,args[1].num);}};
      static t_pow f;s.pfunc=&f;
    }
    {
      auto&s=ctx.funcs["ifgtone"];s.args={"x","y"};
      struct t_ifgtone:t_function::t_sys_func{double main(const vector<t_value>&args)override{return args[0].num>1?args[1].num:args[0].num;}};
      static t_ifgtone f;s.pfunc=&f;
    }
    {
      auto&s=ctx.funcs["ceil"];s.args={"x"};
      struct t_ceil:t_function::t_sys_func{double main(const vector<t_value>&args)override{return ceil(args[0].num);}};
      static t_ceil f;s.pfunc=&f;
    }
    {
      auto&s=ctx.funcs["exp"];s.args={"x"};
      struct t_exp:t_function::t_sys_func{double main(const vector<t_value>&args)override{return exp(args[0].num);}};
      static t_exp f;s.pfunc=&f;
    }
    {
      auto&s=ctx.funcs["log"];s.args={"x"};
      struct t_log:t_function::t_sys_func{double main(const vector<t_value>&args)override{return log(args[0].num);}};
      static t_log f;s.pfunc=&f;
    }
    {
      auto&s=ctx.funcs["tan"];s.args={"x"};
      struct t_tan:t_function::t_sys_func{double main(const vector<t_value>&args)override{return tan(args[0].num);}};
      static t_tan f;s.pfunc=&f;
    }
    }
    //noise(x,y)=sin(x*127.1+y*311.7)*43758.5453-floor(sin(x*127.1+y*311.7)*43758.5453);
    auto r=load_obj_full(tar,data,true,&errmsg);
    parse_ms=clock.MS();
    if(verbose)std::cerr<<"{\"parse_ms\":"<<parse_ms<<"}"<<endl;
    if(!r.ok){QapDebugMsg(r.msg);return {};}
    Do(tar);
    auto mstar=clock.MS();if(verbose)std::cerr<<"{\"Do(tar) done in \":"<<(mstar-parse_ms)<<"}"<<endl;
    return {};
  }
  double run(double x,double y,double t){
    auto&ctx=scopes.back();
    //auto&rbody=*ctx.funcs["r"].body;
    auto&gbody=*ctx.funcs["g"].body;
    //auto&bbody=*ctx.funcs["b"].body;
    auto&Y=ctx.vars["y"];auto&X=ctx.vars["x"];auto&T=ctx.vars["t"];T.num=t;
    Y.num=y;
    X.num=x;
    Do(gbody);
    auto v=int(last.num*255);
    return (v>255?v%256:(v<0?ceil(-v)+v:v))/255.0;
  }
};
};
