#include <stddef.h>
#include <string.h>

void stripEnter(char *str) {
    if (str == NULL) {
        return;
    }
    while (*str != '\0') {
        if (*str == '\n') {
            *str = '\0';
            return;
        }
        str++;
    }
}

void stripQuotes(char *str) {
    int length = (int)strlen(str);
    char tmp[length];
    strcpy(tmp, str);

    int j = 0;
    for (int i = 0; i < length; i++) {
        if (tmp[i] == '\'' || tmp[i] == '"')
            continue;
        str[j++] = tmp[i];
    }

    str[j] = '\0';
}

void makeEmptyString(char *str, int size) {
    for (int i = 0; i < size; i++)
        str[i] = '\0';
}

int strToInt(char *str) {
    int result = 0;

    while (*str != '\0') {
        result = result * 10 + *str - '0';
        str++;
    }

    return result;
}
