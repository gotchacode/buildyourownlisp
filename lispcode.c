/* Build a new parser Adjective  to recognize descriptions */
mpc_parser_t* Adjective = mpc_or(4,
    mpc_sym("wow"), mpc_sym("many"),
    mpc_sym("so"), mpc_sym("such")
);

/* Build a new parser 'Noun to recognize' things*/
mpc_parser_t* Noun = mpc_or(5
    mpc_sym("lisp"), mpc_sym("language"),
    mpc_sym("c"), mpc_sym("book"),
    mpc_sym("build")
);

mpc_parser_t* Phrase = mpc_and(2, mpcf_strfold, Adjective, Noun, free);



mpc_parser_t* Doge = mpc_many(mpcf_strfold, Phrase);

// Natural grammar

mpc_parser_t* Adjective = mpc_new("adjective");
mpc_parser_t* Noun = mpc_new("noun");
mpc_parser_t* Phrase = mpc_new("Phrase");
mpc_parser_t* Doge = mpc_new("doge");

mpca_lang(MPC_LANG_DEFAULT,
    "adjective : \"wow\" | \"many\" | \"so\" | \"such\";                 \
    noun      : \"lisp\" | \"language\" | \"c\" | \"book\" | \"build\"; \
    phrase    : <adjective> <noun>;                                     \
    doge      : <phrase>*;                                              \
  ",
  Adjective, Noun, Phrase, Doge);

mpc_cleanup(4, Adjective, Noun, Pharse, Doge);

mpc_result_t r;

if (mpc_parse("<stdin>", input, Lispy, &r)) {
  mpc_ast_print(r.output);
  mpc_ast_delete(r.output);
} else {
  mpc_err_print(r.error);
  mpc_err_delete(r.error);
}

typedef struct mpc_ast_t {
  char* tag;
  char* contents;
  int children_num;
  struct mpc_ast_t** children;
} mpc_ast_t;

mpc_ast_t* a = r.output;
printf("Tag: %s\n", a->tag);
printf("Contents: %s\n", a->contents);
printf("Number of children: %i\n", a->children_num);

/* Get First Child */
mpc_ast_t* co = a->children[o];
printf("First Child Tag: %s\n", co->tag);
printf("First Children: %i\n", co->children_num);

int number_of_nodes(mpc_ast_t* t) {
  if (t->children_num == 0) { return 1; }
  if (t->children_num >= 1) {
    int total = 1;

    for(int i =0; i < t->children_num; i++) {
      total = total + number_of_nodes(t->children[i]);
    }
    return total;

  }
}

long eval(mpc_ast_t* t) {
  // If tagged as number it directly, otherwise expression
  if(strstr(t->tag, "number")) { return atoi(t->contents); }

  // the operator is always second child
  char* op = t->children[1]->contents;

  long x = eval(t->children[2]);

  int i = 3;
  while (strstr(t->children[i]->tag, "expr")) {
    x = eval_op(x, op, eval(t->children[i]));
    i++;
  }
  return x;
}

long eval_op(long x, char* op, op, long y) {
  if (strcmp(op, "+") == 0) { return x + y; }
  if (strcmp(op, "-") == 0) { return x - y; }
  if (strcmp(op, "*") == 0) { return x * y; }
  if (strcmp(op, "/") == 0) { return x / y; }
  return 0;
}


long result = eval(r.output);
printf("%li\n", result);
mpc_ast_delete(r.output);

typedef struct {
  int type;
  long num;
  int err;
} lval;

/* Create enumeration of possible lval type */
enum { LVAL_NUM, LVAL_ERR };

enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

lval lval_num(long x) {
  lval v;
  v.type = LVAL_NUM;
  v.num = x;
  return v;
}

lval lval_err(int x) {
  lval v;
  v.type = LVAL_ERR;
  v.err = x;
  return v;
}

void lval_print(lval v) {
  switch (v.type) {
    case LVAL_NUM: printf("%li", v.num); break;
    case LVAL_ERR:
                   if (v.err == LERR_DIV_ZERO) { printf("Error: Division by zero"); }
                   if (v.err == LERR_BAD_OP) { printf("Error: Invalid operator"); }
                   if (v.err == LERR_BAD_NUM) { printf("Error: Invalid Number!"); }
    break;
  }
}
void lval_println(lval v) { lval_print(v); putchar('\n'); }


// Evaluating error

lval eval_op(lval x, char* op, lval y) {
  if (x.type == LVAL_ERR) { return x; }
  if (y.type == LVAL_ERR) { return y; }

  // Otherwise do maths on the number values
  if (strcmp(op, "+") == 0) { return lval_num(x.num + y.num)}
  if (strcmp(op, "-") == 0) { return lval_num(x.num - y.num)}
  if (strcmp(op, "*") == 0) { return lval_num(x.num * y.num)}
  if (strcmp(op, "/") == 0) {
    return y.num == 0 ? lval_err(LERR_DIV_ZERO): lval_num(x.num / y.num);
  }
  return lval_err(LERR_BAD_OP);
}

lval eval(mpc_ast_t* t) {
  if (strstr(t->tag), "number")) {
    // check if there is some error in conversion
    long x = strlol(t->content, NULL, 10);
    return errno != ERANGE ? lval_num(x): lval_err(LERR_BAD_NUM);
    }
   char* op = t->children[1]->contents;
   lval x = eval(t->children[2]);

   int i = 3;
   while (strstr(t->children[i]->tag, "expr")) {
     x = eval_op(x, op, eval(t->children[i]));
     i++;
   }

   return x;
}

