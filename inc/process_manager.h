#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#define PID_MAX_LEN 30
#define PID_NAME_MAX_LEN 256
#define PID_CHILDREN_ARRAY_SIZE 20

int processHandler(int mode, char *pid, char *name);
int isProcessActive(int pid);

enum ProcessHandlerMode {
    PROCESS_HANDLER_MODE_UNSET = -1,
    ADD_PROCESS,
    KILL_PROCESS,
    LIST_PROCESSES,
    REMOVE_RECORD
};

typedef struct {
    char *name;
    int rootPid;
    int childrenPids[PID_CHILDREN_ARRAY_SIZE];
    int lastChildIndex;
} Process;

#endif
