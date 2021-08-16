void printIndex(int i);


struct ClassWithPrintIndexMethod {           // function object type:
	void operator() (int i) { std::cout << ' ' << i; }
};