//
//  main.c
//  rpn
//
//  Created by William McCarthy on 192//20.
//  Copyright © 2020 William McCarthy. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

#define MAXOP   100    /* max size of operand/operator */
#define NUMBER '0'     /* signal that a number was found */
#define MAXVAL  100
#define MATH 'n'
#define VARMAX 26

size_t sp = 0;   // aka unsigned long -- printf using %zu
double val[MAXVAL];   // stack of values

char buf[BUFSIZ];
size_t bufp = 0;

int getch_(void) { return (bufp > 0) ? buf[--bufp] : getchar(); }

void ungetch_(int c) {
  if (bufp >= BUFSIZ) { fprintf(stderr, "ungetch: too many characters\n");  return; }
  buf[bufp++] = c;
}

int getop(char* s) {
  int i, c;
  while ((s[0] = c = getch_()) == ' ' || c == '\t') { }  // skip whitespace
  s[1] = '\0';
  
  if (isalpha(c)){
    i = 0;
    while (isalpha(s[++i] = c = getch_())){}
    s[i] = '\0';
    return MATH; }

  if (!isdigit(c) && c != '.' && c!='-') { return c; }  // if not a digit, return

  i = 0;
  
  if(c=='-'){
    if(isdigit(c=getch_()) || c == '.'){
      s[++i]=c;}
    else{
      if(c!=EOF)
      ungetch_(c);
      return '-';}}

  if (isdigit(c)) {  // get digits before '.'
    while (isdigit(s[++i] = c = getch_())) { }
  }
  if (c == '.') {    // get digits after decimal (if any)
    while (isdigit(s[++i] = c = getch_())) { }
  }
  s[i] = '\0';
  if (c != EOF) { ungetch_(c); }
  
  return NUMBER;      // return type is NUMBER, number stored in s
}

double pop(void) {
  if (sp == 0) { fprintf(stderr, "stack underflow\n");  return 0.0; }
  return val[--sp];
}

void push(double f) {
  if (sp == MAXVAL) { fprintf(stderr, "stack overflow -- can't push %g\n", f);  return; }
  val[sp++] = f;
}

void clear(void){
  sp = 0;
}

void math(char s[]){
  double op2;
    
  if(strcmp(s,"sin")==0) push(sin(pop()));
  else if(strcmp(s,"cos")==0) push(cos(pop()));
  else if(strcmp(s,"exp")==0) push(exp(pop()));
  else if(strcmp(s,"pow")==0){
       op2 = pop();
       push(pow(pop(),op2));}
  else printf("%s not valid math operator\n",s);
}

void rpn(void) {
  int type,v;
  double op1,op2,mr;//mr is the most recently printed value
  char s[BUFSIZ];
  double variable[VARMAX];

  while ((type = getop(s)) != EOF) {
    switch(type) {
      case '\n':    
        mr = pop();
        printf("\t%.8g\n", mr);  
        break;
      case NUMBER:  push(atof(s));              break;
      case MATH:    math(s);                    break;
      case '+':     push(pop() + pop());        break;
      case '*':     push(pop() * pop());        break;
      case '-':     push(-(pop() - pop()));     break;
      case '/':
        if ((op2 = pop()) == 0.0) { fprintf(stderr, "divide by zero\n");  break; }
        push(pop() / op2);
        break;
      case '%':
        if ((op2 = pop()) == 0.0) { fprintf(stderr, "module by zero\n"); break; }
        push(fmod(pop(),op2));;
        break;
      case 'c':     clear();                    break; //clear stack
      case 'p': //print the top elements of the stack without popping
        op2 = pop();
        printf("\t%.8g\n", op2);
        push(op2);
        break;
      case 'd': //duplicate
        op2 = pop();
        push(op2);
        push(op2);
        break;
      case 's': //swap
        op1 = pop();
        op2 = pop();
        push(op1);
        push(op2);
        break;
      case '$': push(sin(pop()));               break;//sine function
      case '^': //power
        op2 = pop();
        push(pow(pop(), op2));
        break;
      case 'e': push(exp(pop()));               break;//exponent of e
      case '=':
        pop();
        if(v>= 'A' && v <= 'Z') variable[v-'A'] = pop(); break;
        fprintf(stderr, "invalid variable name\n"); break;
      default:      
        if(type>='A' && type<='Z') push(variable[type-'A']);
        else if(type == 'v') push(mr);
        else fprintf(stderr, "unknown command %s\n", s);  break;
    }
    v = type;
  }
}

int main(int argc, const char * argv[]) {
  rpn();

  return 0;
}
