/* A Bison parser, made by GNU Bison 2.7.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2012 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_QP_QP_TAB_H_INCLUDED
# define YY_QP_QP_TAB_H_INCLUDED
/* Enabling traces.  */
#ifndef QPDEBUG
# if defined YYDEBUG
#  if YYDEBUG
#   define QPDEBUG 1
#  else
#   define QPDEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define QPDEBUG 1
# endif /* ! defined YYDEBUG */
#endif  /* ! defined QPDEBUG */
#if QPDEBUG
extern int qpdebug;
#endif
/* "%code requires" blocks.  */
/* Line 2065 of yacc.c  */
#line 14 "query_parser.y"

  #include "query_parser.h"


/* Line 2065 of yacc.c  */
#line 59 "qp.tab.h"

/* Tokens.  */
#ifndef QPTOKENTYPE
# define QPTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum qptokentype {
     AND = 258,
     OR = 259,
     NOT = 260,
     LPAREN = 261,
     RPAREN = 262,
     EQ = 263,
     GT = 264,
     LT = 265,
     GTE = 266,
     LTE = 267,
     EOQ = 268,
     STAR = 269,
     TILDE = 270,
     NEQ = 271,
     MINUS = 272,
     DIGITS = 273,
     DOT = 274,
     STRING_LITERAL = 275,
     UNQUOTED = 276,
     LIMIT = 277
   };
#endif
/* Tokens.  */
#define AND 258
#define OR 259
#define NOT 260
#define LPAREN 261
#define RPAREN 262
#define EQ 263
#define GT 264
#define LT 265
#define GTE 266
#define LTE 267
#define EOQ 268
#define STAR 269
#define TILDE 270
#define NEQ 271
#define MINUS 272
#define DIGITS 273
#define DOT 274
#define STRING_LITERAL 275
#define UNQUOTED 276
#define LIMIT 277



#if ! defined QPSTYPE && ! defined QPSTYPE_IS_DECLARED
typedef union QPSTYPE
{
/* Line 2065 of yacc.c  */
#line 19 "query_parser.y"
 
  ASTNode *node;
  int         num;
  char       *cptr;


/* Line 2065 of yacc.c  */
#line 125 "qp.tab.h"
} QPSTYPE;
# define QPSTYPE_IS_TRIVIAL 1
# define qpstype QPSTYPE /* obsolescent; will be withdrawn */
# define QPSTYPE_IS_DECLARED 1
#endif

#if ! defined QPLTYPE && ! defined QPLTYPE_IS_DECLARED
typedef struct QPLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} QPLTYPE;
# define qpltype QPLTYPE /* obsolescent; will be withdrawn */
# define QPLTYPE_IS_DECLARED 1
# define QPLTYPE_IS_TRIVIAL 1
#endif


#ifndef YYPUSH_MORE_DEFINED
# define YYPUSH_MORE_DEFINED
enum { YYPUSH_MORE = 4 };
#endif

typedef struct qppstate qppstate;

#if defined __STDC__ || defined __cplusplus
int qpparse (QueryParser* context);
#else
int qpparse ();
#endif
#if defined __STDC__ || defined __cplusplus
int qppush_parse (qppstate *ps, int pushed_char, QPSTYPE const *pushed_val, QPLTYPE *pushed_loc, QueryParser* context);
#else
int qppush_parse ();
#endif
#if defined __STDC__ || defined __cplusplus
int qppull_parse (qppstate *ps, QueryParser* context);
#else
int qppull_parse ();
#endif
#if defined __STDC__ || defined __cplusplus
qppstate * qppstate_new (void);
#else
qppstate * qppstate_new ();
#endif
#if defined __STDC__ || defined __cplusplus
void qppstate_delete (qppstate *ps);
#else
void qppstate_delete ();
#endif

#endif /* !YY_QP_QP_TAB_H_INCLUDED  */
