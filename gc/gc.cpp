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
		cout << "GC: Destroying object ptr = " << (size_t)obj << endl;
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
}

//Should be called before "new" at GarbageCollectorObject
bool GarbageCollector::checkMaxObjects()
{
	if (objects.size() == maxObjects)
	{
		cout << "GC: maxObjects limit reached (" << maxObjects << ")";
		//Trying to delete some objects
		doGarbageCollection();
		if (objects.size() == maxObjects)
		{
			cout << "GC: Nothing to delete\n";
			return false;
		}
	}

	return true;
}

//Pushing already instanciated object to the vector of objects
void GarbageCollector::newObject(GarbageCollectedObject* ptr)
{
	objects.push_back(ptr);
	cout << "GC: New object " << objects.size() << " ptr = " << (size_t)ptr << endl;
}

//forced delete with memory freeing
void GarbageCollector::forceDeleteObject(GarbageCollectedObject* ptr)
{
	if (ptr == nullptr) {
		cout << "GC: Trying to force deletion of nullptr\n";
		return;
	}

	for (auto it = objects.begin(); it != objects.end(); ++it)
	{
		if (*it == ptr)
		{
			GarbageCollectedObject* obj = *it;
			cout << "GC: Destroying object ptr = " << (size_t)obj << endl;
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
	for (auto obj : objects)
	{
		cout << "Object ptr = " << (size_t)obj << " toDelete = " << ((obj->isMarkForDelete()) ? "true" : "false") << endl;
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

		//Nothing found
		if (objToDelete == objects.end())
			break;
		
		//Delete found object
		GarbageCollectedObject* obj = *objToDelete;
		cout << "GC: Destroying object ptr = " << (size_t)obj << endl;
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

		void* ptr = ::operator new(size);
		gc.newObject((GarbageCollectedObject*)ptr);

		cout << "GC OBJ: New ptr = " << (size_t)ptr << endl;

		return ptr;
	}

	return nullptr;
}

//just mark an object to deletion
void GarbageCollectedObject::operator delete (void* ptr)
{
	//marked to deletion
	((GarbageCollectedObject*)ptr)->markForDelete = true;
	cout << "GC OBJ: Marked to delete ptr " << (size_t)ptr << endl;	
}

bool GarbageCollectedObject::isMarkForDelete()
{
	return markForDelete;
}