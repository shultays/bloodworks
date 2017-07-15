#pragma once

#include "cTools.h"

template <class T>
class Array2d 
{
public:
	T* data;
	int w, h;

	Array2d()
	{
		data = nullptr;
	}

	~Array2d()
	{
		SAFE_DELETE_ARRAY(data);
	}

	void init(int w, int h, T data) 
	{
		init(w, h);
		setAll(data);
	}

	void init(int w, int h) 
	{
		SAFE_DELETE_ARRAY(data);
		this->w = w;
		this->h = h;
		data = new T[w*h];
	}

	void init(const IntVec2& dims)
	{
		init(dims.w, dims.h);
	}

	void init(int s) 
	{
		init(s, s);
	}

	void setAll(T a) 
	{
		T* p = data + w*h;
		while (p-- > data) *p = a;
	}

	bool isValid(int x, int y) 
	{
		return data && x >= 0 && y >= 0 && x < w && y < h;
	}

	bool isValid(const IntVec2& side) 
	{
		return isValid(side.x, side.y);
	}

	void setZero() 
	{
		memset(data, 0, sizeof(T)*w*h);
	}

	inline T& operator[](const IntVec2& v) const 
	{
		return *(data + v.x*h + v.y);
	}

	inline T* operator[](int i) const 
	{
		return data + i*h;
	}

	inline T& atRawIndex(int i) const {
		return *(data + i);
	}
};
