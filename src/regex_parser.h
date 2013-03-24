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

#ifndef YY_RX_REGEX_PARSER_H_INCLUDED
# define YY_RX_REGEX_PARSER_H_INCLUDED
/* Enabling traces.  */
#ifndef RXDEBUG
# if defined YYDEBUG
#  if YYDEBUG
#   define RXDEBUG 1
#  else
#   define RXDEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define RXDEBUG 1
# endif /* ! defined YYDEBUG */
#endif  /* ! defined RXDEBUG */
#if RXDEBUG
extern int rxdebug;
#endif
/* "%code requires" blocks.  */
/* Line 2058 of yacc.c  */
#line 14 "regex_parser.y"

  #include "_regex_parser.h"
  #include "pstring.h"


/* Line 2058 of yacc.c  */
#line 60 "regex_parser.h"

/* Tokens.  */
#ifndef RXTOKENTYPE
# define RXTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum rxtokentype {
     RWITH = 258,
     RAS = 259,
     RINT = 260,
     RTOKENIZED = 261,
     RLPAREN = 262,
     RRPAREN = 263,
     RSTRING_LITERAL = 264,
     RUNQUOTED = 265,
     REOQ = 266,
     RCOMMA = 267
   };
#endif
/* Tokens.  */
#define RWITH 258
#define RAS 259
#define RINT 260
#define RTOKENIZED 261
#define RLPAREN 262
#define RRPAREN 263
#define RSTRING_LITERAL 264
#define RUNQUOTED 265
#define REOQ 266
#define RCOMMA 267



#if ! defined RXSTYPE && ! defined RXSTYPE_IS_DECLARED
typedef union RXSTYPE
{
/* Line 2058 of yacc.c  */
#line 20 "regex_parser.y"
 
  pstring *pstr;


/* Line 2058 of yacc.c  */
#line 104 "regex_parser.h"
} RXSTYPE;
# define RXSTYPE_IS_TRIVIAL 1
# define rxstype RXSTYPE /* obsolescent; will be withdrawn */
# define RXSTYPE_IS_DECLARED 1
#endif

#if ! defined RXLTYPE && ! defined RXLTYPE_IS_DECLARED
typedef struct RXLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} RXLTYPE;
# define rxltype RXLTYPE /* obsolescent; will be withdrawn */
# define RXLTYPE_IS_DECLARED 1
# define RXLTYPE_IS_TRIVIAL 1
#endif


#ifndef YYPUSH_MORE_DEFINED
# define YYPUSH_MORE_DEFINED
enum { YYPUSH_MORE = 4 };
#endif

typedef struct rxpstate rxpstate;

#if defined __STDC__ || defined __cplusplus
int rxparse (RegexParserState* context);
#else
int rxparse ();
#endif
#if defined __STDC__ || defined __cplusplus
int rxpush_parse (rxpstate *ps, int pushed_char, RXSTYPE const *pushed_val, RXLTYPE *pushed_loc, RegexParserState* context);
#else
int rxpush_parse ();
#endif
#if defined __STDC__ || defined __cplusplus
int rxpull_parse (rxpstate *ps, RegexParserState* context);
#else
int rxpull_parse ();
#endif
#if defined __STDC__ || defined __cplusplus
rxpstate * rxpstate_new (void);
#else
rxpstate * rxpstate_new ();
#endif
#if defined __STDC__ || defined __cplusplus
void rxpstate_delete (rxpstate *ps);
#else
void rxpstate_delete ();
#endif

#endif /* !YY_RX_REGEX_PARSER_H_INCLUDED  */
