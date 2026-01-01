#ifndef VALIDATION_H
#define VALIDATION_H

#define INVALID_PID_MSG "The PID is invalid"
#define INVALID_NAME_MESSAGE "The maximum name length is"
#define MULTIPLE_PROCESS_HANDLING_MODES "You can use only one mode at a time"
#define UNKNOWN_ARGUMENT_MSG "Unknown argument"

int isEmptyStr(char *str);
int isStrNumber(char *str);
int isPidValid(char *pid, int maxLen);
int isProcessNameValid(char *name, int maxLen);
int isProcessHandlerModeAllowed(int mode);

#endif
