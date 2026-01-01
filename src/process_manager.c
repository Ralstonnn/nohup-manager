#include "process_manager.h"
#include "database.h"
#include "string_utils.h"
#include "validation.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int addProcess();
void killProcess();
int initProcessStruct(char *rootPid, char *name);
int initChildProcesses();
void cleanupProcessStruct();

Process currentProcess;

int processHandler(int mode, char *pid, char *name) {
    int returnValue = 0;

    initDb();

    switch (mode) {
    case ADD_PROCESS:
        if (initProcessStruct(pid, name)) {
            returnValue = 1;
            break;
        }
        returnValue = addProcess();
        cleanupProcessStruct();
        break;
    case KILL_PROCESS:
        if (initProcessStruct(pid, name)) {
            returnValue = 1;
            break;
        }
        killProcess();
        cleanupProcessStruct();
        break;
    case LIST_PROCESSES:
        returnValue = listProcessesAndRemoveInactiveDb();
        break;
    case REMOVE_RECORD:
        returnValue = removeProcessDb(strToInt(pid));
        if (returnValue)
            printf("There was an error removing record PID: %s\n", pid);
        else
            printf("Record PID: %s was removed from the list\n", pid);
        break;
    }

    closeDb();
    return returnValue;
}

int addProcess() {
    if (addProcessDb(currentProcess.rootPid, currentProcess.name)) {
        printf("THERE WAS AN ERROR ADDING THE PROCESS\n");
        return 1;
    }

    if (isEmptyStr(currentProcess.name))
        printf("Process PID: %i was added\n", currentProcess.rootPid);
    else
        printf("Process PID: %i, name: %s was added\n", currentProcess.rootPid,
               currentProcess.name);

    return 0;
}

void killProcess() {
    removeProcessDb(currentProcess.rootPid);

    int currentPid;

    for (int i = currentProcess.lastChildIndex; i >= 0; i--) {
        currentPid = currentProcess.childrenPids[i];
        if (!isProcessActive(currentPid)) {
            continue;
        }

        kill(currentPid, 9);
    }

    if (!isProcessActive(currentProcess.rootPid)) {
        printf("Process PID: %i was killed\n", currentProcess.rootPid);
        return;
    }

    printf("Process PID: %i was killed\n", currentProcess.rootPid);
    kill(currentProcess.rootPid, 9);
}

int initProcessStruct(char *rootPid, char *name) {
    if (!isStrNumber(rootPid)) {
        printf("%s\n", INVALID_PID_MSG);
        return 1;
    }

    currentProcess.rootPid = strToInt(rootPid);
    currentProcess.lastChildIndex = -1;

    if (name != NULL && !isEmptyStr(name))
        currentProcess.name = strdup(name);
    else
        currentProcess.name = NULL;

    initChildProcesses();

    return 0;
}

int initChildProcesses() {
    FILE *fp;
    char *childrenFilePath;

    asprintf(&childrenFilePath, "/proc/%i/task/%i/children",
             currentProcess.rootPid, currentProcess.rootPid);
    fp = fopen(childrenFilePath, "r");

    free(childrenFilePath);

    if (fp == NULL) {
        return 1;
    }

    int i = 0;
    char output_char;

    while (1) {
        output_char = fgetc(fp);

        if (feof(fp) || i >= PID_CHILDREN_ARRAY_SIZE)
            break;

        if (output_char == ' ') {
            currentProcess.lastChildIndex = i;
            i++;
            continue;
        }

        currentProcess.childrenPids[i] =
            currentProcess.childrenPids[i] * 10 + output_char - '0';
    }

    fclose(fp);
    return 0;
}

void cleanupProcessStruct() {
    if (currentProcess.name != NULL) {
        free(currentProcess.name);
        currentProcess.name = NULL;
    }
}

int isProcessActive(int pid) { return kill(pid, 0) == 0; }
