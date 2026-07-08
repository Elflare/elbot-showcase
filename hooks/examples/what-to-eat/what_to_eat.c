#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_DISHES 256
#define MAX_LEN 128

void json_escape(const char *src, char *dst, int dst_size) {
    int j = 0;
    for (int i = 0; src[i]; i++) {
        if (src[i] == '"' || src[i] == '\\' || src[i] == '\n' || src[i] == '\r' || src[i] == '\t') {
            if (j >= dst_size - 2) break;
            dst[j++] = '\\';
            switch (src[i]) {
                case '"':  dst[j++] = '"';  break;
                case '\\': dst[j++] = '\\'; break;
                case '\n': dst[j++] = 'n';  break;
                case '\r': dst[j++] = 'r';  break;
                case '\t': dst[j++] = 't';  break;
            }
        } else {
            if (j >= dst_size - 1) break;
            dst[j++] = src[i];
        }
    }
    dst[j] = '\0';
}

int main() {
    // hook.v1: read init frame from stdin (first line only)
    char buf[65536];
    if (fgets(buf, sizeof(buf), stdin) == NULL) {
        // No init frame, proceed anyway
    }

    // Dishes file path relative to plugins/ directory
    FILE *fp = fopen("./what-to-eat/dishes.txt", "r");
    if (!fp) {
        fprintf(stderr, "Cannot open dishes.txt\n");
        printf("{\"type\":\"error\",\"error\":\"Cannot open dishes.txt\"}\n");
        return 0;
    }

    char dishes[MAX_DISHES][MAX_LEN];
    int count = 0;
    while (count < MAX_DISHES && fgets(dishes[count], MAX_LEN, fp)) {
        size_t len = strlen(dishes[count]);
        while (len > 0 && (dishes[count][len - 1] == '\n' || dishes[count][len - 1] == '\r')) {
            dishes[count][len - 1] = '\0';
            len--;
        }
        if (len > 0) {
            count++;
        }
    }
    fclose(fp);

    if (count == 0) {
        fprintf(stderr, "No dishes found\n");
        printf("{\"type\":\"error\",\"error\":\"No dishes found\"}\n");
        return 0;
    }

    srand((unsigned)time(NULL));
    int idx = rand() % count;

    char escaped[MAX_LEN * 2];
    json_escape(dishes[idx], escaped, sizeof(escaped));

    printf("{\"type\":\"output\",\"outputs\":[{\"kind\":\"text\",\"text\":\"今天吃%s！\"}]}\n", escaped);
    printf("{\"type\":\"done\"}\n");

    return 0;
}
