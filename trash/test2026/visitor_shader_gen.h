/*---
2026.06.19 12:27:11.938
*/
/*struct t_ast2shader : t_calc::i_term::i_visitor, t_calc::i_stat::i_visitor {
  struct t_shader_function {
    string name;
    vector<string> args;
    string body;
    int next_slot = 0;
    bool compiled = false;
    t_shader_function* parent = nullptr;
    map<string,unique_ptr<t_shader_function>> funcs;
    unordered_map<string, int> slots;
    
    int get_slot(const string& name) {
      auto it = slots.find(name);
      if (it != slots.end()) return it->second;
      return slots[name] = next_slot++;
    }
    
    string get_var_name(const string& name) {
      return "var" + to_string(get_slot(name));
    }
  };
  
  vector<t_shader_function*> all_funcs;
  t_shader_function* cur_func = nullptr;
  vector<t_scope> scopes;
  stringstream shader_code;
  int indent_level = 0;
  bool is_root_scope = true;
  
  // Сборка имени функции с учетом пространства имен
  string get_full_func_name(const string& name) {
    if (name == "rgb" || name == "r" || name == "g" || name == "b") {
      return name; // корневые функции
    }
    string full_name = name;
    auto* parent = cur_func->parent;
    while (parent && parent->name != "<root>") {
      full_name = parent->name + "_" + full_name;
      parent = parent->parent;
    }
    return full_name;
  }
  
  // Форматирование чисел для шейдера (добавляем .0 к целым)
  string format_number(double val) {
    stringstream ss;
    if (fmod(val, 1.0) == 0.0) {
      ss << std::fixed << std::setprecision(1) << val;
    } else {
      ss << std::setprecision(8) << val;
    }
    return ss.str();
  }
  
  string format_number(const string& s) {
    double val = atof(s.c_str());
    return format_number(val);
  }
  
  // Индентация
  string indent() {
    return string(indent_level * 4, ' ');
  }
  
  void add_line(const string& line) {
    shader_code << indent() << line << "\n";
  }
  
  template<class TYPE> void Do(TYPE& r) { r.Use(*this); }
  template<class TYPE> void Do(vector<TYPE>& arr) { for(auto& ex : arr) Do(&ex); }
  template<class TYPE> void Do(vector<TAutoPtr<TYPE>>& arr) { for(auto& ex : arr) if(ex) Do(*ex.get()); }
  template<class TYPE> void Do(TAutoPtr<TYPE>& r) { if(r) Do(*r.get()); }
  
  // --- Обработка терминов ---
  
  void Do(t_calc::t_term& r) {
    Do(r.value);
  }
  
  void Do(t_calc::t_number& r) {
    shader_code << format_number(r.value);
  }
  
  void Do(t_calc::t_scope& r) {
    shader_code << "(";
    Do(r.value);
    shader_code << ")";
  }
  
  void Do(t_calc::t_divmul::t_elem& r) {
    string op = (r.oper == "*") ? " * " : " / ";
    shader_code << op;// << "(";
    Do(r.expr);
    //shader_code << ")";
  }
  
  void Do(t_calc::t_divmul& r) {
    Do(r.first);
    for(auto& e : r.arr) {
      Do(e);
    }
  }
  
  void Do(t_calc::t_addsub::t_elem& r) {
    string op = (r.oper == "+") ? " + " : " - ";
    shader_code << op;// << "(";
    Do(r.expr);
    //shader_code; << ")";
  }
  
  void Do(t_calc::t_addsub& r) {
    Do(r.first);
    for(auto& e : r.arr) {
      Do(e);
    }
  }
  
  // --- Обработка вызовов функций и переменных ---
  
  void Do(t_calc::t_varcall& r) {
    const string& name = r.name;
    
    if (!r.params) {
      // Переменная
      //int slot = cur_func->get_slot(name);
      shader_code << name;
      return;
    }
    
    // Вызов функции
    string func_name = get_full_func_name(name);
    
    // Проверяем, не встроенная ли это функция
    if (is_builtin_function(name)) {
      shader_code << name << "(";
      for (size_t i = 0; i < r.params->arr.size(); i++) {
        if (i > 0) shader_code << ", ";
        Do(r.params->arr[i].body);
      }
      shader_code << ")";
    } else {
      // Пользовательская функция
      shader_code << func_name << "(";
      for (size_t i = 0; i < r.params->arr.size(); i++) {
        if (i > 0) shader_code << ", ";
        Do(r.params->arr[i].body);
      }
      shader_code << ")";
    }
  }
  
  bool is_builtin_function(const string& name) {
    static unordered_set<string> builtins = {
      "sin", "cos", "tan", "asin", "acos", "atan", "atan2", "sqr", "dist",
      "sqrt", "pow", "exp", "log", "log2",
      "abs", "sign", "floor", "ceil", "fract",
      "min", "max", "clamp", "mix", "step", "smoothstep",
      "length", "distance", "dot", "cross", "normalize",
      "reflect", "refract"
    };
    return builtins.find(name) != builtins.end();
  }
  
  unordered_set<string> declared_vars;
  vector<unordered_set<string>> declared_vars_stack; // стек для сохранения состояния
  
  // Сохранить состояние переменных
  void push_declared_vars() {
    declared_vars_stack.push_back(declared_vars);
    declared_vars.clear();
  }
  
  // Восстановить состояние переменных
  void pop_declared_vars() {
    declared_vars = declared_vars_stack.back();
    declared_vars_stack.pop_back();
  }
  string last_assigned_var;
  void Do(t_calc::t_assign_stat& r) {
    if (declared_vars.count(r.var)) {
      // Переменная уже объявлена - просто присваиваем
      shader_code << r.var << " = ";
      Do(r.expr);
      shader_code << ";\n";
    } else {
      // Первое объявление переменной
      declared_vars.insert(r.var);
      shader_code << "float " << r.var << " = ";
      Do(r.expr);
      shader_code << ";\n";
    }
    last_assigned_var=r.var;
  }
  
  void Do(t_calc::t_func_stat& r) {
    auto fn = make_unique<t_shader_function>();
    fn->name = r.func;
    for (auto& a : r.args) {
      fn->args.push_back(a.name);
    }
    fn->parent = cur_func;
    fn->body = "";
      
    auto* ptr = fn.get();
    cur_func->funcs[r.func] = std::move(fn);
    //all_funcs.push_back(ptr);
      
    cur_func = ptr;
    for (auto& a : r.args) {
      cur_func->get_slot(a.name);
      declared_vars.insert(a.name); // аргументы тоже считаются объявленными
    }
      
    // Сохраняем состояние перед генерацией тела
    push_declared_vars();
      
    //stringstream body_ss;
    //shader_code.swap(body_ss);
      
    stringstream decl_ss;
    decl_ss << "float " << r.func << "(";
    for (size_t i = 0; i < r.args.size(); i++) {
      if (i > 0) decl_ss << ", ";
      decl_ss << "float " << r.args[i].name;
    }
    decl_ss << ") {\n";
    shader_code << decl_ss.str();
      
    indent_level++;
    Do(*r.body);
    indent_level--;
      
    add_line("return "+last_assigned_var+";");
    shader_code << "}\n\n";
      
    ptr->body = shader_code.str();
    //shader_code.swap(body_ss);
      
    // Восстанавливаем состояние
    pop_declared_vars();
      
    cur_func = cur_func->parent;
  }
  
  // Также нужно обработать блоки (scope), чтобы переменные не выходили за пределы блока
  void Do(t_calc::t_block_stat& r) {
    // Сохраняем состояние перед входом в блок
    push_declared_vars();
    
    scopes.push_back({});
    for (auto& st : r.arr) {
      Do(*st.get());
    }
    scopes.pop_back();
    
    // Восстанавливаем состояние после выхода из блока
    pop_declared_vars();
  }
  
  // В Do(t_calc::t_solo_stat& r) - объявление result
  void Do(t_calc::t_solo_stat& r) {
    // Проверяем, не объявлен ли уже result
    if (declared_vars.count("result")) {
      shader_code << "result = ";
      Do(r.expr);
      shader_code << ";\n";
    } else {
      declared_vars.insert("result");
      shader_code << "float result = ";
      Do(r.expr);
      shader_code << ";\n";
    }
    last_assigned_var="result";
  }
  
  void Do(t_calc& r) {
    // Сначала собираем все функции
    for (auto& st : r.arr) {
      Do(*st.get());
    }
  }
  
  // --- Генерация финального шейдера ---
  
  string generate_shader() {
    stringstream final_shader;
    
    final_shader << "#version 300 es\n";
    final_shader << "precision highp float;\n\n";
    
    // Объявляем uniform-ы
    final_shader << "uniform vec2 uResolution;\n";
    final_shader << "uniform vec2 uCenter;\n";
    final_shader << "uniform float uZoom;\n";
    final_shader << "uniform vec2 uOffset;\n";
    final_shader << "uniform float uT;\n\n";
    
    // Встроенные функции-заглушки для глобальных переменных
    final_shader << "// Встроенные переменные\n";
    final_shader << "float x = 0.0;\n";
    final_shader << "float y = 0.0;\n";
    final_shader << "float t = 0.0;\n\n";
    
    final_shader << shader_code.str();

    // Генерируем все функции
    for (auto* f : all_funcs) {
      //if (f->name == "r" || f->name == "g" || f->name == "b") {
      final_shader << f->body;
      //}
    }
    
    // Корневая функция main
    final_shader << "out vec4 fragColor;\n\n";
    final_shader << "void main() {\n";
    final_shader << "  vec2 fragCoord = gl_FragCoord.xy;\n";
    final_shader << "  vec2 world = uCenter + (fragCoord - uResolution * 0.5) / uZoom + uOffset;\n";
    final_shader << "  \n";
    final_shader << "  float x = world.x;\n";
    final_shader << "  float y = world.y;\n";
    final_shader << "  float t = uT;\n";
    final_shader << "  \n";
    final_shader << "  float r = r(x, y, t);\n";
    final_shader << "  float g = g(x, y, t);\n";
    final_shader << "  float b = b(x, y, t);\n";
    final_shader << "  \n";
    final_shader << "  vec3 color = vec3(r, g, b);\n";
    final_shader << "  fragColor = vec4(color, 1.0);\n";
    final_shader << "}\n";
    
    return final_shader.str();
  }
  
  // Основной метод - компиляция в шейдер
  string main(const string& data, const string& out_fn = {}) {
    t_calc tar;
    string errmsg;
    
    scopes.reserve(4096);
    scopes.push_back({});
    
    cur_func = new t_shader_function();
    cur_func->name = "<root>";
    
    auto r = load_obj_full(tar, data, true, &errmsg);
    if (!r.ok) {
      QapDebugMsg(r.msg);
      return {};
    }
    
    // Обход AST и сбор функций
    Do(tar);
    
    // Генерация шейдера
    string shader_code = generate_shader();
    
    // Сохраняем в файл, если указано
    if (!out_fn.empty()) {
      ofstream f(out_fn);
      f << shader_code;
      f.close();
      cout << "Shader saved to: " << out_fn << endl;
    }
    
    // Выводим шейдер
    cout << "Generated Shader:\n" << shader_code << endl;
    
    return shader_code;
  }
};*/
struct t_ast2shader : t_calc::i_term::i_visitor, t_calc::i_stat::i_visitor {
  struct t_shader_function {
    string name;
    string full_name;
    vector<string> args;
    string body;
    t_shader_function* parent = nullptr;
    
