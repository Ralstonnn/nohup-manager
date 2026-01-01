#include "process_manager.h"
#include <ctype.h>
#include <string.h>

int isEmptyStr(char *str) {
    if (str == NULL)
        return 1;

    int length = (int)strlen(str);

    for (int i = 0; i < length; i++) {
        if (str[i] != '\0')
            return 0;
    }
    return 1;
}

int isStrNumber(char *str) {
    if (str == NULL || *str == '\0') {
        return 0;
    }

    while (*str != '\0') {
        if (!isdigit((unsigned char)*str)) {
            return 0;
        }
        str++;
    }

    return 1;
}

int isPidValid(char *pid, int maxLen) {
    if (pid == NULL || !isStrNumber(pid) || (int)strlen(pid) > maxLen - 1) {
        return 0;
    }
    return 1;
}

int isProcessNameValid(char *name, int maxLen) {
    if ((int)strlen(name) > maxLen - 1) {
        return 0;
    }
    return 1;
}

int isProcessHandlerModeAllowed(int mode) {
    if (mode != PROCESS_HANDLER_MODE_UNSET) {
        return 0;
    }
    return 1;
}
