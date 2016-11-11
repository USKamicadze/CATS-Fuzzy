#ifndef LOG_H
#define LOG_H

#include <stdio.h>

extern FILE* logFile;
void InitLog();

#define Log(msg) {InitLog(); fprintf(logFile, "%s:%d:%s\n", __FILE__, __LINE__, msg);fflush(logFile);}
#define LogT(t, ...) {InitLog(); fprintf(logFile, "%s:%d:"t"\n", __FILE__, __LINE__, __VA_ARGS__);fflush(logFile);}
#define LogIf(condition, text) if (condition) {Log(text);}

#endif // LOG_H

