#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <errno.h>
#include <json-c/json.h>
#include <stdbool.h>

#include "type.h"

kanji_s* init(char*  argv[]){
        kanji_s *ks;
        ks = malloc(sizeof(*ks));

        CURL *ch;
        ch = curl_easy_init();
        if(!ch){
                free(ks);
                return NULL;
        }
        ks->ch = ch;
        ks->data = malloc(sizeof(char));
        ks->size = 0;
        ks->kanji = argv[1];
        
}

static size_t write_curl_callback(void *konten, size_t ukuran, size_t nmemb, void *userdata)
{
        size_t ukuranasli = ukuran * nmemb;
        kanji_s *ks = (kanji_s*)userdata;
        char *resize_memory_asal = realloc(ks->data, ks->size + ukuranasli + 1);
        if(!resize_memory_asal)
        {
                printf("memori tidak cukup\n");
                return -ENOMEM;
        }
        
        ks->data = resize_memory_asal;
        memcpy(&(ks->data[ks->size]), konten, ukuranasli);
        ks->size += ukuranasli;
        ks->data[ks->size] = 0;
        return ukuranasli;
}

/*
 * sumber : https://github.com/php/php-src/blob/23961ef382e1005db6f8c08f3ecc0002839388a7/ext/standard/url.c#L459-L555
 */
char *urlencode(char *alloc, const char *s, size_t len, bool raw)
{
	static const unsigned char hexchars[] = "0123456789ABCDEF";
	register unsigned char c;
	unsigned char *to;
	unsigned char const *from, *end;
	char *start;

	from = (const unsigned char *)s;
	end = (const unsigned char *)s + len;

	if (alloc == NULL) {
		start = malloc((len * 3ul) + 1ul);
	} else {
		start = alloc;
	}

	to = (unsigned char *)start;

	while (from < end) {
		c = *from++;

		if (!raw && c == ' ') {
			*to++ = '+';
		} else if ((c < '0' && c != '-' && c != '.') ||
				(c < 'A' && c > '9') ||
				(c > 'Z' && c < 'a' && c != '_') ||
				(c > 'z' && (!raw || c != '~'))) {
			to[0] = '%';
			to[1] = hexchars[c >> 4];
			to[2] = hexchars[c & 15];
			to += 3;
		} else {
			*to++ = c;
		}
	}
	*to = '\0';

	return start;
}

size_t strlen_teks(const char *str)
{
	size_t return_str = 0;

	while (*str) {
		str++;
		return_str++;
	}

	return return_str;
}

void execcurl(kanji_s *ks)
{
        char escape_kanji[1024];
        char url[1024];
        CURL *ch = ks->ch;
        CURLcode response;

        


        //debug
        
        curl_global_init(CURL_GLOBAL_ALL);
        ch = curl_easy_init();

        // snprintf(concat_kanji, sizeof(concat_kanji), "%s%s", ks->kanji, "\0");
        
        // char *output = curl_easy_escape(ch, concat_kanji, sizeof(concat_kanji));
        // printf("%s", output);
        // return 0;
        // snprintf(url, sizeof(url), "%s%s", "https://jisho.org/api/v1/search/words?keyword=", output);
        //memcpy(url, "https://jisho.org/api/v1/search/words?keyword=", 46);
        urlencode(escape_kanji, ks->kanji, strlen(ks->kanji), false);
        snprintf(url, sizeof(url), "%s%s", "https://jisho.org/api/v1/search/words?keyword=", escape_kanji);
        // printf("%s", url);
        // return 0;
        

        curl_easy_setopt(ch, CURLOPT_URL, url);
        curl_easy_setopt(ch, CURLOPT_WRITEFUNCTION, write_curl_callback);
        curl_easy_setopt(ch, CURLOPT_WRITEDATA, ks);
        response = curl_easy_perform(ch);
        // if(response != CURLE_OK){ 
        //         printf("curl error"); 
        // }else{
        //         printf("data: %s\n", ks->data);
        // }
        curl_easy_cleanup(ch);
        curl_global_cleanup();
}

void destroy_all(kanji_s* ks)
{
        free(ks->data);
        free(ks);
}

void parse_and_show(kanji_s* ks)
{
        int jsonlen_japanese;
        json_object *root_json_jisho = (json_object*)malloc(sizeof(json_tokener_parse(ks->data)));
        root_json_jisho = json_tokener_parse(ks->data);
        json_object *data = json_object_object_get(root_json_jisho, "data");
        json_object *data_root = json_object_array_get_idx(data, 0);

        printf("\n\n===============\n");
        data = json_object_object_get(data_root, "slug");
        printf("Kanji : %s\n", json_object_get_string(data));
        
        json_object *jp = json_object_object_get(data_root, "japanese");
        jsonlen_japanese = json_object_array_length(jp);
        for (int i = 0; i < jsonlen_japanese; i++) {
            json_object *japanese_hiragana_read = json_object_array_get_idx(jp, i);
            json_object *reading = json_object_object_get(japanese_hiragana_read, "reading");
            json_object *word = json_object_object_get(japanese_hiragana_read, "word");
            printf("~ (%s) : %s\n", json_object_get_string(word), json_object_get_string(reading));
        }

        printf("\nmeaning: \n");
        jp = json_object_object_get(data_root, "senses");
        jsonlen_japanese = json_object_array_length(jp);
        for (int i = 0; i < jsonlen_japanese; i++) {
            json_object *senses = json_object_array_get_idx(jp, i);
            json_object *def = json_object_object_get(senses, "english_definitions");
            printf("~ %s\n", json_object_get_string(def));
        }
        free(root_json_jisho);


        // data = json_object_object_get(data_root, "japanese");
        // data = json_object_array_get_idx(data, 0);
        // data = json_object_object_get(data, "reading");
        // printf("Hiragana : %s\n", json_object_get_string(data));


}

