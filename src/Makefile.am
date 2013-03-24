bin_PROGRAMS = $(top_builddir)/sit

__top_builddir__sit_SOURCES = main.c      

__top_builddir__sit_LDADD = $(top_builddir)/contrib/libev/libev.la
__top_builddir__sit_LDADD += $(top_builddir)/contrib/pcre/libpcre.la
__top_builddir__sit_LDADD += $(top_builddir)/libsit.la

__top_builddir__libsit_la_LIBADD = $(top_builddir)/contrib/libev/libev.la
__top_builddir__libsit_la_LIBADD += $(top_builddir)/contrib/pcre/libpcre.la

lib_LTLIBRARIES = $(top_builddir)/libsit.la
AM_LIBTOOLFLAGS = 

AM_YFLAGS = -d

__top_builddir__libsit_la_SOURCES =         \
  ast.c               \
  ast.h               \
  callback.c          \
  callback.h          \
  cursor.h            \
  dict.c              \
  dict.h              \
  dict_types.c        \
  dict_types.h        \
  doc_buf.c           \
  doc_buf.h           \
  engine.c            \
  engine.h            \
  input.c             \
  input.h             \
  json_parser.c       \
  json_parser.h       \
  jsonsl.c            \
  jsonsl.h            \
  ll.c                \
  ll.h                \
  logger.c            \
  logger.h            \
  lrw_dict.c          \
  lrw_dict.h          \
  parser.c            \
  parser.h            \
  plist.c             \
  plist.h             \
  protocol.c          \
  protocol.h          \
  pstring.c           \
  pstring.h           \
  query.c             \
  query.h             \
  _query_parser.h     \
  query_parser.y      \
  query_scanner.l     \
  _regex_parser.h     \
  regex_parser.y      \
  regex_scanner.l     \
  regex_tokenizer.c   \
  regex_tokenizer.h   \
  ring_buffer.c       \
  ring_buffer.h       \
  server.c            \
  server.h            \
  sit.h               \
  solr_parser.c       \
  solr_parser.h       \
  syslog_parser.c     \
  syslog_parser.h     \
  task.c              \
  task.h              \
  term.c              \
  term.h              \
  tokenizer.c         \
  tokenizer.h         \
  util.c              \
  util.h              \
  vstring.c           \
  vstring.h           \
  white_tokenizer.c   \
  white_tokenizer.h   

BUILT_SOURCES = query_parser.h regex_parser.h

