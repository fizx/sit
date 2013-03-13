#ifndef SOLR_PARSER_H_INCLUDED
#define SOLR_PARSER_H_INCLUDED

Parser *
solr_parser_new();

void
solr_parser_free(void *parser);

Parser *
solr_fresh_copy(Parser *parser);

#endif

