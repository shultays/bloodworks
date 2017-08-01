#pragma once


#include "cTools.h"

extern int nextSharedPtrUniqueId;

template <class T>
class cCustomDeallocator 
{
public:
	virtual void objectFreed(T* object) = 0;
};

template <class T>
class cSharedPtr 
{
	friend class cResources;
	class cSharedPtrCounter 
	{
		int referenceCount;
		int uniqueId;

		static int nextUniqueId;
	public:
		cSharedPtrCounter() 
		{
			referenceCount = 0;
			uniqueId = nextSharedPtrUniqueId++;
		}

		int getCount() const
		{
			return referenceCount;
		}

		int getUniqueId() const
		{
			return uniqueId;
		}

		void increment() 
		{
			referenceCount++;
		}

		void decrement() 
		{
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
					cResources::deleteObject(object);
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

	~cSharedPtr() 
	{
		reset();
	}

	cSharedPtr(const cSharedPtr<T>& other) 
	{
		this->object = nullptr;
		this->counter = nullptr;
		this->deallocator = nullptr;
		*this = other;
	}
	cSharedPtr(cSharedPtr<T>&& other) 
	{
		this->object = nullptr;
		this->counter = nullptr;
		this->deallocator = nullptr;
		*this = other;
	}

	cSharedPtr(T* object) 
	{
		this->object = nullptr;
		this->counter = nullptr;
		this->deallocator = nullptr;
		*this = object;
	}

	cSharedPtr<T>& operator=(const T*& object) 
	{
		if (object != nullptr) 
		{
			this->object = object;
			counter = new cSharedPtrCounter();
			counter->increment();
		}
		return *this;
	}

	cSharedPtr<T>& operator=(T* object) 
	{
		reset();
		if (object != nullptr) 
		{
			this->object = object;
			counter = new cSharedPtrCounter();
			counter->increment();
		}
		return *this;
	}

	cSharedPtr<T>& operator=(const cSharedPtr<T>& other) 
	{
		if (this != &other) 
		{
			reset();
			object = other.object;
			counter = other.counter;
			deallocator = other.deallocator;
			if (counter) 
			{
				counter->increment();
			}
		}
		return *this;
	}

	cSharedPtr<T>& operator=(cSharedPtr<T>&& other) 
	{
		reset();
		object = other.object;
		counter = other.counter;
		deallocator = other.deallocator;

		if (counter) 
		{
			counter->increment();
		}
		return *this;
	}

	T* operator->() const
	{
		return object;
	}

	T* getObject() const
	{
		return object;
	}

	int getUniqueId() const
	{
		return counter->getUniqueId();
	}

	void setCustomDeallocator(cCustomDeallocator<T> *deallocator) 
	{
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
