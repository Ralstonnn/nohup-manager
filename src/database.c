#include "args.h"
#include "process_manager.h"
#include "string_utils.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#define DB_FILE_LOCATION ".local/share/nhman"
#define DB_FILE_NAME "data"
#define DB_TMP_FILE_NAME_EXTENSION "__tmp"

char *dbFilePath = NULL;
const int RECORD_MAX_LEN = PID_MAX_LEN + PID_NAME_MAX_LEN + 10;

int isProcessInDb(int pid);
int createDirectoryRecursive(char *dirPath);
void getProcessFromRecordString(char *pid, char *name, char *source);
void printProcessFormatted(char *pid, char *name);
int removeProcessDb(int pid);

int initDb() {
    char *dbPathDir = NULL;
    asprintf(&dbPathDir, "%s/%s", getenv("HOME"), DB_FILE_LOCATION);

    if (createDirectoryRecursive(dbPathDir)) {
        free(dbPathDir);
        printf("ERROR OPENING DB DIRECTORY\n");
        return 1;
    }

    asprintf(&dbFilePath, "%s/%s", dbPathDir, DB_FILE_NAME);
    free(dbPathDir);
    return 0;
}

void closeDb() {
    if (dbFilePath != NULL) {
        free(dbFilePath);
    }
}

int addProcessDb(int pid, char *name) {
    if (isProcessInDb(pid)) {
        printf("A process with the pid already exists in db\n");
        printf("You can remove the process with \"%s {pid}\"\n\n",
               REMOVE_RECORD_ARG);
        return 1;
    }

    FILE *fp = NULL;
    fp = fopen(dbFilePath, "a");

    if (fp == NULL) {
        return 1;
    }

    if (name == NULL)
        fprintf(fp, "%i\n", pid);
    else
        fprintf(fp, "%i \"%s\"\n", pid, name);

    fclose(fp);
    return 0;
}

int isProcessInDb(int pid) {
    FILE *fp = NULL;
    fp = fopen(dbFilePath, "r");

    if (fp == NULL) {
        return 0;
    }

    char outputBuffer[RECORD_MAX_LEN], pidBuffer[PID_MAX_LEN];
    makeEmptyString(outputBuffer, RECORD_MAX_LEN);

    while ((fgets(outputBuffer, RECORD_MAX_LEN, fp)) != NULL) {
        makeEmptyString(pidBuffer, PID_MAX_LEN);
        getProcessFromRecordString(pidBuffer, NULL, outputBuffer);

        if (strToInt(pidBuffer) == pid) {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

int listProcessesAndRemoveInactiveDb() {
    FILE *fp, *fpTmp;
    char *tmpFilePath = NULL;

    asprintf(&tmpFilePath, "%s%s", dbFilePath, DB_TMP_FILE_NAME_EXTENSION);
    fp = fopen(dbFilePath, "r");
    fpTmp = fopen(tmpFilePath, "w");

    if (fp == NULL || fpTmp == NULL) {
        if (fp != NULL)
            fclose(fp);
        if (fpTmp != NULL)
            fclose(fpTmp);

        free(tmpFilePath);
        return 1;
    }

    char outputBuffer[RECORD_MAX_LEN], pidBuffer[PID_MAX_LEN],
        nameBuffer[PID_NAME_MAX_LEN];
    makeEmptyString(outputBuffer, RECORD_MAX_LEN);

    while ((fgets(outputBuffer, RECORD_MAX_LEN, fp)) != NULL) {
        makeEmptyString(pidBuffer, PID_MAX_LEN);
        makeEmptyString(nameBuffer, PID_NAME_MAX_LEN);
        getProcessFromRecordString(pidBuffer, nameBuffer, outputBuffer);

        if (!isProcessActive(strToInt(pidBuffer)))
            continue;

        printProcessFormatted(pidBuffer, nameBuffer);
        fprintf(fpTmp, "%s", outputBuffer);
    }

    fclose(fp);
    fclose(fpTmp);

    if (remove(dbFilePath) != 0) {
        free(tmpFilePath);
        return 1;
    }

    if (rename(tmpFilePath, dbFilePath) != 0) {
        free(tmpFilePath);
        return 1;
    }

    free(tmpFilePath);
    return 0;
}

void printProcessFormatted(char *pid, char *name) {
    if (pid == NULL || *pid == '\0')
        return;

    if (name != NULL && *name != '\0')
        printf("PID: %s   NAME: %s\n", pid, name);
    else
        printf("PID: %s\n", pid);
}

int removeProcessDb(int pid) {
    FILE *fp, *fpTmp;
    char *tmpFilePath = NULL;
    asprintf(&tmpFilePath, "%s%s", dbFilePath, DB_TMP_FILE_NAME_EXTENSION);
    fp = fopen(dbFilePath, "r");
    fpTmp = fopen(tmpFilePath, "w");

    if (fp == NULL || fpTmp == NULL) {
        if (fp != NULL)
            fclose(fp);
        if (fpTmp != NULL)
            fclose(fpTmp);

        free(tmpFilePath);
        return 1;
    }

    char outputBuffer[RECORD_MAX_LEN], pidBuffer[PID_MAX_LEN];
    makeEmptyString(outputBuffer, RECORD_MAX_LEN);

    while ((fgets(outputBuffer, RECORD_MAX_LEN, fp)) != NULL) {
        makeEmptyString(pidBuffer, PID_MAX_LEN);
        getProcessFromRecordString(pidBuffer, NULL, outputBuffer);

        if (strToInt(pidBuffer) == pid)
            continue;

        fprintf(fpTmp, "%s", outputBuffer);
    }

    fclose(fp);
    fclose(fpTmp);

    if (remove(dbFilePath) != 0) {
        free(tmpFilePath);
        return 1;
    }

    if (rename(tmpFilePath, dbFilePath) != 0) {
        free(tmpFilePath);
        return 1;
    }

    free(tmpFilePath);
    return 0;
}

void getProcessFromRecordString(char *pid, char *name, char *source) {
    if (source == NULL) {
        return;
    }

    if (pid != NULL) {
        for (; *source != '\0' && *source != '\n'; source++) {
            if (*source == ' ') {
                source++;
                break;
            }
            *pid = *source;
            pid++;
        }
        *pid = '\0';
    }

    if (name != NULL) {
        for (; *source != '\0' && *source != '\n'; source++) {
            if (*source == '"')
                continue;
            *name = *source;
            name++;
        }
        *name = '\0';
    }
}

int createDirectoryRecursive(char *dirPath) {
    char *tmp = strdup(dirPath);
    char *p = NULL;
    mode_t mode = S_IRWXU;

    for (p = (tmp + 1); *p; p++) {
        if (*p == '/') {
            *p = '\0';

            if (mkdir(tmp, mode) != 0) {
                if (errno != EEXIST) {
                    free(tmp);
                    return 1;
                }
            }

            *p = '/';
        }
    }

    if (mkdir(tmp, mode) != 0) {
        if (errno != EEXIST) {
            free(tmp);
            return 1;
        }
    }

    free(tmp);
    return 0;
}
