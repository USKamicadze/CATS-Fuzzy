#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include "checker.h"

static int64_t a, b, s, r;
static FILE* logfile;
static FILE* in;
static FILE* out;

int check(){
    logfile = fopen("log.txt", "a");
    in = fopen("input.txt", "r");
    out = fopen("output.txt", "r");
    fscanf(in, "%"PRId64 "%"PRId64, &a, &b);
    if (a % b == 0)
        s = 2 * (a / b - 1);
    else
        s = 2 * (a / b);
    if (a == b)
        s = 1;
    fscanf(out, "%" PRId64, &r);
    fprintf(logfile, "input: %"PRId64" %"PRId64" result:%"PRId64" expect:%"PRId64"\n",a,b, r, s);
    assert(r == s);
    return 0;
}
