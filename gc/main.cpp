#include <iostream>
#include "gc.h"

using namespace std;

class A : public GarbageCollectedObject
{
public:
	int a;
	char* s;

	A()
	{
		a = 5;
		s = new char[5]{ 'a', 'b', 'c', 'd', '\0'};
	}

	~A()
	{
		if (s)
			delete[] s;
	}

};

int main(int argc, char** argv)
{
	cout << "Hello World!\n";

	A* a = new A();

	cout << "access to s...\n";
	cout << a->s << endl;
	cout << "success!\n";

	
	delete a;

	unique_ptr<A> a_ptr(new A);

	return 0;
}