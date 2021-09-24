#pragma once
#include <iostream>
#include <vector>

using namespace std;

class GarbageCollectedObject;

//Main GC object (singleton) do all real memory allocation/freeing
//Note: Thread unsafe!
class GarbageCollector
{
private:
	GarbageCollector();
	~GarbageCollector();
	GarbageCollector(const GarbageCollector&);
	GarbageCollector& operator=(GarbageCollector&);

	vector<GarbageCollectedObject*> objects;

	//maximum number of object to manage
	//if this maximum number reached, calling newObject will cause doGarbageCollection() call
	//if there is nothing to destroy, newObject will fail
	unsigned int maxObjects;

public:
	static GarbageCollector& getInstance()
	{
		static GarbageCollector instance;
		return instance;
	}
		
	bool changeMaxObjects(unsigned int maxObjects);
	bool checkMaxObjects();

	void* newObject(size_t size);
	void forceDeleteObject(GarbageCollectedObject* ptr); //forced delete with memory freeing

	vector<GarbageCollectedObject*> currentObjects(); //returns vector of the allocated object ptrs
	void showCurrentObjects(); //returns vector of the allocated object ptrs

	void doGarbageCollection(); //force destroying of all objects marked to delete
};

//Interface for GC-managed obejcts
//All GC-managed objects must be derived from this class
class GarbageCollectedObject
{
private:
	//true if marked for deletion by operator delete
	bool markForDelete;
	
public:
	GarbageCollectedObject() : markForDelete(false) {}
	virtual ~GarbageCollectedObject() { };

	//Overloaded new/delete are being used for pushing ptr into main GC object
	void* operator new (size_t size);
	void operator delete (void* ptr);

	bool isMarkForDelete();
};