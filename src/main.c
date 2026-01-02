#include "args.h"
#include "process_manager.h"
#include "string_utils.h"
#include "validation.h"
#include <stdio.h>
#include <string.h>

int parseArgs(int argc, char **argv);
int copyPidStr(char *destination, char *source, int maxLen, int mode);
int validateProcessHandlerMode(int mode);
int validatePid(char *pid, int maxLen);
void printHelp();

int main(int argc, char **argv) {
    if (parseArgs(argc, argv)) {
        return 1;
    }

    return 0;
}

int parseArgs(int argc, char **argv) {
    enum ProcessHandlerMode mode = PROCESS_HANDLER_MODE_UNSET;
    char pid[PID_MAX_LEN];
    char name[PID_NAME_MAX_LEN];
    char *currentArg, *nextArg;

    makeEmptyString(pid, PID_MAX_LEN);
    makeEmptyString(name, PID_NAME_MAX_LEN);

    for (int i = 1; i < argc; i += 2) {
        currentArg = argv[i];
        nextArg = NULL;

        if (argc > i + 1) {
            nextArg = argv[i + 1];
        }

        if (strcmp(currentArg, HELP_ARG) == 0 ||
            strcmp(currentArg, HELP_FULL_ARG) == 0) {
            printHelp();
            return 0;
        } else if (strcmp(currentArg, ADD_ARG) == 0 ||
                   strcmp(currentArg, ADD_FULL_ARG) == 0) {
            if (copyPidStr(pid, nextArg, PID_MAX_LEN, mode)) {
                return 1;
            }
            mode = ADD_PROCESS;
        } else if (strcmp(currentArg, KILL_ARG) == 0 ||
                   strcmp(currentArg, KILL_FULL_ARG) == 0) {
            if (copyPidStr(pid, nextArg, PID_MAX_LEN, mode)) {
                return 1;
            }
            mode = KILL_PROCESS;
        } else if (strcmp(currentArg, NAME_ARG) == 0 ||
                   strcmp(currentArg, NAME_FULL_ARG) == 0) {
            if (!isProcessNameValid(nextArg, PID_NAME_MAX_LEN)) {
                printf("%s %i\n\n", INVALID_NAME_MESSAGE, PID_NAME_MAX_LEN - 1);
                return 1;
            }
            stripQuotes(nextArg);
            strncpy(name, nextArg, PID_NAME_MAX_LEN - 1);
        } else if (strcmp(currentArg, LIST_ARG) == 0 ||
                   strcmp(currentArg, LIST_FULL_ARG) == 0) {
            if (!isProcessHandlerModeAllowed(mode)) {
                printf("%s\n\n", MULTIPLE_PROCESS_HANDLING_MODES);
                return 1;
            }
            mode = LIST_PROCESSES;
            break;
        } else if (strcmp(currentArg, REMOVE_RECORD_ARG) == 0 ||
                   strcmp(currentArg, REMOVE_RECORD_FULL_ARG) == 0) {
            if (copyPidStr(pid, nextArg, PID_MAX_LEN, mode)) {
                return 1;
            }
            mode = REMOVE_RECORD;
            break;
        }
    }

    if (mode == PROCESS_HANDLER_MODE_UNSET) {
        printf("Provide a valid argument\n");
        return 0;
    }

    processHandler(mode, pid, name);

    return 0;
}

int copyPidStr(char *destination, char *source, int maxLen, int mode) {
    if (validatePid(source, maxLen) || validateProcessHandlerMode(mode)) {
        return 1;
    }
    strncpy(destination, source, maxLen - 1);
    return 0;
}

int validateProcessHandlerMode(int mode) {
    if (!isProcessHandlerModeAllowed(mode)) {
        printf("%s\n\n", MULTIPLE_PROCESS_HANDLING_MODES);
        return 1;
    }
    return 0;
}

int validatePid(char *pid, int maxLen) {
    if (!isPidValid(pid, maxLen)) {
        printf("%s\n\n", INVALID_PID_MSG);
        return 1;
    }
    return 0;
}

void printHelp() {
    printf("\n");
    printf("nhman is a program to manage active processes that were run with "
           "nohup\n");
    printf("\n");
    printf("Arguments:\n");
    printf("\t--help (-h) : Print the help menu\n");
    printf("\t--kill (-k) {PID} : Pass a PID to kill child processes and the "
           "root process\n");
    printf("\t--add (-a) {PID} : Pass a PID of a process to add it to the list "
           "of processes\n");
    printf("\t--name (-n) {name} : Used with the --add flag to add a name or "
           "description to a process\n");
    printf("\t--list (-l) : Print the list of active added processes (inactive "
           "processes are removed from the list automatically)\n");
    printf("\t--remove-record (-rr) {PID} : Remove a process record from the "
           "list\n");
    printf("\n");
}
