#include "CommonIncludes.h"

void printIndex(int i);

struct ClassWithPrintIndexMethod {  
	void operator() (int i) { std::cout << ' ' << i; }
};
