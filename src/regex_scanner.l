%option reentrant
%option bison-bridge
%option bison-locations
%option noyywrap
%option prefix="rx"
%option outfile="lex.yy.c"
%option yylineno
%option extra-type="RegexParserState*"

D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]
NS    [^ \t\n\r,?*();:\^"]

%{
#include "regex_parser.h"
#include "sit.h"
#define YYSTYPE         RXSTYPE
#define YYLTYPE         RXLTYPE
/* Substitute the variable and function names.  */
#define yyparse         rxparse
#define yypush_parse    rxpush_parse
#define yypull_parse    rxpull_parse
#define yypstate_new    rxpstate_new
#define yypstate_delete rxpstate_delete
#define yypstate        rxpstate
#define yylex           rxlex
#define yyerror         rxerror
#define yylval          rxlval
#define yychar          rxchar
#define yydebug         rxdebug
#define yynerrs         rxnerrs
#define yylloc          rxlloc



#define YY_USER_ACTION yylloc->first_line = yylineno;

#define YY_INPUT(cbuf, offset, max_size)  {                                 \
  int i;                                                                    \
  for(i = 0; i < max_size && offset < yyextra->buf->len; i++, offset++) {   \
    cbuf[i] = yyextra->buf->val[offset];                                    \
  }                                                                         \
  offset = i;                                                               \
}

%}

%%

("with"|"WITH")             { return(RWITH); }
("as"|"AS")                 { return(RAS); }
("int"|"INT")               { return(RINT); }
("tokenized"|"TOKENIZED")   { return(RTOKENIZED); }
"("                         { return(RLPAREN); }
")"                         { return(RRPAREN); }
","                         { return(RCOMMA); }
L?`(\\.|[^`])*`      	      { yyextra->ptr = pstring_new2(yytext+1, yyleng-2); ll_add(&yyextra->freeable, yyextra->ptr); return(RSTRING_LITERAL); }
{NS}+             	        { yyextra->ptr = pstring_new2(yytext, yyleng); ll_add(&yyextra->freeable, yyextra->ptr); return(RUNQUOTED); }
[;\n]+                      { return(REOQ); }
[ \t\v\f]  		              { /* ignore whitespace*/ }
.			                      { /* ignore bad characters */ }

%%

void
regex_parser_consume(Parser *parser, pstring *input) {
  RegexParserState *state = parser->state;
  vstring *stream = state->stream;
  vstring_append(stream, input);
  int ovector[RE_PARSER_FIELD_LIMIT];
  pstring text;
  Token t = { &text, 0 };
  DocBuf *doc = parser->buffer;
  
  int len = vstring_size(stream);
  
  while(len > 0) {
    pstring str = { NULL, len };
    vstring_get(&str, stream, 0);
    assert(state->regex);
    assert(state->extra);
    
    int groups = pcre_exec(state->regex, state->extra, str.val, str.len, 
                 0, PCRE_PARTIAL_HARD, ovector, RE_PARSER_FIELD_LIMIT);
    assert(groups != -2);
    if(groups > 0) {
      FieldMatcher *field = state->fields;
      for (int i = 1; i < groups; i++) {
        int len = ovector[2*i+1] - ovector[2*i];
        char *val = str.val + ovector[2*i];
        pstring p = { val, len };
        while(field->group < i) field++; 
        if(field->group == i) {
          doc_buf_field_found(doc, field->alias);
          switch(field->type) {
          case STRING:
            doc_buf_term_found(doc, &p, 0);
            break;
          case INT: {
            int n = pstrtol(&p);
            doc_buf_int_found(doc, n);
            break;
          }
          case TOKENS:
            assert(field->tokenizer);
            field->tokenizer->consume(field->tokenizer, &p);
            break;
          default: 
            assert(0);
          }
        }
      }
      int len = ovector[1] - ovector[0];
      char *val = str.val + ovector[0];
      pstring p = { val, len };
      doc_buf_doc_found(doc, &p);
      parser->on_document->handler(parser->on_document, doc);
      doc_buf_reset(doc);
      vstring_shift(stream, ovector[1]);
    } else if (groups == PCRE_ERROR_PARTIAL) {
      return;
    } else {
      vstring_shift(stream, len);
    }
    len = vstring_size(stream);
  }
}

int
_fldcmp(const void *a, const void *b) {
  return ((FieldMatcher *)a)->group - ((FieldMatcher *)b)->group;
}

Parser *
regex_parser_new(pstring *query) {
  Parser *parser = parser_new();
  parser->buffer = doc_buf_new();
  RegexParserState *state = calloc(1, sizeof(RegexParserState));
  state->parser = parser;
  parser->state = state;
  parser->consume = regex_parser_consume;
  yylex_init(&state->scanner);
  state->lvalp = malloc(sizeof(RXSTYPE));
  state->llocp = malloc(sizeof(RXLTYPE));
  state->stream = vstring_new();
  yyset_extra(state, state->scanner);
  
  state->buf = query;
  rxparse(state);
  if(!state->error) {
    char *cpattern = p2cstring(state->pattern);
    state->regex = pcre_compile(cpattern, 0, &state->re_error, &state->re_error_offset, NULL);
    assert(state->regex);
    // free(cpattern);
    state->extra = pcre_study(state->regex, PCRE_STUDY_JIT_PARTIAL_HARD_COMPILE, &state->re_error);
    assert(state->extra);
    int namecount;
    int _namesize;
    char *table;
    pcre_fullinfo(state->regex, state->extra, PCRE_INFO_NAMECOUNT, &namecount);
    pcre_fullinfo(state->regex, state->extra, PCRE_INFO_NAMEENTRYSIZE, &_namesize);
    pcre_fullinfo(state->regex, state->extra, PCRE_INFO_NAMETABLE, &table);
    
    for (int i = 0; i < namecount; i++) {
      char *str = table + (i * _namesize) + 2;
      int group = *(str - 1);
      pstring *pstr = c2pstring(str);
      bool found = false;
      for(int j = 0; j < state->count; j++) {
        if(!pstrcmp(state->fields[j].name, pstr)) {
          found = true;
          pstring_free(pstr);
          state->fields[j].group = group;
          break;
        }
      }
      if(!found) {
        state->fields[++state->count].name = pstr;
        state->fields[state->count].group = group;
      }
    }
    
    qsort(state->fields, state->count, sizeof(FieldMatcher), _fldcmp);
    
  }
  return parser;
}

extern int yydebug;

Parser *
regex_parser_fresh_copy(Parser *parser) {
  RegexParserState *state = parser->state;
  return regex_parser_new(state->buf);
}

#define yy_create_buffer ada_yy_create_buffer
