/* micro lisp (C) A. Carl Douglas */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct List {
  struct List * next;
  void * data;
} List;
static int look; /* look ahead character */
static char token[16]; /* token */
#define is_space(x)  (x==' '||x=='\n')
#define is_parens(x) (x=='('||x==')')
static void gettoken() {
  int index = 0;
  while(is_space(look))
    look = getchar();
  if (is_parens(look)) {
    token[index++] = look;
    look = getchar();
  } else {
    while(look != EOF && !is_space(look) && !is_parens(look)) {
      token[index++] = look;
      look = getchar();
    }
  }
  token[index] = '\0';
}
#define is_pair(x) ((long) x & 0x1 )  /* tag pointer to pair with 0x1 (alignment dependent)*/
#define untag(x) ((long) x & ~0x1)
#define tag(x)   ((long) x | 0x1)
#define car(x)   (((List*)untag(x))->data)
#define cdr(x)   (((List*)untag(x))->next)
List * cons(void *_car, void *_cdr) {
  List *_pair = calloc( 1, sizeof (List) );
  _pair->data = _car;
  _pair->next = _cdr;
  return (List*) tag(_pair);
}
List * getlist();
void * getobj() {
  if (token[0]=='(') return getlist();
  return strdup(token);
}

List * getlist() {
  List *tmp;
  gettoken();
  if (token[0]==')') return NULL;
  tmp = getobj();
  return cons(tmp, getlist());
}

void print_obj(List *ob, int head_of_list) {
  if (!is_pair(ob)) {
    printf("%s", (char *) ob ? (char*) ob : "null");
  } else {
    if (head_of_list) printf("(");
    print_obj(car(ob), 1);
    if (cdr(ob) != 0) {
      if (is_pair(cdr(ob)) ) {
        printf(" ");
        print_obj(cdr(ob), 0);
      }
    } else {
      printf(")");
    }
  }
}

List *fcons(List *a) {
  return cons(car(a), cons(car(cdr(a)), 0));
}
List *fcar(List *a) {
  return car(car(a));
}
List *fcdr(List *a) {
  return cdr(car(a));
}
List *feq(List *a) {
  return strcmp((char*)car(a), (char*)car(cdr(a)))==0 ? cons("quote", cons("t", 0)) : 0;
}
List *fpair(List *a) {
  return is_pair(car(a)) ? cons("quote", cons("t", 0)) : 0;
}
List * eval(List *exp, List *env) {
  if (!is_pair(exp) ) {
    for ( ; env != 0; env = cdr (env) ) {
      if (strcmp((char*) untag(exp), (char*) car (car (env) ))==0)
        return car(cdr(car(env)));
    }
    return 0;
  } else {
    if (!is_pair(car (exp))) { /* special forms */
      if (strcmp((char*)car(exp), "quote")==0) {
        return car (cdr (exp));
      } else if (strcmp((char*)car(exp), "if")==0) {
        if (eval (car (cdr (exp)), env) != 0) {
          return eval (car (cdr (cdr (exp))), env);
        } else {
          return eval (car (cdr (cdr (cdr (exp)))), env);
        }
      } else { /* primitive operation */
        List *primop = eval (car (exp), env);
        if ( primop ) {
          List *head = 0, **args = &head;
          for ( exp = cdr(exp) ; exp != 0 ; exp = cdr(exp) ) {
            *args = cons(eval(car(exp), env), 0);
            args = &( (List*)untag(*args) )->next;
          }
          return ((List* (*) (List*)) primop) ( head );
        } else {
          return 0;
        }
      }
    } else { /* should be a lambda */
      if (strcmp((char*) (car(car(exp))), "lambda")==0) {
        List *newenvsym = cons(car(car(cdr(car(exp)))), cons(eval(car(cdr(exp)), env), 0));
        return eval (car(cdr(cdr(car(exp)))), cons(newenvsym, env));
      }
    }
  }
  return (List *)"cannot evaluate expression";
}
int main(int argc, char *argv[]) {
  List *env = cons (cons("car", cons((void *)fcar, 0)), cons(
                    cons("cdr", cons((void *)fcdr, 0)), cons(
                    cons("cons", cons((void *)fcons, 0)), cons(
                    cons("eq", cons((void *)feq, 0)), cons(
                    cons("pair?", cons((void *)fpair, 0)), cons(
                    cons("null", cons(0,0)), 0))))));
  look=getchar();
  gettoken();
  print_obj( eval(getobj(), env), 1 );
  printf("\n");
  return 0;
}