    bool is_root = false;
  };
  
  t_shader_function* cur_func = nullptr;
  
  stringstream shader_code;
  int indent_level = 0;
  
  // Сборщик функций
  vector<t_shader_function*> collected_functions;  // функции в порядке сбора (для выноса наверх)
  unordered_set<string> function_names;  // для проверки дубликатов
  
  // Формирование полного имени
  string build_full_name(t_shader_function* func) {
    vector<string> parts;
    auto* current = func;
    while (current && current->name != "<root>") {
      parts.push_back(current->name);
      current = current->parent;
    }
    reverse(parts.begin(), parts.end());
    
    string full_name;
    for (size_t i = 0; i < parts.size(); i++) {
      if (i > 0) full_name += "_";
      full_name += parts[i];
    }
    return full_name;
  }
  
  // Проверка на встроенную функцию
  bool is_builtin_function(const string& name) {
    static unordered_set<string> builtins = {
      "sin", "cos", "tan", "asin", "acos", "atan", "atan2", 
      "sqrt", "pow", "exp", "log", "log2", "abs", "sign", 
      "floor", "ceil", "fract", "min", "max", "clamp", 
      "mix", "step", "smoothstep", "length", "distance", 
      "dot", "cross", "normalize", "reflect", "refract"
    };
    return builtins.find(name) != builtins.end();
  }
  
