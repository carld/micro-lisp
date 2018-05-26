/* micro lisp (C) A. Carl Douglas */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define debug(m,e) printf("%s:%d: %s:",__FILE__,__LINE__,m); print_obj(e); puts("");
typedef struct Object {
  enum { _Symbol, _Pair, _Primitive, _Closure, _Macro, _Syntax, _Char, _String, _Integer } tag;
  union Value {
    int mword;
    const char * string;
    struct Object * (*pfn)   (struct Object *);
    struct Object * (*psyn)  (struct Object *, struct Object *);
    struct List {
      struct Object * next;
      struct Object * data;
    } pair;
    struct Closure {
      struct Object * params;
      struct Object * body;
      struct Object * env;
    } closure;
  } value;
} Object;
Object * symbols = 0;
int look; /* look ahead character */
#define TOKEN_MAX 32
char token[TOKEN_MAX]; /* token */
int is_space(char x)  { return x == ' ' || x == '\n'; }
int is_parens(char x) { return x == '(' || x == ')'; }
int is_quote(char x)  { return x == '"'; }
int is_digit(char x)  { return x >= '0' && x <= '9'; }
void lookahead() { look = getchar(); }
void gettoken() {
  int index = 0;
  while(is_space(look)) { lookahead(); }
  if (is_parens(look)) {
    token[index++] = look;  lookahead();
  } else if (is_quote(look)) { /* string */
    token[index++] = look;  lookahead();
    while(index < TOKEN_MAX - 1 && look != EOF && !is_quote(look)) {
      token[index++] = look; lookahead();
    }
    token[index++] = look;  lookahead();
  } else if (is_digit(look)) { /* number */
    while(index < TOKEN_MAX - 1 && look != EOF && is_digit(look)) {
      token[index++] = look; lookahead();
    }
  } else { /* symbol */
    while(index < TOKEN_MAX - 1 && look != EOF && !is_space(look) && !is_parens(look)) {
      token[index++] = look;  lookahead();
    }
  }
  token[index] = '\0';
}

Object * tagalloc(int type) {
  Object *obj = calloc( 1, sizeof (Object) );
  obj->tag = type;
  return obj;
}

Object * cons(Object * _car, Object * _cdr) {
  Object *_pair = tagalloc(_Pair);
  _pair->value.pair.data = _car, _pair->value.pair.next = _cdr;
  return _pair;
}

Object * newsymbol(const char *str) {
  Object *obj = tagalloc(_Symbol);
  obj->value.string = strdup(str);
  return obj;
}

Object *newnumber(const char * num) {
  Object *obj = tagalloc(_Integer);
  obj->value.mword = atoi(num);
  return obj;
}

Object * newstring(const char *str) {
  Object *obj = tagalloc(_String);
  int length = strlen(str);
  char * string = strdup(str+1); /* trim quotes */ string[length-2] = '\0';
  obj->value.string = string;
  return obj;
}

Object * newprimop( Object * (*fn) (Object *) ) {
  Object *obj = tagalloc(_Primitive);
  obj->value.pfn = fn;
  return obj;
}

Object * newclosure( Object *params, Object *body, Object *env ) {
  Object *obj = tagalloc(_Closure);
  obj->value.closure.params = params, obj->value.closure.body = body, obj->value.closure.env = env;
  return obj;
}

Object * newsyntax( Object * (*fn) (Object *, Object *) ) {
  Object *obj = tagalloc(_Syntax);
  obj->value.psyn = fn;
  return obj;
}

Object * newmacro( Object *params, Object *body, Object *env ) {
  Object *obj = tagalloc(_Macro);
  obj->value.closure.params = params, obj->value.closure.body = body, obj->value.closure.env = env;
  return obj;
}

Object * newchar(char ch) {
  Object *obj = tagalloc(_Char);
  obj->value.mword = ch;
  return obj;
}

Object *car(Object *x) { return x ? x->value.pair.data : 0; }
Object *cdr(Object *x) { return x ? x->value.pair.next : 0; }

Object * intern(const char *sym) {
  Object *_pair = symbols;
  for ( ; _pair ; _pair = cdr(_pair))
    if (strncmp(sym, car(_pair)->value.string, TOKEN_MAX)==0)
      return car(_pair);
  symbols = cons(newsymbol(sym), symbols);
  return car(symbols);
}

Object * getlist();

