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

#ifndef YY_RE_RE_TAB_H_INCLUDED
# define YY_RE_RE_TAB_H_INCLUDED
/* Enabling traces.  */
#ifndef REDEBUG
# if defined YYDEBUG
#  if YYDEBUG
#   define REDEBUG 1
#  else
#   define REDEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define REDEBUG 1
# endif /* ! defined YYDEBUG */
#endif  /* ! defined REDEBUG */
#if REDEBUG
extern int redebug;
#endif
/* "%code requires" blocks.  */
/* Line 2065 of yacc.c  */
#line 14 "regex_parser.y"

  #include "regex_parser.h"


/* Line 2065 of yacc.c  */
#line 59 "re.tab.h"

/* Tokens.  */
#ifndef RETOKENTYPE
# define RETOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum retokentype {
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



#if ! defined RESTYPE && ! defined RESTYPE_IS_DECLARED
typedef union RESTYPE
{
/* Line 2065 of yacc.c  */
#line 19 "regex_parser.y"
 
  pstring *pstr;


/* Line 2065 of yacc.c  */
#line 103 "re.tab.h"
} RESTYPE;
# define RESTYPE_IS_TRIVIAL 1
# define restype RESTYPE /* obsolescent; will be withdrawn */
# define RESTYPE_IS_DECLARED 1
#endif

#if ! defined RELTYPE && ! defined RELTYPE_IS_DECLARED
typedef struct RELTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} RELTYPE;
# define reltype RELTYPE /* obsolescent; will be withdrawn */
# define RELTYPE_IS_DECLARED 1
# define RELTYPE_IS_TRIVIAL 1
#endif


#ifndef YYPUSH_MORE_DEFINED
# define YYPUSH_MORE_DEFINED
enum { YYPUSH_MORE = 4 };
#endif

typedef struct repstate repstate;

#if defined __STDC__ || defined __cplusplus
int reparse (RegexParser* context);
#else
int reparse ();
#endif
#if defined __STDC__ || defined __cplusplus
int repush_parse (repstate *ps, int pushed_char, RESTYPE const *pushed_val, RELTYPE *pushed_loc, RegexParser* context);
#else
int repush_parse ();
#endif
#if defined __STDC__ || defined __cplusplus
int repull_parse (repstate *ps, RegexParser* context);
#else
int repull_parse ();
#endif
#if defined __STDC__ || defined __cplusplus
repstate * repstate_new (void);
#else
repstate * repstate_new ();
#endif
#if defined __STDC__ || defined __cplusplus
void repstate_delete (repstate *ps);
#else
void repstate_delete ();
#endif

#endif /* !YY_RE_RE_TAB_H_INCLUDED  */
