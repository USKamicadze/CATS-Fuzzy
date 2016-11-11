#include <stdio.h>

#include "log.h"

FILE* logFile = NULL;

void InitLog() {
    if (logFile == NULL) {
        logFile = fopen("postlog.txt", "w");
    }
}
