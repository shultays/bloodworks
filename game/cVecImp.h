#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <cmath>

#include "cTools.h"

#if G_VEC_TYPE_ID == G_TYPE_INT
#define G_VEC_TYPE int
#elif G_VEC_TYPE_ID == G_TYPE_FLOAT
#define G_VEC_TYPE float
#define G_IS_TYPE_FLOATING
#elif G_VEC_TYPE_ID == G_TYPE_LONG
#define G_VEC_TYPE long long
#elif G_VEC_TYPE_ID == G_TYPE_DOUBLE
#define G_VEC_TYPE double
#define G_IS_TYPE_FLOATING
#endif


#ifdef G_IS_TYPE_FLOATING
#define G_SQRT_RET_TYPE G_VEC_TYPE
#else
#define G_SQRT_RET_TYPE G_VEC_FLOATING_TYPE
#endif


struct G_VEC_IMP_NAME 
{
public:
	union 
	{
		struct 
		{
			G_VEC_TYPE data[GVEC_N];
		};
#if GVEC_N == 2
		struct 
		{
			G_VEC_TYPE w, h;
		};
#endif

#ifdef G_VEC_PREV_1_CLASS
		struct 
		{
			G_VEC_PREV_1_CLASS G_VEC_PREV_1_NAME;
		};
#endif
#ifdef G_VEC_PREV_2_CLASS
		struct 
		{
			G_VEC_PREV_2_CLASS G_VEC_PREV_2_NAME;
		};
#endif

		struct 
		{
			G_VEC_TYPE x;
			G_VEC_TYPE y;
#if GVEC_N >= 3
			G_VEC_TYPE z;

#if GVEC_N >= 4
			G_VEC_TYPE o;
#endif
#endif
		};
		struct 
		{
			G_VEC_TYPE r;
			G_VEC_TYPE g;
#if GVEC_N >= 3
			G_VEC_TYPE b;

#if GVEC_N >= 4
			G_VEC_TYPE a;
#endif
#endif
		};
	};

	G_VEC_IMP_NAME() {
		/*for (int i = 0; i < GVEC_N; ++i)
		data[i] = 0;*/
	}

	static const G_VEC_IMP_NAME& zero()
	{
		static G_VEC_IMP_NAME v(0);
		return v;
	}

	void setZero() 
	{
		for (int i = 0; i < GVEC_N; ++i)
		{
			data[i] = 0;
		}
	}

	G_VEC_IMP_NAME(const G_VEC_TYPE& con) 
	{
		for (int i = 0; i < GVEC_N; ++i)
		{
			data[i] = con;
		}
	}

	bool isZero() const 
	{
		bool ret = true;
		for (int i = 0; ret && i < GVEC_N; ++i)
		{
			ret &= (data[i] == 0.0f);
		}
		return ret;
	}

	inline bool isNonZero() const 
	{
		return !isZero();
	}

	G_VEC_IMP_NAME operator-() const 
	{
		G_VEC_IMP_NAME cpy = *this;
		for (int i = 0; i < GVEC_N; ++i)
		{
			cpy.data[i] = -cpy.data[i];
		}
		return cpy;
	}

	inline G_VEC_IMP_NAME operator+() const 
	{
		return *this;
	}

	G_VEC_IMP_NAME operator+(const G_VEC_IMP_NAME& other) const 
	{
		G_VEC_IMP_NAME cpy = *this;
		for (int i = 0; i < GVEC_N; ++i)
		{
			cpy.data[i] += other[i];
		}
		return cpy;
	}

	G_VEC_IMP_NAME operator-(const G_VEC_IMP_NAME& other) const
	{
		G_VEC_IMP_NAME cpy = *this;
		for (int i = 0; i < GVEC_N; ++i)
		{
			cpy.data[i] -= other[i];
		}
		return cpy;
	}

	G_VEC_IMP_NAME operator*(const G_VEC_IMP_NAME& other) const 
	{
		G_VEC_IMP_NAME cpy = *this;
		for (int i = 0; i < GVEC_N; ++i)
		{
			cpy.data[i] *= other[i];
		}
		return cpy;
	}

