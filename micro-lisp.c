/* micro lisp (C) A. Carl Douglas */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define debug(m,e) printf("%s:%d: %s:",__FILE__,__LINE__,m); print_obj(e,1); puts("");
typedef struct List {
  struct List * next;
  void * data;
} List;
List *symbols = 0;
static int look; /* look ahead character */
static char token[32]; /* token */
#define is_space(x)  (x == ' ' || x == '\n')
#define is_parens(x) (x == '(' || x == ')')

static void gettoken() {
  int index = 0;
  while(is_space(look)) { look = getchar(); }
  if (is_parens(look)) {
    token[index++] = look;  look = getchar();
  } else {
    while(look != EOF && !is_space(look) && !is_parens(look)) {
      token[index++] = look;  look = getchar();
    }
  }
  token[index] = '\0';
}

#define is_pair(x) (((uintptr_t)x & 0x1) == 0x1)  /* tag pointer to pair with 0x1 (alignment dependent)*/
#define is_atom(x) (((uintptr_t)x & 0x1) == 0x0)
#define untag(x)   ((uintptr_t) x & ~0x1)
#define tag(x)     ((uintptr_t) x | 0x1)
#define car(x)     (((List*)untag(x))->data)
#define cdr(x)     (((List*)untag(x))->next)
#define e_true     cons( intern("quote"), cons( intern("t"), 0))
#define e_false    0

List * cons(void *_car, void *_cdr) {
  List *_pair = calloc( 1, sizeof (List) );
  _pair->data = _car;
  _pair->next = _cdr;
  return (List*) tag(_pair);
}

void *intern(char *sym) {
  List *_pair = symbols;
  for ( ; _pair ; _pair = cdr(_pair))
    if (strncmp(sym, (char*) car(_pair), 32)==0) return car(_pair);
  symbols = cons(strdup(sym), symbols);
  return car(symbols);
}

List * getlist();

void * getobj() {
  if (token[0] == '(') return getlist();
  return intern(token);
}

List * getlist() {
  List *tmp;
  gettoken();
  if (token[0] == ')') return 0;
  tmp = getobj();
  return cons(tmp, getlist());
}

void print_obj(List *ob, int head_of_list) {
  if (!is_pair(ob) ) {
    printf("%s", ob ? (char*) ob : "null" );
  } else {
    if (head_of_list) printf("(");
    print_obj(car(ob), 1);
    if (cdr(ob) != 0) {
      if (is_pair(cdr(ob))) {  printf(" "); print_obj(cdr(ob), 0);  }
    } else  printf(")");
  }
}

List *fcons(List *a)    {  return cons(car(a), car(cdr(a)));  }
List *fcar(List *a)     {  return car(car(a));  }
List *fcdr(List *a)     {  return cdr(car(a));  }
List *feq(List *a)      {  return car(a) == car(cdr(a)) ? e_true : e_false;  }
List *fpair(List *a)    {  return is_pair(car(a))       ? e_true : e_false;  }
List *fatom(List *a)    {  return is_atom(car(a))       ? e_true : e_false;  }
List *fnull(List *a)    {  return car(a) == 0           ? e_true : e_false; }
List *freadobj(List *a) {  look = getchar(); gettoken(); return getobj();  }
List *fwriteobj(List *a){  print_obj(car(a), 1); puts(""); return e_true;  }

List * eval(List *exp, List *env);

List * evlist(List *list, List *env) {
  /* http://cslibrary.stanford.edu/105/LinkedListProblems.pdf */
  List *head = 0, **args = &head;
  for ( ; list ; list = cdr(list) ) {
    *args = cons( eval(car(list), env) , 0);
    args = &( (List *) untag(*args) )->next;
  }
  return head;
}

List * apply_primitive(void *primfn, List *args) {
  return ((List * (*) (List *)) primfn)  ( args );
}

List * eval(List *exp, List *env) {
  if (is_atom(exp) ) {
    for ( ; env != 0; env = cdr(env) )
      if (exp == car(car(env)))  return car(cdr(car(env)));
    return 0;
  } else if (is_atom( car (exp))) { /* special forms */
    if (car(exp) == intern("quote")) {
      return car(cdr(exp));
    } else if (car(exp) == intern("if")) {
      if (eval (car(cdr(exp)), env) != 0)
        return eval (car(cdr(cdr(exp))), env);
      else
        return eval (car(cdr(cdr(cdr(exp)))), env);
    } else if (car(exp) == intern("lambda")) {
      return exp; /* todo: create a closure and capture free vars */
    } else if (car(exp) == intern("apply")) { /* apply function to list */
      List *args = evlist (cdr(cdr(exp)), env);
      args = car(args); /* assumes one argument and that it is a list */
      return apply_primitive( eval(car(cdr(exp)), env), args);
    } else { /* function call */
      List *primop = eval (car(exp), env);
      if (is_pair(primop)) { /* user defined lambda, arg list eval happens in binding  below */
        return eval( cons(primop, cdr(exp)), env );
      } else if (primop) { /* built-in primitive */
        return apply_primitive(primop, evlist(cdr(exp), env));
      }
    }
  } else if (car(car(exp)) == intern("lambda")) { /* should be a lambda, bind names into env and eval body */
    List *extenv = env, *names = car(cdr(car(exp))), *vars = cdr(exp);
    for (  ; names ; names = cdr(names), vars = cdr(vars) )
      extenv = cons (cons(car(names),  cons(eval (car(vars), env), 0)), extenv);
    return eval (car(cdr(cdr(car(exp)))), extenv);
  }
  puts("cannot evaluate expression");
  return 0;
}

int main(int argc, char *argv[]) {
  List *env = cons (cons(intern("car"), cons((void *)fcar, 0)),
              cons (cons(intern("cdr"), cons((void *)fcdr, 0)),
              cons (cons(intern("cons"), cons((void *)fcons, 0)),
              cons (cons(intern("eq?"), cons((void *)feq, 0)),
              cons (cons(intern("pair?"), cons((void *)fpair, 0)),
              cons (cons(intern("symbol?"), cons((void *)fatom, 0)),
              cons (cons(intern("null?"), cons((void *)fnull, 0)),
              cons (cons(intern("read"), cons((void *)freadobj, 0)),
              cons (cons(intern("write"), cons((void *)fwriteobj, 0)),
              cons (cons(intern("null"), cons(0,0)), 0))))))))));
  look = getchar();
  gettoken();
  print_obj( eval(getobj(), env), 1 );
  printf("\n");
  return 0;
}
