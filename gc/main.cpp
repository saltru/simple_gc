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
		cout << "A: ctor\n";
		a = 5;
		s = new char[5]{ 'a', 'b', 'c', 'd', '\0'};
	}

	~A()
	{
		cout << "A: dtor\n";
		if (s)
			delete[] s;
	}

};

int main(int argc, char** argv)
{
	cout << "Hello World!\n";

	A* a = new A();

	vector<A*> array;
	for (int i = 0; i < 10; ++i)
	{
		array.push_back(new A);
	}

	GarbageCollector::getInstance().showCurrentObjects();
	
	delete a;

	GarbageCollector::getInstance().forceDeleteObject(array[0]);

	GarbageCollector::getInstance().showCurrentObjects();

	array.push_back(new A);

	GarbageCollector::getInstance().showCurrentObjects();

	unique_ptr<A> a_ptr(new A);

	return 0;
}