  // Получение полного имени функции для вызова
  string get_full_func_name(const string& name) {
    if (name == "r" || name == "g" || name == "b" || name == "rgb") {
      return name;
    }
    if (is_builtin_function(name)) {
      return name;
    }
    
    // Ищем в собранных функциях
    for (auto* f : collected_functions) {
      if (f->name == name) {
        return f->full_name;
      }
    }
    
    return name;
  }
  
  string format_number(double val) {
    stringstream ss;
    if (fmod(val, 1.0) == 0.0) {
      ss << std::fixed << std::setprecision(1) << val;
    } else {
      ss << std::setprecision(8) << val;
    }
    return ss.str();
  }
  
  string format_number(const string& s) {
    double val = atof(s.c_str());
    return format_number(val);
  }
  
  string indent() {
    return string(indent_level * 4, ' ');
  }
  
  void add_line(const string& line) {
    shader_code << indent() << line << "\n";
  }
  
  template<class TYPE> void Do(TYPE& r) { r.Use(*this); }
  template<class TYPE> void Do(vector<TYPE>& arr) { for(auto& ex : arr) Do(&ex); }
  template<class TYPE> void Do(vector<TAutoPtr<TYPE>>& arr) { for(auto& ex : arr) if(ex) Do(*ex.get()); }
  template<class TYPE> void Do(TAutoPtr<TYPE>& r) { if(r) Do(*r.get()); }
  