Object * getobj() {
  if (token[0] == '(') return getlist();
  if (is_quote(token[0])) return newstring(token);
  if (is_digit(token[0])) return newnumber(token);
  return intern(token);
}

Object * getlist() {
  Object *tmp;
  gettoken();
  if (token[0] == ')') return (Object *) 0;
  tmp = getobj();
  return cons(tmp, getlist());
}

void print_obj(Object *ob);

void print_obj_list(Object *ob) {
  print_obj(car(ob));
  if (cdr(ob) != 0) {
    if(cdr(ob)->tag == _Pair) {
      printf(" ");
      print_obj_list(cdr(ob));
    } else {
      printf(" . ");
      print_obj(cdr(ob));
    }
  }
}

void print_obj(Object *ob) {
  if (ob == 0) {
    printf("()");
  } else if (ob->tag == _Symbol) {
    printf("%s", ob->value.string);
  } else if (ob->tag == _Pair) {
    printf("(");
    print_obj_list(ob);
    printf(")");
  } else if (ob->tag == _Closure) {
    printf("<CLOSURE>");
  } else if (ob->tag == _Char) {
    printf("%c", ob->value.mword);
  } else if (ob->tag == _String) {
    printf("\"%s\"", ob->value.string);
  } else if (ob->tag == _Integer) {
    printf("%d", ob->value.mword);
  }
}

Object * map(Object *list, Object * (*fn) (Object *, Object *), Object *context) {
  Object *head = 0, **args = &head;
  for ( ; list ; list = cdr(list) ) {
    *args = cons( fn(car(list), context), 0);
    args = &(*args)->value.pair.next;
  }
  return head;
}

Object * eval(Object *exp, Object *env);
Object * apply(Object *fun, Object *args);

#define e_true     intern("#t")
#define e_false    0

Object *fcons(Object *a)    {  return cons(car(a), car(cdr(a)));  }
Object *fcar(Object *a)     {  return car(car(a));  }
Object *fcdr(Object *a)     {  return cdr(car(a));  }
Object *feq(Object *a)      {  return car(a) == car(cdr(a)) ? e_true : e_false;  }
Object *fpair(Object *a)    {  return car(a)->tag == _Pair  ? e_true : e_false;  }
Object *fatom(Object *a)    {  return car(a)->tag == _Symbol  ? e_true : e_false;  }
Object *fnull(Object *a)    {  return car(a) == 0           ? e_true : e_false; }
Object *freadobj(Object *a) {  lookahead(); gettoken(); return getobj();  }
Object *fwriteobj(Object *a){  print_obj(car(a)); puts(""); return e_true;  }
Object *fputch(Object *a)   {  putchar(car(a)->value.mword); return e_true; }
Object *fgetch(Object *a)   {  return newchar(getchar()); }
#define DEFMATH(op,name) \
Object * name (Object *args) { \
  Object *result = tagalloc(_Integer); \
  result->value.mword = car(args)->value.mword; \
  for ( args = cdr(args); args; args = cdr(args)) \
    result->value.mword = result->value.mword op car(args)->value.mword; \
  return result; \
}
DEFMATH(+,fadd)
DEFMATH(-,fsub)
DEFMATH(*,fmul)
DEFMATH(/,fdiv)

Object *fapply(Object *exp, Object *env) {
  Object *head = 0, **args = &head, *tmp = cdr(cdr(exp));
  for (  ; cdr(tmp); tmp = cdr(tmp) ) {
    *args = cons( eval(car(tmp), env), 0);
    args = &(*args)->value.pair.next;
  }
  *args = eval(car(tmp), env); /* last argument to apply must be a list */
  return apply(eval(car(cdr(exp)), env), head);
}

Object *fquote(Object *exp, Object *env){  return car(cdr(exp)); }
Object *flambda(Object *exp, Object *env){ return newclosure(car(cdr(exp)), car(cdr(cdr(exp))), env); }
Object *fcond(Object *exp, Object *env) {
  for (exp = cdr(exp) ; exp ; exp = cdr(exp) ) {
    if (eval(car(car(exp)), env) != e_false) /* anything not false is true */
      return eval(car(cdr(car(exp))), env);
  }
  return 0;
}

Object *fmacro(Object *exp, Object *env) {
  Object *macro = eval(car(cdr(exp)), env);
  return newmacro(macro->value.closure.params, macro->value.closure.body, macro->value.closure.env);
}

