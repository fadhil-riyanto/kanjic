#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <errno.h>
#include <json-c/json.h>
#include <stdbool.h>

#ifndef USRCONST
# define USRCONST const
#endif

typedef struct _kanji_t {
        USRCONST CURL       *ch;
        char                *data;
        char                *kanji;
        size_t              size;
} kanji_s;

typedef struct parsed_data_t {

} parsed_data;

kanji_s* init(char*  argv[]);
void execcurl(kanji_s *ks);
void destroy_all(kanji_s* ks);
void parse_and_show(kanji_s* ks);
size_t strlen_teks(const char *str);
char *urlencode(char *alloc, const char *s, size_t len, bool raw);