  // --- Обработка терминов ---
  
  void Do(t_calc::t_term& r) { Do(r.value); }
  
  void Do(t_calc::t_number& r) {
    shader_code << format_number(r.value);
  }
  
  void Do(t_calc::t_scope& r) {
    shader_code << "(";
    Do(r.value);
    shader_code << ")";
  }
  
  void Do(t_calc::t_divmul::t_elem& r) {
    string op = (r.oper == "*") ? " * " : " / ";
    shader_code << op;
    Do(r.expr);
  }
  
  void Do(t_calc::t_divmul& r) {
    Do(r.first);
    for(auto& e : r.arr) Do(e);
  }
  
  void Do(t_calc::t_addsub::t_elem& r) {
    string op = (r.oper == "+") ? " + " : " - ";
    shader_code << op;
    Do(r.expr);
  }
  
  void Do(t_calc::t_addsub& r) {
    Do(r.first);
    for(auto& e : r.arr) Do(e);
  }
  
  // --- Обработка вызовов ---
  
  void Do(t_calc::t_varcall& r) {
    const string& name = r.name;
    
    if (!r.params) {
      shader_code << name;
      return;
    }
    
    string func_name = get_full_func_name(name);
    
    if (is_builtin_function(name)) {
      shader_code << name << "(";
      for (size_t i = 0; i < r.params->arr.size(); i++) {
        if (i > 0) shader_code << ", ";
        Do(r.params->arr[i].body);
      }
      shader_code << ")";
    } else {
      shader_code << func_name << "(";
      for (size_t i = 0; i < r.params->arr.size(); i++) {
        if (i > 0) shader_code << ", ";
        Do(r.params->arr[i].body);
      }
      shader_code << ")";
    }
  }
  
