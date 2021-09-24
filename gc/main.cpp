#include <iostream>
#include "gc.h"

class A : public GarbageCollectedObject
{
public:
	int a;
	char* s;

	A()
	{
		cout << "A: ctor ptr = " << this << "\n";
		a = 5;
		s = new char[5]{ 'a', 'b', 'c', 'd', '\0'};
	}

	~A()
	{
		cout << "A: dtor ptr = " << this << "\n";
		if (s)
			delete[] s;
	}

	void printData()
	{
		//Check if this object invalidated
		if (isMarkForDelete())
		{
			cout << "Object type A ptr = " << this << " INVALIDATED!\n";
			return;
		}

		cout << "Object type A ptr = " << this << " contains:\n";
		cout << "int a = " << this->a << endl;
		cout << "char[] s = " << this->s << endl;
	}
};

int main(int argc, char** argv)
{
	cout << "Hello World!\n";

	cout << endl;

	//Create single A
	A* a = new A();

	cout << endl;
	a->printData();

	cout << endl;
	//Create array of 10 A
	vector<A*> array;
	for (int i = 0; i < 10; ++i)
	{
		array.push_back(new A);
	}

	cout << endl;
	//Show current allocated objects
	GarbageCollector::getInstance().showCurrentObjects();
	
	cout << endl;
	//Try to delete a, dtor is called, but memory has not freed
	delete a;
	a->printData();

	cout << endl;
	//Forced deletion, dtor + memory free
	GarbageCollector::getInstance().forceDeleteObject(array[0]);

	cout << endl;
	GarbageCollector::getInstance().showCurrentObjects();

	cout << endl;
	//+1 object in pool
	array.push_back(new A);

	cout << endl;
	GarbageCollector::getInstance().showCurrentObjects();

	cout << endl;
	//+1 object in pool, GC will free first 'a', marked to deletion
	unique_ptr<A> a_ptr(new A);

	cout << endl;
	return 0;
}