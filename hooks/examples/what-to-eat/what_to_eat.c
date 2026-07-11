#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_DISHES 256
#define MAX_LEN 128

static void json_escape(const char *src, char *dst, size_t size) {
    size_t j = 0;
    for (size_t i = 0; src[i] && j + 2 < size; i++) {
        switch (src[i]) {
        case '"': dst[j++] = '\\'; dst[j++] = '"'; break;
        case '\\': dst[j++] = '\\'; dst[j++] = '\\'; break;
        case '\n': dst[j++] = '\\'; dst[j++] = 'n'; break;
        case '\r': dst[j++] = '\\'; dst[j++] = 'r'; break;
        case '\t': dst[j++] = '\\'; dst[j++] = 't'; break;
        default: dst[j++] = src[i];
        }
    }
    dst[j] = '\0';
}

static int json_string(const char *json, const char *key, char *out, size_t size) {
    char needle[64];
    snprintf(needle, sizeof(needle), "\"%s\"", key);
    const char *p = strstr(json, needle);
    if (!p) return 0;
    p += strlen(needle);
    while (isspace((unsigned char)*p)) p++;
    if (*p++ != ':') return 0;
    while (isspace((unsigned char)*p)) p++;
    if (*p++ != '"') return 0;

    size_t j = 0;
    while (*p && *p != '"' && j + 1 < size) {
        if (*p == '\\' && p[1]) {
            p++;
            switch (*p) {
            case 'n': out[j++] = '\n'; break;
            case 'r': out[j++] = '\r'; break;
            case 't': out[j++] = '\t'; break;
            default: out[j++] = *p;
            }
        } else {
            out[j++] = *p;
        }
        p++;
    }
    out[j] = '\0';
    return *p == '"';
}

static void response(const char *id, const char *result) {
    char escaped_id[256];
    json_escape(id, escaped_id, sizeof(escaped_id));
    printf("{\"type\":\"response\",\"id\":\"%s\",\"ok\":true,\"result\":%s}\n", escaped_id, result);
    fflush(stdout);
}

int main(void) {
    char line[65536];
    while (fgets(line, sizeof(line), stdin)) {
        char id[256] = "";
        char method[64] = "";
        json_string(line, "id", id, sizeof(id));
        json_string(line, "method", method, sizeof(method));

        if (strcmp(method, "system.init") == 0) {
            response(id, "{}");
            continue;
        }
        if (strcmp(method, "system.shutdown") == 0) {
            response(id, "{}");
            return 0;
        }
        if (strcmp(method, "event.handle") != 0) {
            printf("{\"type\":\"response\",\"id\":\"%s\",\"ok\":false,\"error\":\"unsupported method\"}\n", id);
            fflush(stdout);
            return 0;
        }

        FILE *fp = fopen("dishes.txt", "r");
        if (!fp) {
            fprintf(stderr, "Cannot open dishes.txt\n");
            printf("{\"type\":\"response\",\"id\":\"%s\",\"ok\":false,\"error\":\"Cannot open dishes.txt\"}\n", id);
            return 0;
        }

        char dishes[MAX_DISHES][MAX_LEN];
        int count = 0;
        while (count < MAX_DISHES && fgets(dishes[count], MAX_LEN, fp)) {
            size_t len = strlen(dishes[count]);
            while (len > 0 && (dishes[count][len - 1] == '\n' || dishes[count][len - 1] == '\r')) {
                dishes[count][--len] = '\0';
            }
            if (len > 0) count++;
        }
        fclose(fp);

        if (count == 0) {
            printf("{\"type\":\"response\",\"id\":\"%s\",\"ok\":false,\"error\":\"No dishes found\"}\n", id);
            return 0;
        }

        srand((unsigned)time(NULL));
        char dish[MAX_LEN * 2];
        json_escape(dishes[rand() % count], dish, sizeof(dish));
        char result[MAX_LEN * 2 + 128];
        snprintf(result, sizeof(result), "{\"status\":\"completed\",\"outputs\":[{\"kind\":\"text\",\"text\":\"今天吃%s！\"}]}", dish);
        response(id, result);
        return 0;
    }
    return 0;
}