  // --- Состояние переменных ---
  
  unordered_set<string> declared_vars;
  vector<unordered_set<string>> declared_vars_stack;
  string last_assigned_var;
  
  void push_declared_vars() {
    declared_vars_stack.push_back(declared_vars);
    declared_vars.clear();
  }
  
  void pop_declared_vars() {
    declared_vars = declared_vars_stack.back();
    declared_vars_stack.pop_back();
  }
  
  void Do(t_calc::t_assign_stat& r) {
    if (declared_vars.count(r.var)) {
      shader_code << r.var << " = ";
      Do(r.expr);
      shader_code << ";\n";
    } else {
      declared_vars.insert(r.var);
      shader_code << "float " << r.var << " = ";
      Do(r.expr);
      shader_code << ";\n";
    }
    last_assigned_var = r.var;
  }
  
  void Do(t_calc::t_solo_stat& r) {
    if (declared_vars.count("result")) {
      shader_code << "result = ";
      Do(r.expr);
      shader_code << ";\n";
    } else {
      declared_vars.insert("result");
      shader_code << "float result = ";
      Do(r.expr);
      shader_code << ";\n";
    }
    last_assigned_var = "result";
  }
  
  void Do(t_calc::t_block_stat& r) {
    push_declared_vars();
    for (auto& st : r.arr) {
      Do(*st.get());
    }
    pop_declared_vars();
  }
  
  // --- Обработка функции ---
  
  void Do(t_calc::t_func_stat& r) {
    // Создаем новую функцию
    auto fn = new t_shader_function();
    fn->name = r.func;
    for (auto& a : r.args) {
      fn->args.push_back(a.name);
    }
    fn->parent = cur_func;
    fn->is_root = false;
    
    // Вычисляем полное имя
    fn->full_name = build_full_name(fn);
    
    // Сохраняем в общий список (собираем все функции)
    collected_functions.push_back(fn);
    
    // Переключаем контекст
    auto* old_func = cur_func;
    cur_func = fn;
    
    // Генерируем тело функции
    push_declared_vars();
    for (auto& a : r.args) {
      declared_vars.insert(a.name);
    }
    
    // Сохраняем текущий код
    stringstream saved_code;
    shader_code.swap(saved_code);
    
    // Генерируем тело
    Do(*r.body);
    
    // Добавляем return
    add_line("return " + last_assigned_var + ";");
    
    // Сохраняем тело в функции
    fn->body = shader_code.str();
    
    // Восстанавливаем код
    shader_code.swap(saved_code);
    
    pop_declared_vars();
    cur_func = old_func;
  }
  
  void Do(t_calc& r) {
    // Просто обходим AST, собирая все функции
    // Ничего не генерируем в этот момент
    for (auto& st : r.arr) {
      Do(*st.get());
    }
  }
  
  // --- Генерация финального шейдера ---
  
