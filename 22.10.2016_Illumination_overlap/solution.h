#ifndef __SOLUTION_H__
#define __SOLUTION_H__


#include <stdio.h>
extern FILE* _fpin;
extern FILE* _fpout;
#include <iostream>
#include <fstream>
#define cin _mycin
extern std::ifstream _mycin;
#define cout _mycout
extern std::ofstream _mycout;

int solve();


#endif