Object *flet(Object *exp, Object *env) {
  Object *names_head = 0, **names = &names_head, *values_head = 0, **values = &values_head, *body = car(cdr(cdr(exp)));
  for ( exp = car(cdr(exp)); exp ; exp = cdr(exp) ) {
    *names = cons(car(car(exp)), 0);
    names = &( *names )->value.pair.next;
    *values = cons(car(cdr(car(exp))), 0);
    values = &( *values )->value.pair.next;
  }
  return eval(cons(cons(intern("lambda"), cons(names_head, cons(body, 0))), values_head), env);
}

Object * bind_append(Object *names, Object *values, Object *tail) {
  Object *head = tail, **args = &head;
  for ( ; values ; values = cdr(values), names = cdr(names) ) {
    if (car(names) == intern(".")) { /* variadic lambda syntax */
      *args = cons( cons(car(cdr(names)), cons(values, 0)), tail);
      break;
    }
    *args = cons( cons(car(names), cons(car(values), 0)) , tail);
    args = &( *args )->value.pair.next;
  }
  return head;
}

Object * apply(Object *fun, Object *args) {
  if (fun->tag == _Primitive) {
    return fun->value.pfn(args);
  } else if (fun->tag == _Closure) {
    Object *env = bind_append(fun->value.closure.params, args, fun->value.closure.env);
    return eval( fun->value.closure.body, env );
  }
  puts("cannot apply: "); print_obj(fun); printf("\n");
  return 0;
}

Object * eval(Object *exp, Object *env) {
  if (exp->tag == _Symbol ) {
    for ( ; env != 0; env = cdr(env) ) {
      if (exp == car(car(env)))
        return car(cdr(car(env)));
    }
    printf("cannot lookup: "); print_obj(exp); printf("\n");
    return 0;
  } else if (exp->tag == _Integer) {
    return exp;
  } else if (exp->tag == _String) {
    return exp;
  } else if (exp->tag == _Closure) {
    return eval( exp->value.closure.body, env );
  } else if (exp->tag == _Pair) { /* prepare for apply */
    Object *fun = eval(car(exp), env);
    if (fun->tag == _Macro) {
      Object *env = bind_append(fun->value.closure.params, cdr(exp), fun->value.closure.env);
      return eval(fun->value.closure.body, env);
    } else if (fun->tag == _Syntax) {
      return fun->value.psyn(exp, env);
    } else {
      return apply(fun, map(cdr(exp), eval, env));
    }
  }
  puts("cannot evaluate: "); print_obj(exp); printf("\n");
  return 0;
}

int main(int argc, char *argv[]) {
  Object *env = cons (cons(intern("car"), cons(newprimop(fcar), 0)),
              cons (cons(intern("cdr"), cons(newprimop(fcdr), 0)),
              cons (cons(intern("cons"), cons(newprimop(fcons), 0)),
              cons (cons(intern("eq?"), cons(newprimop(feq), 0)),
              cons (cons(intern("pair?"), cons(newprimop(fpair), 0)),
              cons (cons(intern("symbol?"), cons(newprimop(fatom), 0)),
              cons (cons(intern("null?"), cons(newprimop(fnull), 0)),
              cons (cons(intern("read"), cons(newprimop(freadobj), 0)),
              cons (cons(intern("write"), cons(newprimop(fwriteobj), 0)),
              cons (cons(intern("apply"), cons(newsyntax(fapply), 0)),
              cons (cons(intern("quote"), cons(newsyntax(fquote), 0)),
              cons (cons(intern("lambda"), cons(newsyntax(flambda), 0)),
              cons (cons(intern("cond"), cons(newsyntax(fcond), 0)),
              cons (cons(intern("let"), cons(newsyntax(flet), 0)),
              cons (cons(intern("macro"), cons(newsyntax(fmacro), 0)),
              cons (cons(intern("+"), cons(newprimop(fadd), 0)),
              cons (cons(intern("-"), cons(newprimop(fsub), 0)),
              cons (cons(intern("*"), cons(newprimop(fmul), 0)),
              cons (cons(intern("/"), cons(newprimop(fdiv), 0)),
               0)))))))))))))))))));
  lookahead();
  gettoken();
  print_obj( eval(getobj(), env) );
  printf("\n");
  return 0;
}