	G_VEC_IMP_NAME operator/(const G_VEC_IMP_NAME& other) const 
	{
		G_VEC_IMP_NAME cpy = *this;
		for (int i = 0; i < GVEC_N; ++i)
		{
			cpy.data[i] /= other[i];
		}
		return cpy;
	}

	G_VEC_IMP_NAME operator+(const G_VEC_TYPE& con) const 
	{
		G_VEC_IMP_NAME cpy = *this;
		for (int i = 0; i < GVEC_N; ++i)
		{
			cpy.data[i] += con;
		}
		return cpy;
	}

	G_VEC_IMP_NAME operator-(const G_VEC_TYPE& con) const 
	{
		G_VEC_IMP_NAME cpy = *this;
		for (int i = 0; i < GVEC_N; ++i)
		{
			cpy.data[i] -= con;
		}
		return cpy;
	}

	G_VEC_IMP_NAME operator*(const G_VEC_TYPE& con) const 
	{
		G_VEC_IMP_NAME cpy = *this;
		for (int i = 0; i < GVEC_N; ++i)
		{
			cpy.data[i] *= con;
		}
		return cpy;
	}

	G_VEC_IMP_NAME operator/(const G_VEC_TYPE& con) const 
	{
		G_VEC_IMP_NAME cpy = *this;
		for (int i = 0; i < GVEC_N; ++i)
		{
			cpy.data[i] /= con;
		}
		return cpy;
	}

	G_VEC_IMP_NAME& operator=(const G_VEC_IMP_NAME& other) 
	{
		for (int i = 0; i < GVEC_N; ++i)
		{
			data[i] = other[i];
		}
		return *this;
	}

	G_VEC_IMP_NAME& operator=(const G_VEC_TYPE& con) 
	{
		for (int i = 0; i < GVEC_N; ++i)
		{
			data[i] = con;
		}
		return *this;
	}

	G_VEC_IMP_NAME& operator+=(const G_VEC_IMP_NAME& other) 
	{
		for (int i = 0; i < GVEC_N; ++i)
		{
			data[i] += other[i];
		}
		return *this;
	}

	G_VEC_IMP_NAME& operator-=(const G_VEC_IMP_NAME& other) 
	{
		for (int i = 0; i < GVEC_N; ++i)
		{
			data[i] -= other[i];
		}
		return *this;
	}

	G_VEC_IMP_NAME& operator/=(const G_VEC_IMP_NAME& other) 
	{
		for (int i = 0; i < GVEC_N; ++i)
		{
			data[i] /= other[i];
		}
		return *this;
	}

	G_VEC_IMP_NAME& operator*=(const G_VEC_IMP_NAME& other) 
	{
		for (int i = 0; i < GVEC_N; ++i)
		{
			data[i] *= other[i];
		}
		return *this;
	}

	G_VEC_IMP_NAME& operator+=(const G_VEC_TYPE& con) 
	{
		for (int i = 0; i < GVEC_N; ++i)
		{
			data[i] += con;
		}
		return *this;
	}

	G_VEC_IMP_NAME& operator-=(const G_VEC_TYPE& con) 
	{
		for (int i = 0; i < GVEC_N; ++i)
		{
			data[i] -= con;
		}
		return *this;
	}

	G_VEC_IMP_NAME& operator*=(const G_VEC_TYPE& con) 
	{
		for (int i = 0; i < GVEC_N; ++i)
		{
			data[i] *= con;
		}
		return *this;
	}

	G_VEC_IMP_NAME& operator/=(const G_VEC_TYPE& con) 
	{
		for (int i = 0; i < GVEC_N; ++i)
		{
			data[i] /= con;
		}
		return *this;
	}

	bool operator==(const G_VEC_IMP_NAME& other) const 
	{
		bool ret = true;
		for (int i = 0; i < GVEC_N && ret; ++i)
		{
			ret &= (other[i] == data[i]);
		}
		return ret;
	}

