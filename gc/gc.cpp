#include <iostream>
#include <algorithm>
#include "gc.h"

using namespace std;

//GarbageCollector methods
GarbageCollector::GarbageCollector()
{
	maxObjects = 10;
	cout << "GC: INIT, maxObjects = " << maxObjects << endl;
}

GarbageCollector::~GarbageCollector()
{
	cout << "GC: FINAL DESTROY\n";

	for (auto obj : objects)
	{
		cout << "GC: Destroying object ptr = " << obj << endl;
		obj->~GarbageCollectedObject();
		::operator delete(obj);
	}
}

//Returns false if current number of allocated objects is bigger than new maxObjects limit
bool GarbageCollector::changeMaxObjects(unsigned int newMaxObjects)
{
	if (newMaxObjects <= objects.size())
		return false;

	this->maxObjects = newMaxObjects;
	return true;
}

//Should be called before "new" at GarbageCollectorObject
bool GarbageCollector::checkMaxObjects()
{
	if (objects.size() == maxObjects)
	{
		cout << "GC: New allocation: maxObjects limit reached (" << maxObjects << ")\n";
		//Trying to delete some objects
		doGarbageCollection();
		if (objects.size() == maxObjects)
		{
			cout << "GC: Nothing to delete. Allocation failed.\n";
			return false;
		}
	}

	return true;
}

//Pushing already instanciated object to the vector of objects
void* GarbageCollector::newObject(size_t size)
{
	void* ptr = ::operator new(size);
	objects.push_back((GarbageCollectedObject*)ptr);
	cout << "GC: New object in pool ptr = " << ptr << endl;

	return ptr;
}

//forced delete with memory freeing
void GarbageCollector::forceDeleteObject(GarbageCollectedObject* ptr)
{
	if (ptr == nullptr) {
		cout << "GC: Failed to do force deletion of nullptr\n";
		return;
	}

	for (auto it = objects.begin(); it != objects.end(); ++it)
	{
		if (*it == ptr)
		{
			GarbageCollectedObject* obj = *it;
			cout << "GC: Forced destroying object ptr = " << obj << endl;
			obj->~GarbageCollectedObject();
			::operator delete(obj);
			objects.erase(it);
			break;
		}
	}
}

//returns vector of the allocated object ptrs
vector<GarbageCollectedObject*> GarbageCollector::currentObjects()
{
	return objects;
}

void GarbageCollector::showCurrentObjects()
{
	cout << "GC: Current GC-managed objects (maxObjects = " << maxObjects << "):\n";

	int i = 0;
	for (auto obj : objects)
	{
		cout << i++ << ": Object ptr = " << obj << " markToDelete = " << ((obj->isMarkForDelete()) ? "true" : "false") << endl;
	}
}

//force destroying of all objects marked to delete
void GarbageCollector::doGarbageCollection()
{
	cout << "GC: Force garbage collection call (destroying objects marked for deletion)\n";

	while (true)
	{
		auto objToDelete = find_if(objects.begin(), objects.end(),
			[](GarbageCollectedObject* obj) { return obj->isMarkForDelete(); }
		);

		//Nothing else found
		if (objToDelete == objects.end())
			break;
		
		//Delete found object
		GarbageCollectedObject* obj = *objToDelete;
		cout << "GC: Destroying object ptr = " << obj << endl;
		obj->~GarbageCollectedObject();
		::operator delete(obj);
		objects.erase(objToDelete);
	}

}



//GarbageCollectedObject methods

//Overloaded new/delete are being used for pushing ptr into main GC object, 
//instead of real memory allocation/freeing.
void* GarbageCollectedObject::operator new (size_t size)
{	
	GarbageCollector& gc = GarbageCollector::getInstance();

	if (gc.checkMaxObjects())
	{
		void* ptr = gc.newObject(size);
		cout << "GC OBJ: New ptr = " << ptr << endl;

		return ptr;
	}

	return nullptr;
}

//mark an object to deletion from memory
//destructor will be called instead - by design of "delete" operator
void GarbageCollectedObject::operator delete (void* ptr)
{
	//marked to deletion
	((GarbageCollectedObject*)ptr)->markForDelete = true;
	cout << "GC OBJ: Marked to delete ptr " << ptr << endl;	
}

bool GarbageCollectedObject::isMarkForDelete()
{
	return markForDelete;
}