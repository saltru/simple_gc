#pragma once
#include <iostream>
#include <vector>

using namespace std;

class GarbageCollectedObject;

//Main GC object (singleton) do all real memory allocation/freeing
//Now implemented as classic Meyers' signleton (thread unsafe)
//TODO: thread safety
class GarbageCollector
{
private:
	GarbageCollector();
	GarbageCollector(const GarbageCollector&);
	//GarbageCollector& operator=(GarbageCollector&);

	vector<GarbageCollectedObject*> objects;

public:
	static GarbageCollector& getInstance()
	{
		static GarbageCollector instance;
		return instance;
	}

	void newObject(GarbageCollectedObject* obj);
	void deleteObject(GarbageCollectedObject* obj); //just mark for deletion
	void forceDeleteObject(GarbageCollectedObject* obj); //forced delete with memory freeing

	vector<GarbageCollectedObject*> currentObjects(); //returns vector of the allocated object ptrs

	~GarbageCollector();
};

//Interface for GC-managed obejcts
//All GC-managed objects must be derived from this class
class GarbageCollectedObject
{
private:
	//true if marked for deletion by operator delete
	bool canBeDeleted;
public:
	GarbageCollectedObject() : canBeDeleted(false) {}
	virtual ~GarbageCollectedObject() {};

	//Overloaded new/delete are being used for pushing ptr into main GC object, 
	//instead of real memory allocation/freeing.
	void* operator new (size_t size);
	void operator delete (void* ptr);
};