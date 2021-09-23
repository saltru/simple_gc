#include <iostream>>
#include "gc.h"

using namespace std;

//GarbageCollector methods
GarbageCollector::GarbageCollector()
{
	cout << "GC: INIT\n";
}

GarbageCollector::~GarbageCollector()
{
	for (auto obj : objects)
	{
		cout << "GC: Destroying object ptr = " << (size_t)obj << endl;
		::operator delete(obj);
	}
	cout << "GC: FINAL DESTROY\n";
}

void GarbageCollector::newObject(GarbageCollectedObject* obj)
{
	objects.push_back(obj);
	cout << "GC: New object " << objects.size() << " ptr = " << (size_t)obj << endl;
}



//GarbageCollectedObject methods
void* GarbageCollectedObject::operator new (size_t size)
{	
	GarbageCollector& gc = GarbageCollector::getInstance();

	void* ptr = ::operator new(size);
	gc.newObject((GarbageCollectedObject*)ptr);

	cout << "GC OBJ: New ptr = " << (size_t)ptr << endl;
	
	return ptr;
}

void GarbageCollectedObject::operator delete (void* ptr)
{
	cout << "GC OBJ: Marked to delete ptr " << (size_t)ptr << endl;
	//marked to deletion
}