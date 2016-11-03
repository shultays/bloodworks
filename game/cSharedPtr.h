#pragma once


#include "cTools.h"


template <class T>
class cCustomDeallocator {
public:
	virtual void objectFreed(T* object) = 0;
};


template <class T>
class cSharedPtr {
	class cSharedPtrCounter {
		int referenceCount;
	public:
		cSharedPtrCounter() {
			referenceCount = 0;
		}

		int getCount() const {
			return referenceCount;
		}

		void increment() {
			referenceCount++;
		}

		void decrement() {
			assert(referenceCount);
			referenceCount--;
		}
	};


	T* object;
	cCustomDeallocator<T> *deallocator;
	cSharedPtrCounter *counter;

	void reset() 
	{
		if (counter) 
		{
			counter->decrement();

			if (counter->getCount() == (deallocator ? 1 : 0)) 
			{
				if (deallocator) 
				{
					deallocator->objectFreed(object);
				}
				else 
				{
					SAFE_DELETE(object);
					SAFE_DELETE(counter);
				}
			}
		}
		object = nullptr;
		counter = nullptr;
		deallocator = nullptr;
	}

public:
	cSharedPtr() 
	{
		this->object = nullptr;
		this->counter = nullptr;
		this->deallocator = nullptr;
	}

	~cSharedPtr() {
		reset();
	}

	cSharedPtr(const cSharedPtr<T>& other) {
		this->object = nullptr;
		this->counter = nullptr;
		this->deallocator = nullptr;
		*this = other;
	}
	cSharedPtr(cSharedPtr<T>&& other) {
		this->object = nullptr;
		this->counter = nullptr;
		this->deallocator = nullptr;
		*this = other;
	}

	cSharedPtr(T* object) {
		this->object = nullptr;
		this->counter = nullptr;
		this->deallocator = nullptr;
		*this = object;
	}

	cSharedPtr<T>& operator=(const T*& object) {
		if (object != nullptr) {
			this->object = object;
			counter = new cSharedPtrCounter();
			counter->increment();
		}
		return *this;
	}

	cSharedPtr<T>& operator=(T* object) {
		reset();
		if (object != nullptr) {
			this->object = object;
			counter = new cSharedPtrCounter();
			counter->increment();
		}
		return *this;
	}

	cSharedPtr<T>& operator=(const cSharedPtr<T>& other) {
		if (this != &other) {
			reset();
			object = other.object;
			counter = other.counter;
			deallocator = other.deallocator;
			if (counter) {
				counter->increment();
			}
		}
		return *this;
	}

	cSharedPtr<T>& operator=(cSharedPtr<T>&& other) {
		object = other.object;
		counter = other.counter;
		deallocator = other.deallocator;

		if (counter) {
			counter->increment();
		}
		return *this;
	}

	T* operator->() {
		return object;
	}

	T* getObject() {
		return object;
	}

	void setCustomDeallocator(cCustomDeallocator<T> *deallocator) {
		this->deallocator = deallocator;
	}

	bool operator==(const cSharedPtr<T>& other) const
	{
		return this->object == other.object;
	}

	bool operator!=(const cSharedPtr<T>& other) const
	{
		return this->object != other.object;
	}
};