	bool operator!=(const G_VEC_IMP_NAME& other) const 
	{
		return !(other == *this);
	}

	inline G_VEC_TYPE& operator[](int index) 
	{
		return ((G_VEC_TYPE*)this)[index];
	}

	inline G_VEC_TYPE const& operator[](int index) const 
	{
		return ((G_VEC_TYPE*)this)[index];
	}

	G_SQRT_RET_TYPE length() const 
	{
		G_VEC_TYPE ret = 0;
		for (int i = 0; i < GVEC_N; ++i)
		{
			ret += data[i] * data[i];
		}
		return (G_SQRT_RET_TYPE)sqrt(ret);
	}

	G_VEC_TYPE lengthSquared() const 
	{
		G_VEC_TYPE ret = 0;
		for (int i = 0; i < GVEC_N; ++i)
		{
			ret += data[i] * data[i];
		}
		return ret;
	}

	static G_VEC_TYPE length(const G_VEC_IMP_NAME& vec) 
	{
		G_VEC_TYPE ret = 0;
		for (int i = 0; i < GVEC_N; ++i)
		{
			ret += vec.data[i] * vec.data[i];
		}

		return (G_VEC_TYPE)sqrt((double)ret);
	}

	static G_VEC_TYPE lengthSquared(const G_VEC_IMP_NAME& vec) 
	{
		G_VEC_TYPE ret = 0;
		for (int i = 0; i < GVEC_N; ++i)
		{
			ret += vec.data[i] * vec.data[i];
		}
		return ret;
	}

	G_VEC_TYPE manhattanDistance(const G_VEC_IMP_NAME& other) const
	{
		G_VEC_TYPE ret = 0;
		for (int i = 0; i < GVEC_N; ++i)
		{
			ret += abs(data[i] - other.data[i]);
		}
		return ret;
	}

	G_SQRT_RET_TYPE distance(const G_VEC_IMP_NAME& other) const 
	{
		G_VEC_TYPE ret = 0;
		for (int i = 0; i < GVEC_N; ++i) 
		{
			G_VEC_TYPE diff = data[i] - other.data[i];
			ret += diff*diff;
		}
		return (G_SQRT_RET_TYPE)sqrt((double)ret);
	}

	G_VEC_TYPE distanceSquared(const G_VEC_IMP_NAME& other) const 
	{
		G_VEC_TYPE ret = 0;
		for (int i = 0; i < GVEC_N; ++i) 
		{
			G_VEC_TYPE diff = data[i] - other.data[i];
			ret += diff*diff;
		}
		return ret;
	}

	static G_SQRT_RET_TYPE distance(const G_VEC_IMP_NAME& vec1, const G_VEC_IMP_NAME& vec2) 
	{
		G_VEC_TYPE ret = 0;
		for (int i = 0; i < GVEC_N; ++i) 
		{
			G_VEC_TYPE diff = vec1.data[i] - vec2.data[i];
			ret += diff*diff;
		}
		return (G_SQRT_RET_TYPE)sqrt((double)ret);
	}

	static G_VEC_TYPE distanceSquared(const G_VEC_IMP_NAME& vec1, const G_VEC_IMP_NAME& vec2) 
	{
		G_VEC_TYPE ret = 0;
		for (int i = 0; i < GVEC_N; ++i) 
		{
			G_VEC_TYPE diff = vec1.data[i] - vec2.data[i];
			ret += diff*diff;
		}
		return ret;
	}

	//

	G_VEC_TYPE dot(const G_VEC_IMP_NAME& other) const 
	{
		G_VEC_TYPE ret = 0;
		for (int i = 0; i < GVEC_N; ++i)
			ret += data[i] * other.data[i];
		return ret;
	}

