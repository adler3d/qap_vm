// 1239.701965 ms
/*---
2025.12.28 20:51:12.813
вот тебе новый калькулятор(твоя задача написать интерпретатор который будет вычислять финальное состояние контекста после прогона всех статаментов):
*/
struct t_calc:public t_node_base{
  //===>>===i_term_visitor
  #define DEF_PRO_BLANK()
  #define LIST(ADDBEG,ADD,ADDEND)\
  ADDBEG()\
    ADD(t_number)\
    ADD(t_scope)\
    ADD(t_varcall)\
  ADDEND()
  class i_term;
  #define ADD(TYPE)class TYPE;
  LIST(DEF_PRO_BLANK,ADD,DEF_PRO_BLANK)
  #undef ADD
  class i_term_visitor{
  public:
    typedef t_calc::i_term i_term;
  public:
    #define ADD(TYPE)virtual void Do(TYPE&r)=0;
    LIST(DEF_PRO_BLANK,ADD,DEF_PRO_BLANK)
    #undef ADD
  public:
    #define ADD(U)typedef t_calc::U U;//typedef t_calc::U U;
    LIST(DEF_PRO_BLANK,ADD,DEF_PRO_BLANK)
    #undef ADD
  public:
    #ifdef QAP_FAST_UBERCAST
    template<class TYPE,class Visitor>
    struct Is:public Visitor{
      TYPE*ptr{};
    public:
      #define ADD(U)void Do(U&r){ptr=std::is_same<U,TYPE>::value?(TYPE*)&r:nullptr;}
      LIST(DEF_PRO_BLANK,ADD,DEF_PRO_BLANK)
      #undef ADD
    };
    template<class TYPE>
    static TYPE*UberCast(i_term*p){
      if(!p)return nullptr;Is<TYPE,i_term_visitor> IS;p->Use(IS);return IS.ptr;
    }
    #else
    template<class TYPE>
    static TYPE*UberCast(i_term*p){return dynamic_cast<TYPE*>(p);}
    #endif
  };
  //struct t_visitor:public i_term_visitor{
  //  void Do(t_number&r){}
  //  void Do(t_scope&r){}
  //  void Do(t_varcall&r){}
  //};
  #undef LIST
  #undef DEF_PRO_BLANK
  //===<<===i_term_visitor
  struct i_term:public t_node_base{
  #define DEF_PRO_STRUCT_INFO(NAME,PARENT,OWNER)NAME(i_term)OWNER(t_calc)
  #define DEF_PRO_VARIABLE(ADDBEG,ADDVAR,ADDEND)\
  ADDBEG()\
  ADDEND()
  //=====+>>>>>i_term
  #include "QapGenStructNoTemplate.inl"
  //<<<<<+=====i_term
  public:
    typedef i_term_visitor i_visitor;
    virtual void Use(i_visitor&A){QapDebugMsg("no way.");/*A.Do(*this);*/}
    virtual ~i_term()=default;
  public:
    virtual bool go(i_dev&dev){QapDebugMsg("no way.");return false;};
    struct t_poly_impl:public t_poly_tool::go_poly<SelfClass>
    {
      #include "QapLexPolyBeg.inl"
      void load();/*
      {
        F(t_number);
        F(t_scope);
        F(t_varcall);
        (void)count;(void)first_id;(void)out_arr;(void)this;
        main();
        return scope.ok;
      }*/
      #include "QapLexPolyEndNoTemplate.inl"
    };
  };
  //===>>===i_sep_visitor
  #define DEF_PRO_BLANK()
  #define LIST(ADDBEG,ADD,ADDEND)\
  ADDBEG()\
    ADD(t_sep_seq)\
    ADD(t_c_comment)\
    ADD(t_cpp_comment)\
  ADDEND()
  class i_sep;
  #define ADD(TYPE)class TYPE;
  LIST(DEF_PRO_BLANK,ADD,DEF_PRO_BLANK)
  #undef ADD
  class i_sep_visitor{
  public:
    typedef t_calc::i_sep i_sep;
  public:
    #define ADD(TYPE)virtual void Do(TYPE&r)=0;
    LIST(DEF_PRO_BLANK,ADD,DEF_PRO_BLANK)
    #undef ADD
  public:
    #define ADD(U)typedef t_calc::U U;//typedef t_calc::U U;
    LIST(DEF_PRO_BLANK,ADD,DEF_PRO_BLANK)
    #undef ADD
  public:
    #ifdef QAP_FAST_UBERCAST
    template<class TYPE,class Visitor>
    struct Is:public Visitor{
      TYPE*ptr{};
    public:
      #define ADD(U)void Do(U&r){ptr=std::is_same<U,TYPE>::value?(TYPE*)&r:nullptr;}
      LIST(DEF_PRO_BLANK,ADD,DEF_PRO_BLANK)
      #undef ADD
    };
    template<class TYPE>
    static TYPE*UberCast(i_sep*p){
      if(!p)return nullptr;Is<TYPE,i_sep_visitor> IS;p->Use(IS);return IS.ptr;
    }
    #else
    template<class TYPE>
    static TYPE*UberCast(i_sep*p){return dynamic_cast<TYPE*>(p);}
    #endif
  };
  //struct t_visitor:public i_sep_visitor{
  //  void Do(t_sep_seq&r){}
  //  void Do(t_c_comment&r){}
  //  void Do(t_cpp_comment&r){}
  //};
  #undef LIST
  #undef DEF_PRO_BLANK
  //===<<===i_sep_visitor
  struct i_sep:public t_node_base{
  #define DEF_PRO_STRUCT_INFO(NAME,PARENT,OWNER)NAME(i_sep)OWNER(t_calc)
  #define DEF_PRO_VARIABLE(ADDBEG,ADDVAR,ADDEND)\
  ADDBEG()\
  ADDEND()
  //=====+>>>>>i_sep
  #include "QapGenStructNoTemplate.inl"
  //<<<<<+=====i_sep
  public:
    typedef i_sep_visitor i_visitor;
    virtual void Use(i_visitor&A){QapDebugMsg("no way.");/*A.Do(*this);*/}
    virtual ~i_sep()=default;
  public:
    virtual bool go(i_dev&dev){QapDebugMsg("no way.");return false;};
    struct t_poly_impl:public t_poly_tool::go_poly<SelfClass>
    {
      #include "QapLexPolyBeg.inl"
      void load();/*
      {
        F(t_sep_seq);
        F(t_c_comment);
        F(t_cpp_comment);
        (void)count;(void)first_id;(void)out_arr;(void)this;
        main();
        return scope.ok;
      }*/
      #include "QapLexPolyEndNoTemplate.inl"
    };
  };
  //===>>===i_stat_visitor
  #define DEF_PRO_BLANK()
  #define LIST(ADDBEG,ADD,ADDEND)\
  ADDBEG()\
    ADD(t_block_stat)\
    ADD(t_solo_stat)\
    ADD(t_func_stat)\
    ADD(t_assign_stat)\
  ADDEND()
  class i_stat;
  #define ADD(TYPE)class TYPE;
  LIST(DEF_PRO_BLANK,ADD,DEF_PRO_BLANK)
  #undef ADD
  class i_stat_visitor{
  public:
    typedef t_calc::i_stat i_stat;
  public:
    #define ADD(TYPE)virtual void Do(TYPE&r)=0;
    LIST(DEF_PRO_BLANK,ADD,DEF_PRO_BLANK)
    #undef ADD
  public:
    #define ADD(U)typedef t_calc::U U;//typedef t_calc::U U;
    LIST(DEF_PRO_BLANK,ADD,DEF_PRO_BLANK)
    #undef ADD
  public:
    #ifdef QAP_FAST_UBERCAST
    template<class TYPE,class Visitor>
    struct Is:public Visitor{
      TYPE*ptr{};
    public:
      #define ADD(U)void Do(U&r){ptr=std::is_same<U,TYPE>::value?(TYPE*)&r:nullptr;}
      LIST(DEF_PRO_BLANK,ADD,DEF_PRO_BLANK)
      #undef ADD
    };
    template<class TYPE>
    static TYPE*UberCast(i_stat*p){
      if(!p)return nullptr;Is<TYPE,i_stat_visitor> IS;p->Use(IS);return IS.ptr;
    }
    #else
    template<class TYPE>
    static TYPE*UberCast(i_stat*p){return dynamic_cast<TYPE*>(p);}
    #endif
  };
  //struct t_visitor:public i_stat_visitor{
  //  void Do(t_block_stat&r){}
  //  void Do(t_solo_stat&r){}
  //  void Do(t_func_stat&r){}
  //  void Do(t_assign_stat&r){}
  //};
  #undef LIST
  #undef DEF_PRO_BLANK
  //===<<===i_stat_visitor
  struct i_stat:public t_node_base{
  #define DEF_PRO_STRUCT_INFO(NAME,PARENT,OWNER)NAME(i_stat)OWNER(t_calc)
  #define DEF_PRO_VARIABLE(ADDBEG,ADDVAR,ADDEND)\
  ADDBEG()\
  ADDEND()
  //=====+>>>>>i_stat
  #include "QapGenStructNoTemplate.inl"
  //<<<<<+=====i_stat
  public:
    typedef i_stat_visitor i_visitor;
    virtual void Use(i_visitor&A){QapDebugMsg("no way.");/*A.Do(*this);*/}
    virtual ~i_stat()=default;
  public:
    virtual bool go(i_dev&dev){QapDebugMsg("no way.");return false;};
    struct t_poly_impl:public t_poly_tool::go_poly<SelfClass>
    {
      #include "QapLexPolyBeg.inl"
      void load();/*
      {
        F(t_block_stat);
        F(t_solo_stat);
        F(t_func_stat);
        F(t_assign_stat);
        (void)count;(void)first_id;(void)out_arr;(void)this;
        main();
        return scope.ok;
      }*/
      #include "QapLexPolyEndNoTemplate.inl"
    };
  };
  struct t_term:public t_node_base{
  #define DEF_PRO_STRUCT_INFO(NAME,PARENT,OWNER)NAME(t_term)OWNER(t_calc)
  #define DEF_PRO_VARIABLE(ADDBEG,ADDVAR,ADDEND)\
  ADDBEG()\
  ADDVAR(TAutoPtr<i_term>,value,DEF,$,$)\
  ADDEND()
  //=====+>>>>>t_term
  #include "QapGenStructNoTemplate.inl"
  //<<<<<+=====t_term
  public:
    bool go(i_dev&dev){
      t_fallback $(dev,"t_calc::t_term",nullptr,this);
      auto&ok=$.ok;
      ok=dev.go_auto(value);$(ok,"TAutoPtr<i_term>,gen_dips(\"09@Zaz\")+\"$(_\"");
      if(!ok)return ok;
      return ok;
    }
  };
  struct t_number:public i_term{
    struct t_ext:public t_node_base{
    #define DEF_PRO_STRUCT_INFO(NAME,PARENT,OWNER)NAME(t_ext)OWNER(t_number)
    #define DEF_PRO_VARIABLE(ADDBEG,ADDVAR,ADDEND)\
    ADDBEG()\
    ADDVAR(string,v,DEF,$,$)\
    ADDEND()
    //=====+>>>>>t_ext
    #include "QapGenStructNoTemplate.inl"
    //<<<<<+=====t_ext
    public:
      bool go(i_dev&dev){
        t_fallback $(dev,"t_calc::t_number::t_ext",nullptr,this);
        auto&ok=$.ok;
        ok=dev.go_const(".");$(ok,",\".\"");
        if(!ok)return ok;
        static const auto g_static_var_1=CharMask::fromStr(gen_dips("09"));
        ok=dev.go_any(v,g_static_var_1);$(ok,",\"0123456789\"");
        if(!ok)return ok;
        return ok;
      }
    };
    struct t_impl:public t_node_base{
    #define DEF_PRO_STRUCT_INFO(NAME,PARENT,OWNER)NAME(t_impl)OWNER(t_number)
    #define DEF_PRO_VARIABLE(ADDBEG,ADDVAR,ADDEND)\
    ADDBEG()\
    ADDVAR(string,bef,DEF,$,$)\
    ADDVAR(TAutoPtr<t_ext>,ext,DEF,$,$)\
    ADDEND()
    //=====+>>>>>t_impl
    #include "QapGenStructNoTemplate.inl"
    //<<<<<+=====t_impl
    public:
      bool go(i_dev&dev){
        t_fallback $(dev,"t_calc::t_number::t_impl",nullptr,this);
        auto&ok=$.ok;
        static const auto g_static_var_0=CharMask::fromStr(gen_dips("09"));
        ok=dev.go_any(bef,g_static_var_0);$(ok,",\"0123456789\"");
        if(!ok)return ok;
        {bool ok=dev.go_auto(ext);$(ok,"TAutoPtr<t_ext>,\".\"");}
        return ok;
      }
    };
  #define DEF_PRO_NESTED(F)\
    /*<DEF_PRO_NESTED>*/\
    F(t_ext )\
    F(t_impl)\
    /*</DEF_PRO_NESTED>*/
  #define DEF_PRO_STRUCT_INFO(NAME,PARENT,OWNER)NAME(t_number)PARENT(i_term)OWNER(t_calc)
  #define DEF_PRO_VARIABLE(ADDBEG,ADDVAR,ADDEND)\
  ADDBEG()\
  ADDVAR(string,value,DEF,$,$)\
  ADDEND()
  //=====+>>>>>t_number
  #include "QapGenStructNoTemplate.inl"
  //<<<<<+=====t_number
  public:
    void Use(i_visitor&A){A.Do(*this);}
    static SelfClass*UberCast(ParentClass*ptr){return i_visitor::UberCast<SelfClass>(ptr);}
  public:
    bool go(i_dev&dev){
      t_fallback $(dev,"t_calc::t_number",nullptr,this);
      auto&ok=$.ok;
      ok=dev.go_str<t_impl>(value);$(ok,"t_impl,\"0123456789\"");
      if(!ok)return ok;
      return ok;
    }
  public:
  };
  struct t_divmul:public t_node_base{
    struct t_elem:public t_node_base{
    #define DEF_PRO_STRUCT_INFO(NAME,PARENT,OWNER)NAME(t_elem)OWNER(t_divmul)
    #define DEF_PRO_VARIABLE(ADDBEG,ADDVAR,ADDEND)\
    ADDBEG()\
    ADDVAR(string,oper,DEF,$,$)\
    ADDVAR(t_term,expr,DEF,$,$)\
    ADDEND()
    //=====+>>>>>t_elem
    #include "QapGenStructNoTemplate.inl"
    //<<<<<+=====t_elem
    public:
      bool go(i_dev&dev){
        t_fallback $(dev,"t_calc::t_divmul::t_elem",nullptr,this);
        auto&ok=$.ok;
        static const auto g_static_var_0=QapStrFinder::fromArr(split("/,*",","));
        ok=dev.go_any_str_from_vec(oper,g_static_var_0);$(ok,",\"/*\"");
        if(!ok)return ok;
        ok=dev.go_auto(expr);$(ok,"t_term,gen_dips(\"09@Zaz\")+\"$(_\"");
        if(!ok)return ok;
        return ok;
      }
    };
  #define DEF_PRO_NESTED(F)\
    /*<DEF_PRO_NESTED>*/\
    F(t_elem)\
    /*</DEF_PRO_NESTED>*/
  #define DEF_PRO_STRUCT_INFO(NAME,PARENT,OWNER)NAME(t_divmul)OWNER(t_calc)
  #define DEF_PRO_VARIABLE(ADDBEG,ADDVAR,ADDEND)\
  ADDBEG()\
  ADDVAR(t_term,first,DEF,$,$)\
  ADDVAR(vector<t_elem>,arr,DEF,$,$)\
  ADDEND()
  //=====+>>>>>t_divmul
  #include "QapGenStructNoTemplate.inl"
  //<<<<<+=====t_divmul
  public:
    bool go(i_dev&dev){
      t_fallback $(dev,"t_calc::t_divmul",nullptr,this);
      auto&ok=$.ok;
      ok=dev.go_auto(first);$(ok,"t_term,gen_dips(\"09@Zaz\")+\"$(_\"");
      if(!ok)return ok;
      {bool ok=dev.go_auto(arr);$(ok,"vector<t_elem>,\"*/\"");}
      return ok;
    }
  public:
  };
  struct t_addsub:public t_node_base{
    struct t_elem:public t_node_base{
    #define DEF_PRO_STRUCT_INFO(NAME,PARENT,OWNER)NAME(t_elem)OWNER(t_addsub)
    #define DEF_PRO_VARIABLE(ADDBEG,ADDVAR,ADDEND)\
    ADDBEG()\
    ADDVAR(string,oper,DEF,$,$)\
    ADDVAR(t_divmul,expr,DEF,$,$)\
    ADDEND()
    //=====+>>>>>t_elem
    #include "QapGenStructNoTemplate.inl"
    //<<<<<+=====t_elem
    public:
      bool go(i_dev&dev){
        t_fallback $(dev,"t_calc::t_addsub::t_elem",nullptr,this);
        auto&ok=$.ok;
        static const auto g_static_var_0=QapStrFinder::fromArr(split("+,-",","));
        ok=dev.go_any_str_from_vec(oper,g_static_var_0);$(ok,",\"+-\"");
        if(!ok)return ok;
        ok=dev.go_auto(expr);$(ok,"t_divmul,gen_dips(\"09@Zaz\")+\"$(_\"");
        if(!ok)return ok;
        return ok;
      }
    };
  #define DEF_PRO_NESTED(F)\
    /*<DEF_PRO_NESTED>*/\
    F(t_elem)\
    /*</DEF_PRO_NESTED>*/
  #define DEF_PRO_STRUCT_INFO(NAME,PARENT,OWNER)NAME(t_addsub)OWNER(t_calc)
  #define DEF_PRO_VARIABLE(ADDBEG,ADDVAR,ADDEND)\
  ADDBEG()\
  ADDVAR(t_divmul,first,DEF,$,$)\
  ADDVAR(vector<t_elem>,arr,DEF,$,$)\
  ADDEND()
  //=====+>>>>>t_addsub
  #include "QapGenStructNoTemplate.inl"
  //<<<<<+=====t_addsub
  public:
    bool go(i_dev&dev){
      t_fallback $(dev,"t_calc::t_addsub",nullptr,this);
      auto&ok=$.ok;
      ok=dev.go_auto(first);$(ok,"t_divmul,gen_dips(\"09@Zaz\")+\"$(_\"");
      if(!ok)return ok;
      {bool ok=dev.go_auto(arr);$(ok,"vector<t_elem>,\"+-\"");}
      return ok;
    }
  public:
  };
  struct t_scope:public i_term{
  #define DEF_PRO_STRUCT_INFO(NAME,PARENT,OWNER)NAME(t_scope)PARENT(i_term)OWNER(t_calc)
  #define DEF_PRO_VARIABLE(ADDBEG,ADDVAR,ADDEND)\
  ADDBEG()\
  ADDVAR(t_addsub,value,DEF,$,$)\
  ADDEND()
  //=====+>>>>>t_scope
  #include "QapGenStructNoTemplate.inl"
  //<<<<<+=====t_scope
  public:
    void Use(i_visitor&A){A.Do(*this);}
    static SelfClass*UberCast(ParentClass*ptr){return i_visitor::UberCast<SelfClass>(ptr);}
  public:
    bool go(i_dev&dev){
      t_fallback $(dev,"t_calc::t_scope",nullptr,this);
      auto&ok=$.ok;
      ok=dev.go_const("(");$(ok,",\"(\"");
      if(!ok)return ok;
      ok=dev.go_auto(value);$(ok,"t_addsub,gen_dips(\"09@Zaz\")+\"$(_\"");
      if(!ok)return ok;
      ok=dev.go_const(")");$(ok,",\")\"");
      if(!ok)return ok;
      return ok;
    }
  };
  struct t_sep_seq:public i_sep{
  #define DEF_PRO_STRUCT_INFO(NAME,PARENT,OWNER)NAME(t_sep_seq)PARENT(i_sep)OWNER(t_calc)
  #define DEF_PRO_VARIABLE(ADDBEG,ADDVAR,ADDEND)\
  ADDBEG()\
  ADDVAR(string,body,DEF,$,$)\
  ADDEND()
  //=====+>>>>>t_sep_seq
  #include "QapGenStructNoTemplate.inl"
  //<<<<<+=====t_sep_seq
  public:
    void Use(i_visitor&A){A.Do(*this);}
    static SelfClass*UberCast(ParentClass*ptr){return i_visitor::UberCast<SelfClass>(ptr);}
  public:
    bool go(i_dev&dev){
      t_fallback $(dev,"t_calc::t_sep_seq",nullptr,this);
      auto&ok=$.ok;
      static const auto g_static_var_0=CharMask::fromStr(" \t\r\n");
      ok=dev.go_any(body,g_static_var_0);$(ok,",\" \\t\\r\\n\"");
      if(!ok)return ok;
      return ok;
    }
  };
  struct t_c_comment:public i_sep{
  #define DEF_PRO_STRUCT_INFO(NAME,PARENT,OWNER)NAME(t_c_comment)PARENT(i_sep)OWNER(t_calc)
  #define DEF_PRO_VARIABLE(ADDBEG,ADDVAR,ADDEND)\
  ADDBEG()\
  ADDVAR(string,body,DEF,$,$)\
  ADDEND()
  //=====+>>>>>t_c_comment
  #include "QapGenStructNoTemplate.inl"
  //<<<<<+=====t_c_comment
  public:
    void Use(i_visitor&A){A.Do(*this);}
    static SelfClass*UberCast(ParentClass*ptr){return i_visitor::UberCast<SelfClass>(ptr);}
  public:
    bool go(i_dev&dev){
      t_fallback $(dev,"t_calc::t_c_comment",nullptr,this);
      auto&ok=$.ok;
      ok=dev.go_const("/*");$(ok,",\"/\"");
      if(!ok)return ok;
      ok=dev.go_end(body,"*/");$(ok,",\"*\"");
      if(!ok)return ok;
      return ok;
    }
  };
  struct t_cpp_comment:public i_sep{
  #define DEF_PRO_STRUCT_INFO(NAME,PARENT,OWNER)NAME(t_cpp_comment)PARENT(i_sep)OWNER(t_calc)
  #define DEF_PRO_VARIABLE(ADDBEG,ADDVAR,ADDEND)\
  ADDBEG()\
  ADDVAR(string,body,DEF,$,$)\
  ADDEND()
  //=====+>>>>>t_cpp_comment
  #include "QapGenStructNoTemplate.inl"
  //<<<<<+=====t_cpp_comment
  public:
    void Use(i_visitor&A){A.Do(*this);}
    static SelfClass*UberCast(ParentClass*ptr){return i_visitor::UberCast<SelfClass>(ptr);}
  public:
    bool go(i_dev&dev){
      t_fallback $(dev,"t_calc::t_cpp_comment",nullptr,this);
      auto&ok=$.ok;
      ok=dev.go_const("//");$(ok,",\"/\"");
      if(!ok)return ok;
      static const auto g_static_var_1=CharMask::fromStr(dip_inv("\n"));
      {bool ok=dev.go_any(body,g_static_var_1);$(ok,",gen_dips(\"\\x00\\t\\x0B\\xFF\")");}
      return ok;
    }
  };
  struct t_sep:public t_node_base{
    struct t_impl:public t_node_base{
    #define DEF_PRO_STRUCT_INFO(NAME,PARENT,OWNER)NAME(t_impl)OWNER(t_sep)
    #define DEF_PRO_VARIABLE(ADDBEG,ADDVAR,ADDEND)\
    ADDBEG()\
    ADDVAR(vector<TAutoPtr<i_sep>>,arr,DEF,$,$)\
    ADDEND()
    //=====+>>>>>t_impl
    #include "QapGenStructNoTemplate.inl"
    //<<<<<+=====t_impl
    public:
      bool go(i_dev&dev){
        t_fallback $(dev,"t_calc::t_sep::t_impl",nullptr,this);
        auto&ok=$.ok;
        ok=dev.go_auto(arr);$(ok,"vector<TAutoPtr<i_sep>>,\"\\t\\n\\r /\"");
        if(!ok)return ok;
        return ok;
      }
    };
  #define DEF_PRO_NESTED(F)\
    /*<DEF_PRO_NESTED>*/\
    F(t_impl)\
    /*</DEF_PRO_NESTED>*/
  #define DEF_PRO_STRUCT_INFO(NAME,PARENT,OWNER)NAME(t_sep)OWNER(t_calc)
  #define DEF_PRO_VARIABLE(ADDBEG,ADDVAR,ADDEND)\
  ADDBEG()\
  ADDVAR(string,value,DEF,$,$)\
  ADDEND()
  //=====+>>>>>t_sep
  #include "QapGenStructNoTemplate.inl"
  //<<<<<+=====t_sep
  public:
    bool go(i_dev&dev){
      t_fallback $(dev,"t_calc::t_sep",nullptr,this);
      auto&ok=$.ok;
      ok=dev.go_str<t_impl>(value);$(ok,"t_impl,\"\\t\\n\\r /\"");
      if(!ok)return ok;
      return ok;
    }
  public:
  };
  struct t_call_param:public t_node_base{
  #define DEF_PRO_STRUCT_INFO(NAME,PARENT,OWNER)NAME(t_call_param)OWNER(t_calc)
  #define DEF_PRO_VARIABLE(ADDBEG,ADDVAR,ADDEND)\
  ADDBEG()\
  ADDVAR(t_addsub,body,DEF,$,$)\
  ADDVAR(t_sep,$sep1,DEF,$,$)\
  ADDEND()
  //=====+>>>>>t_call_param
  #include "QapGenStructNoTemplate.inl"
  //<<<<<+=====t_call_param
  public:
    bool go(i_dev&dev){
      t_fallback $(dev,"t_calc::t_call_param",nullptr,this);
      auto&ok=$.ok;
      ok=dev.go_auto(body);$(ok,"t_addsub,gen_dips(\"09@Zaz\")+\"$(_\"");
      if(!ok)return ok;
      {bool ok=dev.go_auto($sep1);$(ok,"t_sep,\"\\t\\n\\r /\"");}
      return ok;
    }
  };
  struct t_call_params:public t_node_base{
  #define DEF_PRO_STRUCT_INFO(NAME,PARENT,OWNER)NAME(t_call_params)OWNER(t_calc)
  #define DEF_PRO_VARIABLE(ADDBEG,ADDVAR,ADDEND)\
  ADDBEG()\
  ADDVAR(t_sep,$sep1,DEF,$,$)\
  ADDVAR(vector<t_call_param>,arr,DEF,$,$)\
  ADDEND()
  //=====+>>>>>t_call_params
  #include "QapGenStructNoTemplate.inl"
  //<<<<<+=====t_call_params
  public:
    bool go(i_dev&dev){
      t_fallback $(dev,"t_calc::t_call_params",nullptr,this);
      auto&ok=$.ok;
      ok=dev.go_const("(");$(ok,",\"(\"");
      if(!ok)return ok;
      {bool ok=dev.go_auto($sep1);$(ok,"t_sep,\"\\t\\n\\r /\"");}
      {bool ok=dev.go_vec(arr,",");$(ok,"vector<t_call_param>,gen_dips(\"09@Zaz\")+\"$(_\"");}
      ok=dev.go_const(")");$(ok,",\")\"");
      if(!ok)return ok;
      return ok;
    }
  };
  struct t_varcall:public i_term{
    struct t_impl:public t_node_base{
    #define DEF_PRO_STRUCT_INFO(NAME,PARENT,OWNER)NAME(t_impl)OWNER(t_varcall)
    #define DEF_PRO_VARIABLE(ADDBEG,ADDVAR,ADDEND)\
    ADDBEG()\
    ADDVAR(char,A,DEF,$,$)\
    ADDVAR(string,B,DEF,$,$)\
    ADDEND()
    //=====+>>>>>t_impl
    #include "QapGenStructNoTemplate.inl"
    //<<<<<+=====t_impl
    public:
      bool go(i_dev&dev){
        t_fallback $(dev,"t_calc::t_varcall::t_impl",nullptr,this);
        auto&ok=$.ok;
        static const auto g_static_var_0=CharMask::fromStr(gen_dips("azAZ")+"_$@");
        ok=dev.go_any_char(A,g_static_var_0);$(ok,",gen_dips(\"@Zaz\")+\"$_\"");
        if(!ok)return ok;
        static const auto g_static_var_1=CharMask::fromStr(gen_dips("azAZ09")+"_$@");
        {bool ok=dev.go_any(B,g_static_var_1);$(ok,",gen_dips(\"09@Zaz\")+\"$_\"");}
        return ok;
      }
    };
  #define DEF_PRO_NESTED(F)\
    /*<DEF_PRO_NESTED>*/\
    F(t_impl)\
    /*</DEF_PRO_NESTED>*/
  #define DEF_PRO_STRUCT_INFO(NAME,PARENT,OWNER)NAME(t_varcall)PARENT(i_term)OWNER(t_calc)
  #define DEF_PRO_VARIABLE(ADDBEG,ADDVAR,ADDEND)\
  ADDBEG()\
  ADDVAR(string,name,DEF,$,$)\
  ADDVAR(t_sep,$sep1,DEF,$,$)\
  ADDVAR(TAutoPtr<t_call_params>,params,DEF,$,$)\
  ADDVAR(t_sep,$sep3,DEF,$,$)\
  ADDEND()
  //=====+>>>>>t_varcall
  #include "QapGenStructNoTemplate.inl"
  //<<<<<+=====t_varcall
  public:
    void Use(i_visitor&A){A.Do(*this);}
    static SelfClass*UberCast(ParentClass*ptr){return i_visitor::UberCast<SelfClass>(ptr);}
  public:
    bool go(i_dev&dev){
      t_fallback $(dev,"t_calc::t_varcall",nullptr,this);
      auto&ok=$.ok;
      ok=dev.go_str<t_impl>(name);$(ok,"t_impl,gen_dips(\"@Zaz\")+\"$_\"");
      if(!ok)return ok;
      {bool ok=dev.go_auto($sep1);$(ok,"t_sep,\"\\t\\n\\r /\"");}
      {bool ok=dev.go_auto(params);$(ok,"TAutoPtr<t_call_params>,\"(\"");}
      {bool ok=dev.go_auto($sep3);$(ok,"t_sep,\"\\t\\n\\r /\"");}
      return ok;
    }
  public:
  };
  struct t_block_stat:public i_stat{
  #define DEF_PRO_STRUCT_INFO(NAME,PARENT,OWNER)NAME(t_block_stat)PARENT(i_stat)OWNER(t_calc)
  #define DEF_PRO_VARIABLE(ADDBEG,ADDVAR,ADDEND)\
  ADDBEG()\
  ADDVAR(t_sep,$sep1,DEF,$,$)\
  ADDVAR(vector<TAutoPtr<i_stat>>,arr,DEF,$,$)\
  ADDVAR(t_sep,$sep4,DEF,$,$)\
  ADDEND()
  //=====+>>>>>t_block_stat
  #include "QapGenStructNoTemplate.inl"
  //<<<<<+=====t_block_stat
  public:
    void Use(i_visitor&A){A.Do(*this);}
    static SelfClass*UberCast(ParentClass*ptr){return i_visitor::UberCast<SelfClass>(ptr);}
  public:
    bool go(i_dev&dev){
      t_fallback $(dev,"t_calc::t_block_stat",nullptr,this);
      auto&ok=$.ok;
      ok=dev.go_const("{");$(ok,",\"{\"");
      if(!ok)return ok;
      {bool ok=dev.go_auto($sep1);$(ok,"t_sep,\"\\t\\n\\r /\"");}
      ok=dev.go_auto(arr);$(ok,"vector<TAutoPtr<i_stat>>,gen_dips(\"09@Za{\")+\"$(_\"");
      if(!ok)return ok;
      ok=dev.go_const("}");$(ok,",\"}\"");
      if(!ok)return ok;
      {bool ok=dev.go_auto($sep4);$(ok,"t_sep,\"\\t\\n\\r /\"");}
      return ok;
    }
  };
  struct t_solo_stat:public i_stat{
  #define DEF_PRO_STRUCT_INFO(NAME,PARENT,OWNER)NAME(t_solo_stat)PARENT(i_stat)OWNER(t_calc)
  #define DEF_PRO_VARIABLE(ADDBEG,ADDVAR,ADDEND)\
  ADDBEG()\
  ADDVAR(t_addsub,expr,DEF,$,$)\
  ADDVAR(t_sep,$sep2,DEF,$,$)\
  ADDEND()
  //=====+>>>>>t_solo_stat
  #include "QapGenStructNoTemplate.inl"
  //<<<<<+=====t_solo_stat
  public:
    void Use(i_visitor&A){A.Do(*this);}
    static SelfClass*UberCast(ParentClass*ptr){return i_visitor::UberCast<SelfClass>(ptr);}
  public:
    bool go(i_dev&dev){
      t_fallback $(dev,"t_calc::t_solo_stat",nullptr,this);
      auto&ok=$.ok;
      ok=dev.go_auto(expr);$(ok,"t_addsub,gen_dips(\"09@Zaz\")+\"$(_\"");
      if(!ok)return ok;
      ok=dev.go_const(";");$(ok,",\";\"");
      if(!ok)return ok;
      {bool ok=dev.go_auto($sep2);$(ok,"t_sep,\"\\t\\n\\r /\"");}
      return ok;
    }
  };
  struct t_func_stat:public i_stat{
    struct t_arg:public t_node_base{
    #define DEF_PRO_STRUCT_INFO(NAME,PARENT,OWNER)NAME(t_arg)OWNER(t_func_stat)
    #define DEF_PRO_VARIABLE(ADDBEG,ADDVAR,ADDEND)\
    ADDBEG()\
    ADDVAR(t_sep,$sep0,DEF,$,$)\
    ADDVAR(string,name,DEF,$,$)\
    ADDVAR(t_sep,$sep2,DEF,$,$)\
    ADDEND()
    //=====+>>>>>t_arg
    #include "QapGenStructNoTemplate.inl"
    //<<<<<+=====t_arg
    public:
      bool go(i_dev&dev){
        t_fallback $(dev,"t_calc::t_func_stat::t_arg",nullptr,this);
        auto&ok=$.ok;
        {bool ok=dev.go_auto($sep0);$(ok,"t_sep,\"\\t\\n\\r /\"");}
        ok=dev.go_str<t_varcall::t_impl>(name);$(ok,"t_varcall::t_impl,gen_dips(\"@Zaz\")+\"$_\"");
        if(!ok)return ok;
        {bool ok=dev.go_auto($sep2);$(ok,"t_sep,\"\\t\\n\\r /\"");}
        return ok;
      }
    };
  #define DEF_PRO_NESTED(F)\
    /*<DEF_PRO_NESTED>*/\
    F(t_arg)\
    /*</DEF_PRO_NESTED>*/
  #define DEF_PRO_STRUCT_INFO(NAME,PARENT,OWNER)NAME(t_func_stat)PARENT(i_stat)OWNER(t_calc)
  #define DEF_PRO_VARIABLE(ADDBEG,ADDVAR,ADDEND)\
  ADDBEG()\
  ADDVAR(string,func,DEF,$,$)\
  ADDVAR(t_sep,$sep1,DEF,$,$)\
  ADDVAR(t_sep,$sep3,DEF,$,$)\
  ADDVAR(vector<t_arg>,args,DEF,$,$)\
  ADDVAR(t_sep,$sep6,DEF,$,$)\
  ADDVAR(t_sep,$sep8,DEF,$,$)\
  ADDVAR(TAutoPtr<i_stat>,body,DEF,$,$)\
  ADDEND()
  //=====+>>>>>t_func_stat
  #include "QapGenStructNoTemplate.inl"
  //<<<<<+=====t_func_stat
  public:
    void Use(i_visitor&A){A.Do(*this);}
    static SelfClass*UberCast(ParentClass*ptr){return i_visitor::UberCast<SelfClass>(ptr);}
  public:
    bool go(i_dev&dev){
      t_fallback $(dev,"t_calc::t_func_stat",nullptr,this);
      auto&ok=$.ok;
      ok=dev.go_str<t_varcall::t_impl>(func);$(ok,"t_varcall::t_impl,gen_dips(\"@Zaz\")+\"$_\"");
      if(!ok)return ok;
      {bool ok=dev.go_auto($sep1);$(ok,"t_sep,\"\\t\\n\\r /\"");}
      ok=dev.go_const("(");$(ok,",\"(\"");
      if(!ok)return ok;
      {bool ok=dev.go_auto($sep3);$(ok,"t_sep,\"\\t\\n\\r /\"");}
      ok=dev.go_vec(args,",");$(ok,"vector<t_arg>,gen_dips(\"\\t\\n@Zaz\")+\"\\r $/_\"");
      if(!ok)return ok;
      ok=dev.go_const(")");$(ok,",\")\"");
      if(!ok)return ok;
      {bool ok=dev.go_auto($sep6);$(ok,"t_sep,\"\\t\\n\\r /\"");}
      ok=dev.go_const("=");$(ok,",\"=\"");
      if(!ok)return ok;
      {bool ok=dev.go_auto($sep8);$(ok,"t_sep,\"\\t\\n\\r /\"");}
      ok=dev.go_auto(body);$(ok,"TAutoPtr<i_stat>,gen_dips(\"09@Za{\")+\"$(_\"");
      if(!ok)return ok;
      return ok;
    }
  public:
  };
  struct t_assign_stat:public i_stat{
  #define DEF_PRO_STRUCT_INFO(NAME,PARENT,OWNER)NAME(t_assign_stat)PARENT(i_stat)OWNER(t_calc)
  #define DEF_PRO_VARIABLE(ADDBEG,ADDVAR,ADDEND)\
  ADDBEG()\
  ADDVAR(string,var,DEF,$,$)\
  ADDVAR(t_sep,$sep1,DEF,$,$)\
  ADDVAR(t_sep,$sep3,DEF,$,$)\
  ADDVAR(t_addsub,expr,DEF,$,$)\
  ADDVAR(t_sep,$sep6,DEF,$,$)\
  ADDEND()
  //=====+>>>>>t_assign_stat
  #include "QapGenStructNoTemplate.inl"
  //<<<<<+=====t_assign_stat
  public:
    void Use(i_visitor&A){A.Do(*this);}
    static SelfClass*UberCast(ParentClass*ptr){return i_visitor::UberCast<SelfClass>(ptr);}
  public:
    bool go(i_dev&dev){
      t_fallback $(dev,"t_calc::t_assign_stat",nullptr,this);
      auto&ok=$.ok;
      ok=dev.go_str<t_varcall::t_impl>(var);$(ok,"t_varcall::t_impl,gen_dips(\"@Zaz\")+\"$_\"");
      if(!ok)return ok;
      {bool ok=dev.go_auto($sep1);$(ok,"t_sep,\"\\t\\n\\r /\"");}
      ok=dev.go_const("=");$(ok,",\"=\"");
      if(!ok)return ok;
      {bool ok=dev.go_auto($sep3);$(ok,"t_sep,\"\\t\\n\\r /\"");}
      ok=dev.go_auto(expr);$(ok,"t_addsub,gen_dips(\"09@Zaz\")+\"$(_\"");
      if(!ok)return ok;
      ok=dev.go_const(";");$(ok,",\";\"");
      if(!ok)return ok;
      {bool ok=dev.go_auto($sep6);$(ok,"t_sep,\"\\t\\n\\r /\"");}
      return ok;
    }
  };
#define DEF_PRO_NESTED(F)\
  /*<DEF_PRO_NESTED>*/\
  F(t_term       )\
  F(t_number     )\
  F(t_divmul     )\
  F(t_addsub     )\
  F(t_scope      )\
  F(t_sep_seq    )\
  F(t_c_comment  )\
  F(t_cpp_comment)\
  F(t_sep        )\
  F(t_call_param )\
  F(t_call_params)\
  F(t_varcall    )\
  F(t_block_stat )\
  F(t_solo_stat  )\
  F(t_func_stat  )\
  F(t_assign_stat)\
  /*</DEF_PRO_NESTED>*/
#define DEF_PRO_STRUCT_INFO(NAME,PARENT,OWNER)NAME(t_calc)
#define DEF_PRO_VARIABLE(ADDBEG,ADDVAR,ADDEND)\
ADDBEG()\
ADDVAR(t_sep,$sep0,DEF,$,$)\
ADDVAR(vector<TAutoPtr<i_stat>>,arr,DEF,$,$)\
ADDEND()
//=====+>>>>>t_calc
#include "QapGenStructNoTemplate.inl"
//<<<<<+=====t_calc
public:
  bool go(i_dev&dev){
    t_fallback $(dev,"t_calc",nullptr,this);
    auto&ok=$.ok;
    {bool ok=dev.go_auto($sep0);$(ok,"t_sep,\"\\t\\n\\r /\"");}
    ok=dev.go_auto(arr);$(ok,"vector<TAutoPtr<i_stat>>,gen_dips(\"09@Za{\")+\"$(_\"");
    if(!ok)return ok;
    return ok;
  }
public:
};
/*
firealgo(a,start_node)={
  next=vec(int);cur=vec(int);V=vec(int);resize(V,size(a),0);
  push_back(cur,start_node);vec_at_set(V,start_node,1);
  {
    iter=2;
    while(size(cur)){
      i=0;n=size(cur);
      while(i<n){
        f=cur(i);i++;
        ja=a(f);
        j=0;jn=size(ja);
        while(j<jn){
          nb=ja(j);j++;
          v=V(nb);
          if(v)continue;
          vec_at_set(V,nb,iter);
          push_back(next,nb);
        }
      }
      cur=next;clear(next);
      iter++;
    }
  }
  result=V;
}
---
2025.12.29 10:00:36.880
---
2025.12.29 10:10:22.792
*/
void t_calc::i_term::t_poly_impl::load()
{
  #define F(TYPE,MASK)t_lex{#TYPE,[](t_poly_impl*self){self->go_for<TYPE>();},CharMask::fromStr(MASK,true)}
  static std::array<t_lex,3> lex={
    F(t_number,"0123456789"),
    F(t_scope,"("),
    F(t_varcall,gen_dips("@Zaz")+"$_")
  };
  #undef F
  #include "poly_fast_impl.inl"
  main(&lex);
  return;
}
void t_calc::i_sep::t_poly_impl::load()
{
  #define F(TYPE,MASK)t_lex{#TYPE,[](t_poly_impl*self){self->go_for<TYPE>();},CharMask::fromStr(MASK,true)}
  static std::array<t_lex,3> lex={
    F(t_sep_seq,"\t\n\r "),
    F(t_c_comment,"/"),
    F(t_cpp_comment,"/")
  };
  #undef F
  #include "poly_fast_impl.inl"
  main(&lex);
  return;
}
void t_calc::i_stat::t_poly_impl::load()
{
  #define F(TYPE,MASK)t_lex{#TYPE,[](t_poly_impl*self){self->go_for<TYPE>();},CharMask::fromStr(MASK,true)}
  static std::array<t_lex,4> lex={
    F(t_block_stat,"{"),
    F(t_solo_stat,gen_dips("09@Zaz")+"$(_"),
    F(t_func_stat,gen_dips("@Zaz")+"$_"),
    F(t_assign_stat,gen_dips("@Zaz")+"$_")
  };
  #undef F
  #include "poly_fast_impl.inl"
  main(&lex);
  return;
}
/*
//list of types:
F(t_calc)
//app:
adler3d.github.io/test2013/
//code:
return decodeURIComponent(POST['data'].split("\n").join(""));
//data:
%2f%2a%2d%2d%2d%0a2025%2e12%2e28%2020%3a51%3a12%2e813%0a%e2%ee%f2%20%f2%e5%e1%e5
%20%ed%ee%e2%fb%e9%20%ea%e0%eb%fc%ea%f3%eb%ff%f2%ee%f0%28%f2%e2%ee%ff%20%e7%e0%e
4%e0%f7%e0%20%ed%e0%ef%e8%f1%e0%f2%fc%20%e8%ed%f2%e5%f0%ef%f0%e5%f2%e0%f2%ee%f0%
20%ea%ee%f2%ee%f0%fb%e9%20%e1%f3%e4%e5%f2%20%e2%fb%f7%e8%f1%eb%ff%f2%fc%20%f4%e8
%ed%e0%eb%fc%ed%ee%e5%20%f1%ee%f1%f2%ee%ff%ed%e8%e5%20%ea%ee%ed%f2%e5%ea%f1%f2%e
0%20%ef%ee%f1%eb%e5%20%ef%f0%ee%e3%ee%ed%e0%20%e2%f1%e5%f5%20%f1%f2%e0%f2%e0%ec%
e5%ed%f2%ee%e2%29%3a%0a%2a%2f%0at%5fcalc%7b%0a%20%20t%5fterm%7b%0a%20%20%20%20TA
utoPtr%3ci%5fterm%3e%20value%3b%0a%20%20%7d%0a%20%20t%5fnumber%3ai%5fterm%7b%0a%
20%20%20%20t%5fext%7b%0a%20%20%20%20%20%20%22%2e%22%0a%20%20%20%20%20%20string%2
0v%3dany%28gen%5fdips%28%2209%22%29%29%3b%0a%20%20%20%20%7d%0a%20%20%20%20t%5fim
pl%7b%0a%20%20%20%20%20%20string%20bef%3dany%28gen%5fdips%28%2209%22%29%29%3b%0a
%20%20%20%20%20%20TAutoPtr%3ct%5fext%3e%20ext%3f%3b%0a%20%20%20%20%7d%0a%20%20%2
0%20string%20value%3dstr%3ct%5fimpl%3e%28%29%3b%0a%20%20%7d%0a%20%20t%5fdivmul%7
b%0a%20%20%20%20t%5felem%7b%0a%20%20%20%20%20%20string%20oper%3dany%5fstr%5ffrom
%5fvec%28split%28%22%2f%2c%2a%22%2c%22%2c%22%29%29%3b%0a%20%20%20%20%20%20t%5fte
rm%20expr%3b%0a%20%20%20%20%7d%0a%20%20%20%20t%5fterm%20first%3b%0a%20%20%20%20v
ector%3ct%5felem%3e%20arr%3f%3b%0a%20%20%7d%0a%20%20t%5faddsub%7b%0a%20%20%20%20
t%5felem%7b%0a%20%20%20%20%20%20string%20oper%3dany%5fstr%5ffrom%5fvec%28split%2
8%22%2b%2c%2d%22%2c%22%2c%22%29%29%3b%0a%20%20%20%20%20%20t%5fdivmul%20expr%3b%0
a%20%20%20%20%7d%0a%20%20%20%20t%5fdivmul%20first%3b%0a%20%20%20%20vector%3ct%5f
elem%3e%20arr%3f%3b%0a%20%20%7d%0a%20%20t%5fscope%3ai%5fterm%7b%0a%20%20%20%20%2
2%28%22%0a%20%20%20%20t%5faddsub%20value%3b%0a%20%20%20%20%22%29%22%0a%20%20%7d%
0a%20%20t%5fsep%5fseq%3ai%5fsep%7b%0a%20%20%20%20string%20body%3dany%28%22%20%5c
t%5cr%5cn%22%29%3b%0a%20%20%7d%0a%20%20t%5fc%5fcomment%3ai%5fsep%7b%0a%20%20%20%
20%22%2f%2a%22%0a%20%20%20%20string%20body%3dend%28%22%2a%2f%22%29%3b%0a%20%20%7
d%0a%20%20t%5fcpp%5fcomment%3ai%5fsep%7b%0a%20%20%20%20%22%2f%2f%22%0a%20%20%20%
20string%20body%3dany%28dip%5finv%28%22%5cn%22%29%29%3f%3b%0a%20%20%7d%0a%20%20t
%5fsep%7b%0a%20%20%20%20t%5fimpl%7b%0a%20%20%20%20%20%20vector%3cTAutoPtr%3ci%5f
sep%3e%3e%20arr%3b%0a%20%20%20%20%7d%0a%20%20%20%20string%20value%3dstr%3ct%5fim
pl%3e%28%29%3b%0a%20%20%7d%0a%20%20using%20%22%20%22%20as%20t%5fsep%3b%0a%20%20t
%5fcall%5fparam%7b%0a%20%20%20%20t%5faddsub%20body%3b%0a%20%20%20%20%22%20%22%3f
%0a%20%20%7d%0a%20%20t%5fcall%5fparams%7b%0a%20%20%20%20%22%28%22%0a%20%20%20%20
%22%20%22%3f%0a%20%20%20%20vector%3ct%5fcall%5fparam%3e%20arr%3dvec%28%22%2c%22%
29%3f%3b%0a%20%20%20%20%22%29%22%0a%20%20%7d%0a%20%20t%5fvarcall%3ai%5fterm%7b%0
a%20%20%20%20t%5fimpl%7b%0a%20%20%20%20%20%20char%20A%3dany%5fchar%28gen%5fdips%
28%22azAZ%22%29%2b%22%5f%24%40%22%29%3b%0a%20%20%20%20%20%20string%20B%3dany%28g
en%5fdips%28%22azAZ09%22%29%2b%22%5f%24%40%22%29%3f%3b%0a%20%20%20%20%7d%0a%20%2
0%20%20string%20name%3dstr%3ct%5fimpl%3e%28%29%3b%0a%20%20%20%20%22%20%22%3f%0a%
20%20%20%20TAutoPtr%3ct%5fcall%5fparams%3e%20params%3f%3b%0a%20%20%20%20%22%20%2
2%3f%0a%20%20%7d%0a%20%20t%5fblock%5fstat%3ai%5fstat%7b%0a%20%20%20%20%22%7b%22%
0a%20%20%20%20%22%20%22%3f%0a%20%20%20%20vector%3cTAutoPtr%3ci%5fstat%3e%3e%20ar
r%3b%0a%20%20%20%20%22%7d%22%0a%20%20%20%20%22%20%22%3f%0a%20%20%7d%0a%20%20t%5f
solo%5fstat%3ai%5fstat%7b%0a%20%20%20%20t%5faddsub%20expr%3b%0a%20%20%20%20%22%3
b%22%0a%20%20%20%20%22%20%22%3f%0a%20%20%7d%0a%20%20t%5ffunc%5fstat%3ai%5fstat%7
b%0a%20%20%20%20t%5farg%7b%0a%20%20%20%20%20%20%22%20%22%3f%0a%20%20%20%20%20%20
string%20name%3dstr%3ct%5fvarcall%3a%3at%5fimpl%3e%28%29%3b%0a%20%20%20%20%20%20
%22%20%22%3f%0a%20%20%20%20%7d%0a%20%20%20%20string%20func%3dstr%3ct%5fvarcall%3
a%3at%5fimpl%3e%28%29%3b%0a%20%20%20%20%22%20%22%3f%0a%20%20%20%20%22%28%22%20%2
2%20%22%3f%0a%20%20%20%20vector%3ct%5farg%3e%20args%3dvec%28%22%2c%22%29%3b%20%0
a%20%20%20%20%22%29%22%0a%20%20%20%20%22%20%22%3f%0a%20%20%20%20%22%3d%22%0a%20%
20%20%20%22%20%22%3f%0a%20%20%20%20TAutoPtr%3ci%5fstat%3e%20body%3b%0a%20%20%7d%
0a%20%20t%5fassign%5fstat%3ai%5fstat%7b%0a%20%20%20%20string%20var%3dstr%3ct%5fv
arcall%3a%3at%5fimpl%3e%28%29%3b%0a%20%20%20%20%22%20%22%3f%0a%20%20%20%20%22%3d
%22%0a%20%20%20%20%22%20%22%3f%0a%20%20%20%20t%5faddsub%20expr%3b%0a%20%20%20%20
%22%3b%22%0a%20%20%20%20%22%20%22%3f%0a%20%20%7d%0a%20%20%22%20%22%3f%0a%20%20ve
ctor%3cTAutoPtr%3ci%5fstat%3e%3e%20arr%3b%0a%7d%0a%2f%2a%0afirealgo%28a%2cstart%
5fnode%29%3d%7b%0a%20%20next%3dvec%28int%29%3bcur%3dvec%28int%29%3bV%3dvec%28int
%29%3bresize%28V%2csize%28a%29%2c0%29%3b%0a%20%20push%5fback%28cur%2cstart%5fnod
e%29%3bvec%5fat%5fset%28V%2cstart%5fnode%2c1%29%3b%0a%20%20%7b%0a%20%20%20%20ite
r%3d2%3b%0a%20%20%20%20while%28size%28cur%29%29%7b%0a%20%20%20%20%20%20i%3d0%3bn
%3dsize%28cur%29%3b%0a%20%20%20%20%20%20while%28i%3cn%29%7b%0a%20%20%20%20%20%20
%20%20f%3dcur%28i%29%3bi%2b%2b%3b%0a%20%20%20%20%20%20%20%20ja%3da%28f%29%3b%0a%
20%20%20%20%20%20%20%20j%3d0%3bjn%3dsize%28ja%29%3b%0a%20%20%20%20%20%20%20%20wh
ile%28j%3cjn%29%7b%0a%20%20%20%20%20%20%20%20%20%20nb%3dja%28j%29%3bj%2b%2b%3b%0
a%20%20%20%20%20%20%20%20%20%20v%3dV%28nb%29%3b%0a%20%20%20%20%20%20%20%20%20%20
if%28v%29continue%3b%0a%20%20%20%20%20%20%20%20%20%20vec%5fat%5fset%28V%2cnb%2ci
ter%29%3b%0a%20%20%20%20%20%20%20%20%20%20push%5fback%28next%2cnb%29%3b%0a%20%20
%20%20%20%20%20%20%7d%0a%20%20%20%20%20%20%7d%0a%20%20%20%20%20%20cur%3dnext%3bc
lear%28next%29%3b%0a%20%20%20%20%20%20iter%2b%2b%3b%0a%20%20%20%20%7d%0a%20%20%7
d%0a%20%20result%3dV%3b%0a%7d%0a%2d%2d%2d%0a2025%2e12%2e29%2010%3a00%3a36%2e880%
0a%2d%2d%2d%0a2025%2e12%2e29%2010%3a10%3a22%2e792%0a%2a%2f
*/