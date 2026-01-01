#ifndef DATABASE_H
#define DATABASE_H

int initDb();
void closeDb();
int addProcessDb(int pid, char *name);
int listProcessesAndRemoveInactiveDb();
int removeProcessDb(int pid);

#endif