	static G_VEC_TYPE dot(const G_VEC_IMP_NAME& vec1, const G_VEC_IMP_NAME& vec2) 
	{
		G_VEC_TYPE ret = 0;
		for (int i = 0; i < GVEC_N; ++i)
			ret += vec1.data[i] * vec2.data[i];
		return ret;
	}

#ifdef G_IS_TYPE_FLOATING
	static G_VEC_IMP_NAME safeNormalize(const G_VEC_IMP_NAME& vec)
	{
		if (vec.isZero())
		{
			return zero();
		}
		G_VEC_TYPE d = vec.length();
		G_VEC_IMP_NAME ret;
		for (int i = 0; i < GVEC_N; ++i)
		{
			ret.data[i] = vec.data[i] / d;
		}
		return ret;
	}

	G_VEC_TYPE safeNormalize()
	{
		if (isZero())
		{
			return 0;
		}
		G_VEC_TYPE d = length();
		for (int i = 0; i < GVEC_N; ++i)
		{
			data[i] /= d;
		}
		return d;
	}

	static G_VEC_IMP_NAME normalize(const G_VEC_IMP_NAME& vec)
	{
		G_VEC_TYPE d = vec.length();
		G_VEC_IMP_NAME ret;
		for (int i = 0; i < GVEC_N; ++i)
		{
			ret.data[i] = vec.data[i] / d;
		}
		return ret;
	}

	G_VEC_TYPE normalize()
	{
		G_VEC_TYPE d = length();
		for (int i = 0; i < GVEC_N; ++i)
		{
			data[i] /= d;
		}
		return d;
	}

	G_VEC_IMP_NAME normalized()  const
	{
		return G_VEC_IMP_NAME::normalize(*this);
	}

	G_VEC_IMP_NAME safeNormalized()  const
	{
		return G_VEC_IMP_NAME::safeNormalize(*this);
	}

	bool almostEquals(const G_VEC_IMP_NAME& other) const 
	{
		for (int i = 0; i < GVEC_N; ++i) 
		{
			if (abs(data[i] - other.data[i]) > 0.000001f) return false;
		}
		return true;
	}

#endif

#if GVEC_N == 2
	G_VEC_IMP_NAME(G_VEC_TYPE x, G_VEC_TYPE y) 
	{
		data[0] = x;
		data[1] = y;
	}

#ifdef G_IS_TYPE_FLOATING
	static G_VEC_IMP_NAME fromAngle(float angle)
	{
		return G_VEC_IMP_NAME(cos(angle), sin(angle));
	}

	float toAngle() const
	{
		return (float)std::atan2(y, x);
	}

	G_VEC_IMP_NAME sideVec() const
	{
		return G_VEC_IMP_NAME(-y, x);
	}

#endif
#endif

#if GVEC_N == 3

	static G_VEC_IMP_NAME fromColor(unsigned int color) 
	{
		G_VEC_IMP_NAME ret;

#ifdef G_IS_TYPE_FLOATING
		G_VEC_TYPE d = (G_VEC_TYPE)255;
		ret.r = ((color >> 16) & 255) / d;
		ret.g = ((color >> 8) & 255) / d;
		ret.b = ((color >> 0) & 255) / d;
#else
		ret.r = ((color >> 16) & 255);
		ret.g = ((color >> 8) & 255);
		ret.b = ((color >> 0) & 255);
#endif
		return ret;
	}

	G_VEC_IMP_NAME(G_VEC_PREV_1_CLASS xy, G_VEC_TYPE z) 
	{
		data[0] = xy.x;
		data[1] = xy.y;
		data[2] = z;
	}


	G_VEC_IMP_NAME(G_VEC_TYPE x, G_VEC_TYPE y, G_VEC_TYPE z) 
	{
		data[0] = x;
		data[1] = y;
		data[2] = z;
	}

	G_VEC_IMP_NAME cross(const G_VEC_IMP_NAME& other) const 
	{
		return G_VEC_IMP_NAME(
			y * other.z - other.y * z,
			z * other.x - other.z * x,
			x * other.y - other.x * y);
	}


