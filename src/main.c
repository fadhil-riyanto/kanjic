#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <errno.h>
#include <json-c/json.h>
#include <stdbool.h>

#include "type.h"

int main(int argc, char *argv[])
{
        kanji_s *ks;
        ks = init(argv);
        execcurl(ks);
        parse_and_show(ks);
        destroy_all(ks);
        return 0;
}