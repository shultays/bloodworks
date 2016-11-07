#pragma once

#include <vector>

const int max_priority = 0x0FFFFFFF;
const int defaultPriority = 0x0FFFFFFF;

template <class T>
class cPriorityVector
{
	struct Element {
		int p;
		T item;
	};

	std::vector<struct Element> elements;

	void fixInsert(int i) 
	{
		while (true) 
		{
			if (i == 0) break;
			if (elements[i].p >= elements[i - 1].p) break;
			int j = i - 1;
			int p = elements[j].p;
			j--;
			while (j >= 0 && elements[j].p == p) j--;

			swapt(elements[j + 1], elements[i]);
			i = j + 1;
		}
	}

	void fixRemove(int i) 
	{
		while (true) 
		{
			if (i == elements.size() - 1) break;
			if (elements[i].p <= elements[i + 1].p) break;
			int j = i + 1;
			int p = elements[j].p;
			j++;
			while (j < elements.size() && elements[j].p == p) j++;

			swapt(elements[j - 1], elements[i]);
			i = j - 1;
		}
	}

	int fix(int i) 
	{
		if (i > 0 && elements[i].p < elements[i - 1].p) fixInsert(i);
		if (i < elements.size() - 1 && elements[i].p < elements[i + 1].p) fixRemove(i);
	}

public:
	cPriorityVector() 
	{
	}

	void clear() 
	{
		elements.clear();
	}

	void insert(T item, int priority) 
	{
		Element e;
		e.item = item;
		e.p = priority;
		elements.push_back(e);
		fixInsert(size() - 1);
	}

	void insert(T item) 
	{
		insert(item, defaultPriority);
	}

	void remove(T item) 
	{
		for (int i = 0; i < elements.size(); i++) 
		{
			if (elements[i].item == item) 
			{
				removeAtIndex(i);
				return;
			}
		}
	}

	void removeAtIndex(int i) 
	{
		elements[i].p = max_priority + 1;
		fixRemove(i);
		elements.resize(elements.size() - 1);
	}

	int getPriorityAt(int i) 
	{
		return elements[i].p;
	}

	void changePriority(T item, int newPriority) 
	{
		for (int i = 0; i < size; i++) 
		{
			if (elements[i].item == item)
			{
				changePriorityAtIndex(i, newPriority);
				return;
			}
		}
	}
	int changePriorityAtIndex(int i, int newPriority) 
	{
		elements[i].p = newPriority;
		fix(i);
	}

	inline T& operator[](int i) 
	{
		return elements[i].item;
	}

	int size()
	{
		return (int)elements.size();
	}
};