  string generate_shader() {
    stringstream final_shader;
    
    final_shader << "#version 300 es\n";
    final_shader << "precision highp float;\n\n";
    
    final_shader << "uniform vec2 uResolution;\n";
    final_shader << "uniform vec2 uCenter;\n";
    final_shader << "uniform float uZoom;\n";
    final_shader << "uniform vec2 uOffset;\n";
    final_shader << "uniform float uT;\n\n";
    
    // Генерируем все собранные функции (в порядке сбора)
    // Но сначала нужно отсортировать: сначала те, у которых нет parent, потом остальные
    // Лучше: сначала все функции, которые не являются r/g/b
    
    // Сортируем: сначала все функции, потом корневые r/g/b
    vector<t_shader_function*> sorted_funcs;
    vector<t_shader_function*> root_funcs;
    
    for (auto* f : collected_functions) {
      // Проверяем, является ли функция корневой (r, g, b)
      bool is_root_color = (f->name == "r" || f->name == "g" || f->name == "b") && 
                           f->parent && f->parent->name == "<root>";
      
      if (is_root_color) {
        root_funcs.push_back(f);
      } else {
        sorted_funcs.push_back(f);
      }
    }
    
    // Сортируем по имени (для детерминизма)
    if(0)sort(sorted_funcs.begin(), sorted_funcs.end(), 
         [](t_shader_function* a, t_shader_function* b) {
           return a->full_name < b->full_name;
         });
    
    // Генерируем все функции (кроме r/g/b)
    for (auto* f : sorted_funcs) {
      final_shader << "float " << f->full_name << "(";
      for (size_t i = 0; i < f->args.size(); i++) {
        if (i > 0) final_shader << ", ";
        final_shader << "float " << f->args[i];
      }
      final_shader << ") {\n";
      final_shader << f->body;
      final_shader << "}\n\n";
    }
    
    // Генерируем корневые r/g/b
    for (auto* f : root_funcs) {
      final_shader << "float " << f->full_name << "(";
      for (size_t i = 0; i < f->args.size(); i++) {
        if (i > 0) final_shader << ", ";
        final_shader << "float " << f->args[i];
      }
      final_shader << ") {\n";
      final_shader << f->body;
      final_shader << "}\n\n";
    }
    
    // Корневая функция main
    final_shader << "out vec4 fragColor;\n\n";
    final_shader << "void main() {\n";
    final_shader << "  vec2 fragCoord = gl_FragCoord.xy;\n";
    final_shader << "  vec2 world = uCenter + (fragCoord - uResolution * 0.5) / uZoom + uOffset;\n";
    final_shader << "  \n";
    final_shader << "  float x = world.x;\n";
    final_shader << "  float y = world.y;\n";
    final_shader << "  float t = uT;\n";
    final_shader << "  \n";
    
    // Вызываем r/g/b с полными именами
    string r_name = "r";
    string g_name = "g";
    string b_name = "b";
    
    for (auto* f : root_funcs) {
      if (f->name == "r") r_name = f->full_name;
      if (f->name == "g") g_name = f->full_name;
      if (f->name == "b") b_name = f->full_name;
    }
    
    final_shader << "  float r = " << r_name << "(x, y, t);\n";
    final_shader << "  float g = " << g_name << "(x, y, t);\n";
    final_shader << "  float b = " << b_name << "(x, y, t);\n";
    final_shader << "  \n";
    final_shader << "  vec3 color = vec3(r, g, b);\n";
    final_shader << "  fragColor = vec4(color, 1.0);\n";
    final_shader << "}\n";
    
    return final_shader.str();
  }
  
  // Основной метод
  string main(const string& data, const string& out_fn = {}) {
    t_calc tar;
    string errmsg;
    
    // Корневая функция
    auto root_func = new t_shader_function();
    root_func->name = "<root>";
    root_func->full_name = "<root>";
    root_func->parent = nullptr;
    root_func->is_root = true;
    cur_func = root_func;
    
    auto r = load_obj_full(tar, data, true, &errmsg);
    if (!r.ok) {
      QapDebugMsg(r.msg);
      return {};
    }
    
    // Сбор всех функций (они соберутся в collected_functions)
    Do(tar);
    
    // Генерация шейдера
    string shader_code = generate_shader();
    
    if (!out_fn.empty()) {
      ofstream f(out_fn);
      f << shader_code;
      f.close();
      cout << "Shader saved to: " << out_fn << endl;
    }
    
    return shader_code;
  }
};