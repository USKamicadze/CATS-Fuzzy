#include "solution.h"
#include "checker.h"

#include <stdio.h>
FILE* _fpin;
FILE* _fpout;
#include <iostream>
#include <fstream>
std::ifstream _mycin;
std::ofstream _mycout;

int main(){
	solve();
	fclose(_fpin);
	fclose(_fpout);
	_mycin.close();
    _mycout.close();
	check();
}