	static G_VEC_IMP_NAME cross(const G_VEC_IMP_NAME& vec1, const G_VEC_IMP_NAME& vec2) 
	{
		return G_VEC_IMP_NAME(
			vec1.y * vec2.z - vec2.y * vec1.z,
			vec1.z * vec2.x - vec2.z * vec1.x,
			vec1.x * vec2.y - vec2.x * vec1.y);
	}
#endif

#ifndef G_IS_TYPE_FLOATING
	G_VEC_FLOATING_IMP_NAME toVec() const
	{
		G_VEC_FLOATING_IMP_NAME v;

		for (int i = 0; i < GVEC_N; ++i)
		{
			v.data[i] = (G_VEC_FLOATING_TYPE)data[i];
		}

		return v;
	}
#endif

#if GVEC_N == 4
	static G_VEC_IMP_NAME fromColor(unsigned int color) 
	{
		G_VEC_IMP_NAME ret;

#ifdef G_IS_TYPE_FLOATING
		G_VEC_TYPE d = (G_VEC_TYPE)255;
		ret.r = ((color >> 16) & 255) / d;
		ret.g = ((color >> 8) & 255) / d;
		ret.b = ((color >> 0) & 255) / d;
		ret.a = ((color >> 24) & 255) / d;
#else
		ret.r = ((color >> 16) & 255);
		ret.g = ((color >> 8) & 255);
		ret.b = ((color >> 0) & 255);
		ret.a = ((color >> 24) & 255);
#endif
		return ret;
	}
	G_VEC_IMP_NAME(G_VEC_PREV_1_CLASS xyz, G_VEC_TYPE o) 
	{
		data[0] = xyz.x;
		data[1] = xyz.y;
		data[2] = xyz.z;
		data[3] = o;
	}
	G_VEC_IMP_NAME(G_VEC_TYPE x, G_VEC_TYPE y, G_VEC_TYPE z) 
	{
		data[0] = x;
		data[1] = y;
		data[2] = z;
		data[3] = (G_VEC_TYPE)1;
	}
	G_VEC_IMP_NAME(G_VEC_TYPE x, G_VEC_TYPE y, G_VEC_TYPE z, G_VEC_TYPE o) 
	{
		data[0] = x;
		data[1] = y;
		data[2] = z;
		data[3] = o;
	}
#endif

#ifndef G_IS_TYPE_FLOATING

	G_VEC_IMP_NAME getSide(int side) const 
	{
		switch (side) {
		case 0: return left();
		case 1: return right();
		case 2: return down();
		case 3: return up();
#if GVEC_N >= 3
		case 4: return bottom();
		case 5: return top();

#if GVEC_N >= 4
		case 6: return oMinus();
		case 7: return oPlus();
#endif
#endif
		default:assert(false && "InvalidSide"); return zero();
		}
	}


	G_VEC_IMP_NAME left() const 
	{
		G_VEC_IMP_NAME v = *this;
		v.x--;
		return v;
	}
	G_VEC_IMP_NAME right() const 
	{
		G_VEC_IMP_NAME v = *this;
		v.x++;
		return v;
	}
	G_VEC_IMP_NAME up() const 
	{
		G_VEC_IMP_NAME v = *this;
		v.y++;
		return v;
	}

	G_VEC_IMP_NAME down() const 
	{
		G_VEC_IMP_NAME v = *this;
		v.y--;
		return v;
	}

#if GVEC_N >= 3
	G_VEC_IMP_NAME top() const 
	{
		G_VEC_IMP_NAME v = *this;
		v.z++;
		return v;
	}

	G_VEC_IMP_NAME bottom() const 
	{
		G_VEC_IMP_NAME v = *this;
		v.z--;
		return v;
	}

#if GVEC_N >= 4
	G_VEC_IMP_NAME oMinus() const 
	{
		G_VEC_IMP_NAME v = *this;
		v.o++;
		return v;
	}

	G_VEC_IMP_NAME oPlus() const 
	{
		G_VEC_IMP_NAME v = *this;
		v.o--;
		return v;
	}
#endif

#endif

	static int sideCount() 
	{
		return GVEC_N * 2;
	}

#endif
};

#undef G_VEC_TYPE
#ifdef G_IS_TYPE_FLOATING
#undef G_IS_TYPE_FLOATING
#endif
#undef G_SQRT_RET_TYPE