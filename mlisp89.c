/* micro lisp (C) A. Carl Douglas */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define debug(m,e) fprintf(default_output_port->value.stream, "%s:%d: %s:",__FILE__,__LINE__,m); print_obj(default_output_port, e); fprintf(default_output_port->value.stream, "\n");
typedef struct Object {
  enum { _Symbol, _Pair, _Primitive, _Closure, _Macro, _Syntax, _Char, _String, _Integer, _Port } tag;
  union Value {
    int mword;
    const char * string;
    FILE *stream;
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
static Object *default_input_port, *default_output_port;
static Object * symbols = 0;
static int look; /* look ahead character */
#define TOKEN_MAX 32
static char token[TOKEN_MAX]; /* token */
int is_space(char x)  { return x == ' ' || x == '\n'; }
int is_parens(char x) { return x == '(' || x == ')'; }
int is_syntaxquote(char x)  { return x == '\'' || x == '`' || x == ','; }
int is_doublequote(char x)  { return x == '"'; }
int is_digit(char x)  { return x >= '0' && x <= '9'; }

int freadmem(void *cookie, char *buf, int size) {  /* funopen enables parsing of memory as well as files */
  char **position = (char **) cookie,   *mem = * (char **) position;
  int num_read = 0;
  for( ; num_read < size && *mem != '\0'; num_read++)
    *buf++ = *mem++;
  if (*mem == '\0')   *buf++ = EOF;
  /* printf("Read %d bytes of %d: %s\n", num_read, size, *position); */
  *position = mem; /* move the stream pointer */
  return num_read;
}

void lookahead(Object *port) { look = getc(port->value.stream); }
void gettoken(Object *port) {
  int index = 0;
  while(is_space(look)) { lookahead(port); }
  if (is_parens(look)) {
    token[index++] = look;  lookahead(port);
  } else if (is_syntaxquote(look)) { /* quoting, quasiquoting */
    token[index++] = look; lookahead(port);
  } else if (is_doublequote(look)) { /* string */
    token[index++] = look;  lookahead(port);
    while(index < TOKEN_MAX - 1 && look != EOF && !is_doublequote(look)) {
      token[index++] = look; lookahead(port);
    }
    token[index++] = look;  lookahead(port);
  } else if (is_digit(look)) { /* number */
    while(index < TOKEN_MAX - 1 && look != EOF && is_digit(look)) {
      token[index++] = look; lookahead(port);
    }
  } else { /* symbol */
    while(index < TOKEN_MAX - 1 && look != EOF && !is_space(look) && !is_parens(look)) {
      token[index++] = look;  lookahead(port);
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

Object * newport(FILE *stream) {
  Object *obj = tagalloc(_Port);
  obj->value.stream = stream;
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

Object * getlist(Object *port);
Object * getobj(Object *port);
Object * gettokenobj(Object *port) {
  gettoken(port);
  return getobj(port);
}

Object * getobj(Object *port) {
    /* reader macros */
  if (token[0] == '`') return cons(intern("quasiquote"), cons(gettokenobj(port), 0));
  if (token[0] == '\'') return cons(intern("quote"), cons(gettokenobj(port), 0));
  if (token[0] == ',') return cons(intern("unquote"), cons(gettokenobj(port), 0));
  if (token[0] == '@') return cons(intern("unquote-splicing"), cons(gettokenobj(port), 0));

  if (token[0] == '(') return getlist(port);
  if (is_doublequote(token[0])) return newstring(token);
  if (is_digit(token[0])) return newnumber(token);
  return intern(token);
}

Object * getlist(Object *port) {
  Object *tmp;
  gettoken(port);
  if (token[0] == ')') return (Object *) 0;
  tmp = getobj(port);
  return cons(tmp, getlist(port));
}

void print_obj(Object *port, Object *ob);

void print_obj_list(Object *port, Object *ob) {
  print_obj(port, car(ob));
  if (cdr(ob) != 0) {
    if(cdr(ob)->tag == _Pair) {
      fprintf(port->value.stream, " ");
      print_obj_list(port, cdr(ob));
    } else {
      fprintf(port->value.stream, " . ");
      print_obj(port, cdr(ob));
    }
  }
}

void print_obj(Object *port, Object *ob) {
  if (ob == 0) {
    fprintf(port->value.stream, "()");
  } else if (ob->tag == _Symbol) {
    fprintf(port->value.stream, "%s", ob->value.string);
  } else if (ob->tag == _Pair) {
    /* todo print '`, instead of quote */
    fprintf(port->value.stream, "("); print_obj_list(port, ob); fprintf(port->value.stream, ")");
  } else if (ob->tag == _Closure) {
    fprintf(port->value.stream, "<CLOSURE>");
  } else if (ob->tag == _Primitive) {
    fprintf(port->value.stream, "<PRIMOP>");
  } else if (ob->tag == _Macro) {
    fprintf(port->value.stream, "<MACRO>");
  } else if (ob->tag == _Syntax) {
    fprintf(port->value.stream, "<SYNTAX>");
  } else if (ob->tag == _Char) {
    fprintf(port->value.stream, "%c", ob->value.mword);
  } else if (ob->tag == _String) {
    fprintf(port->value.stream, "\"%s\"", ob->value.string);
  } else if (ob->tag == _Integer) {
    fprintf(port->value.stream, "%d", ob->value.mword);
  }
}

void newline(Object *port) {
  fprintf(port->value.stream, "\n");
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
Object *fnot(Object *a)     {  return car(a) == e_false ? e_true : e_false;  }
Object *fpair(Object *a)    {  return car(a)->tag == _Pair  ? e_true : e_false;  }
Object *fatom(Object *a)    {  return car(a)->tag == _Symbol  ? e_true : e_false;  }
Object *fnull(Object *a)    {  return car(a) == 0           ? e_true : e_false; }
Object *freadobj(Object *a) {  lookahead(default_input_port) /* what about successive call? */; return gettokenobj(default_input_port);  }
Object *fwriteobj(Object *a){  print_obj(default_output_port, car(a)); newline(default_output_port); return e_true;  }
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

Object *fmacro(Object *exp, Object *env) {
  Object *lambda = eval(car(cdr(exp)), env);
  if (lambda->tag != _Closure) { printf("cannot make macro from non-lambda"); }
  return newmacro(lambda->value.closure.params, lambda->value.closure.body, lambda->value.closure.env);
}

Object *fcond(Object *exp, Object *env) {
  for (exp = cdr(exp) ; exp ; exp = cdr(exp) ) {
    if (eval(car(car(exp)), env) != e_false) /* anything not false is true */
      return eval(car(cdr(car(exp))), env);
  }
  return 0;
}
Object *fif(Object *exp, Object *env) {
    if (eval(car(cdr(exp)), env) != e_false) /* anything not false is true */
      return eval(car(cdr(cdr(exp))), env);
    else
      return eval(car(cdr(cdr(cdr(exp)))), env);
}

Object * bind_append(Object *names, Object *values, Object *tail) {
  Object *head = tail, **args = &head;
  if (names == 0) return tail;
  if (names->tag == _Symbol) { /* (lambda args (do args)) */
    return cons( cons(names, cons(values, 0)), tail);
  }
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
  puts("cannot apply: "); print_obj(default_output_port, fun); newline(default_output_port);
  return 0;
}

Object * eval(Object *exp, Object *env) {
  if (exp->tag == _Symbol ) {
    for ( ; env != 0; env = cdr(env) ) {
      if (exp == car(car(env)))
        return car(cdr(car(env)));
    }
    printf("cannot lookup: "); print_obj(default_output_port, exp); newline(default_output_port);
    return 0;
  } else if (exp->tag == _Integer) {
    return exp;
  } else if (exp->tag == _String) {
    return exp;
  } else if (exp->tag == _Closure) {
    return eval( exp->value.closure.body, env );
  } else if (exp->tag == _Pair) { /* prepare for apply */
    Object *fun = eval(car(exp), env);
    if (fun->tag == _Macro) { /* expand and evaluate */
      Object *env0 = bind_append(fun->value.closure.params, cdr(exp), fun->value.closure.env);
      Object *expanded = eval( fun->value.closure.body, env0 );
      return eval(expanded, env);
    } else if (fun->tag == _Syntax) { /* special forms */
      return fun->value.psyn(exp, env);
    } else {
      return apply(fun, map(cdr(exp), eval, env));
    }
  }
  puts("cannot evaluate: "); print_obj(default_output_port, exp); newline(default_output_port);
  return 0;
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

#define LISP(code) #code
static const char * env_src[][2]  = {
  { "Y", LISP((lambda (fn)
                      ((lambda (h) (h h))
                        (lambda (g)
                          (fn (lambda args
                              (apply (g g) args))))))) },
  { "list", LISP((lambda args
                    args)) },
  { "cond", LISP((macro
                    (lambda exp0
                      ((Y (lambda (condr)
                            (lambda (exp)
                              (if (null? exp)
                                (quote (quote ()))
                                (list (quote if) (car (car exp)) (car (cdr (car exp))) (condr (cdr exp)))))))
                        exp0  )))) },
  { "append", LISP((Y (lambda (append0)
                          (lambda (x y)
                             (cond ((null? x) y)
                                   ((quote #t)   (cons (car x) (append0 (cdr x) y)))))))) },
  { "quasiquote", LISP((macro
                        (lambda (exp0)
                          (cons (quote list)
                           ((Y (lambda (expand-qq)
                                (lambda (exp)
                                      (cond ((null? exp)  (quote ()))
                                            ((pair? exp)
                                                  (cond ((eq? (car exp) (quote unquote))  (car (cdr exp)))
                                                        ((quote #t)     (cons (expand-qq (car exp)) (expand-qq (cdr exp))))))
                                            ((quote #t)   (cons (quote quote) (cons exp (quote ()) ))))))) exp0 ))))) },
  { 0, 0 }
};

Object *eval_string(const char *str, Object *env) {
  Object port;
  port.tag = _Port;
  port.value.stream = funopen(&str, freadmem, NULL, NULL, NULL);
  lookahead(&port);
  return eval(gettokenobj(&port), env);
}

Object *extend_env(Object *env, const char *src[][2]) {
  for ( ; (*src)[0] ; src++) {
    env = cons(cons(intern( (*src)[0] ), cons(eval_string( (*src)[1] , env), 0)), env);
  }
  return env;
}

int main(int argc, char *argv[]) {
  Object *env;
  env =       cons (cons(intern("car"), cons(newprimop(fcar), 0)),
              cons (cons(intern("cdr"), cons(newprimop(fcdr), 0)),
              cons (cons(intern("cons"), cons(newprimop(fcons), 0)),
              cons (cons(intern("eq?"), cons(newprimop(feq), 0)),
              cons (cons(intern("not"), cons(newprimop(fnot), 0)),
              cons (cons(intern("pair?"), cons(newprimop(fpair), 0)),
              cons (cons(intern("symbol?"), cons(newprimop(fatom), 0)),
              cons (cons(intern("null?"), cons(newprimop(fnull), 0)),
              cons (cons(intern("read"), cons(newprimop(freadobj), 0)),
              cons (cons(intern("write"), cons(newprimop(fwriteobj), 0)),
              cons (cons(intern("eval"), cons(newsyntax(eval), 0)),
              cons (cons(intern("apply"), cons(newsyntax(fapply), 0)),
              cons (cons(intern("quote"), cons(newsyntax(fquote), 0)),
              cons (cons(intern("lambda"), cons(newsyntax(flambda), 0)),
              cons (cons(intern("if"), cons(newsyntax(fif), 0)),
              cons (cons(intern("let"), cons(newsyntax(flet), 0)),
              cons (cons(intern("macro"), cons(newsyntax(fmacro), 0)),
              cons (cons(intern("+"), cons(newprimop(fadd), 0)),
              cons (cons(intern("-"), cons(newprimop(fsub), 0)),
              cons (cons(intern("*"), cons(newprimop(fmul), 0)),
              cons (cons(intern("/"), cons(newprimop(fdiv), 0)),
               0)))))))))))))))))))));
  env = extend_env(env, env_src);
  default_input_port = newport(stdin), default_output_port = newport(stdout);
#if DEBUG
  print_obj(default_output_port, env);
#endif
  newline(default_output_port);
  lookahead(default_input_port);
  print_obj(default_output_port, eval(gettokenobj(default_input_port), env) );
  newline(default_output_port);
  return 0;
}
