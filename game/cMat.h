#pragma once

#include "cVec.h"

class Mat4;
class Mat3;
class Mat2;

Vec4 operator*(const Vec4 &v, const Mat4 &m);
Vec4 &operator*=(Vec4 &v, const Mat4 &m);

Vec3 operator*(const Vec3 &v, const Mat3 &m);
Vec3 &operator*=(Vec3 &v, const Mat3 &m);

Vec2 operator*(const Vec2 &v, const Mat2 &m);
Vec2 &operator*=(Vec2 &v, const Mat2 &m);

class Mat4 
{
public:
	union 
	{
		struct
		{
			float _00, _01, _02, _03;
			float _10, _11, _12, _13;
			float _20, _21, _22, _23;
			float _30, _31, _32, _33;
		};

		struct 
		{
			float arr[4][4];
		};
		struct 
		{
			float data[4 * 4];
		};
		struct 
		{
			Vec4 row0;
			Vec4 row1;
			Vec4 row2;
			Vec4 row3;
		};

	};

	Mat4() {}

	Mat4(float con) 
	{
		for (int i = 0; i < 4 * 4; i++)
		{
			data[i] = con;
		}
	}

	Mat4(const Vec4& v0, const Vec4& v1, const Vec4& v2, const Vec4& v3)
	{
		row0 = v0;
		row1 = v1;
		row2 = v2;
		row3 = v3;
	}

	static const Mat4& identity();

	void makeIdentity() 
	{
		_00 = 1.0f;
		_01 = 0.0f;
		_02 = 0.0f;
		_03 = 0.0f;
		_10 = 0.0f;
		_11 = 1.0f;
		_12 = 0.0f;
		_13 = 0.0f;
		_20 = 0.0f;
		_21 = 0.0f;
		_22 = 1.0f;
		_23 = 0.0f;
		_30 = 0.0f;
		_31 = 0.0f;
		_32 = 0.0f;
		_33 = 1.0f;
	}

	bool isIdentity() const
	{
		return
			_00 == 1.0f &&
			_01 == 0.0f &&
			_02 == 0.0f &&
			_03 == 0.0f &&
			_10 == 0.0f &&
			_11 == 1.0f &&
			_12 == 0.0f &&
			_13 == 0.0f &&
			_20 == 0.0f &&
			_21 == 0.0f &&
			_22 == 1.0f &&
			_23 == 0.0f &&
			_30 == 0.0f &&
			_31 == 0.0f &&
			_32 == 0.0f &&
			_33 == 1.0f;
	}

	static Mat4 rotation(float angle, const Vec3& axis) 
	{
		float s = sin(angle);
		float c = cos(angle);
		float s_1 = 1.0f - s;
		float c_1 = 1.0f - s;

		Mat4 mat;

		mat._00 = axis[0] * axis[0] * c_1 + c;
		mat._01 = axis[1] * axis[0] * c_1 - axis[2] * s;
		mat._02 = axis[2] * axis[0] * c_1 + axis[1] * s;
		mat._03 = 0.0f;
		mat._10 = axis[0] * axis[1] * c_1 + axis[2] * s;
		mat._11 = axis[1] * axis[1] * c_1 + c;
		mat._12 = axis[2] * axis[1] * c_1 - axis[0] * s;
		mat._13 = 0.0f;
		mat._20 = axis[0] * axis[2] * c_1 - axis[1] * s;
		mat._21 = axis[1] * axis[2] * c_1 + axis[0] * s;
		mat._22 = axis[2] * axis[2] * c_1 + c;
		mat._23 = 0.0f;
		mat._30 = mat._31 = mat._32 = 0.0f;
		mat._33 = 1.0f;

		return mat;
	}


	static Mat4 rotationX(float angle) 
	{
		float s = sin(angle);
		float c = cos(angle);
		float s_1 = 1.0f - s;
		float c_1 = 1.0f - s;

		Mat4 mat;

		mat._00 = c_1 + c;
		mat._01 = 0.0f;
		mat._02 = 0.0f;
		mat._03 = 0.0f;
		mat._10 = 0.0f;
		mat._11 = c;
		mat._12 = -s;
		mat._13 = 0.0f;
		mat._20 = 0.0f;
		mat._21 = s;
		mat._22 = c;
		mat._23 = 0.0f;
		mat._30 = 0.0f;
		mat._31 = 0.0f;
		mat._32 = 0.0f;
		mat._33 = 1.0f;

		return mat;
	}

	static Mat4 rotationY(float angle) 
	{
		float s = sin(angle);
		float c = cos(angle);
		float s_1 = 1.0f - s;
		float c_1 = 1.0f - s;

		Mat4 mat;

		mat._00 = c;
		mat._01 = 0.0f;
		mat._02 = s;
		mat._03 = 0.0f;
		mat._10 = 0.0f;
		mat._11 = c_1 + c;
		mat._12 = 0.0f;
		mat._13 = 0.0f;
		mat._20 = -s;
		mat._21 = 0.0f;
		mat._22 = c;
		mat._23 = 0.0f;
		mat._23 = 0.0f;
		mat._30 = 0.0f;
		mat._31 = 0.0f;
		mat._32 = 0.0f;
		mat._33 = 1.0f;

		return mat;
	}

	static Mat4 rotationZ(float angle) 
	{
		float s = sin(angle);
		float c = cos(angle);
		float s_1 = 1.0f - s;
		float c_1 = 1.0f - s;

		Mat4 mat;

		mat._00 = c;
		mat._01 = -s;
		mat._02 = 0.0f;
		mat._03 = 0.0f;
		mat._10 = s;
		mat._11 = c;
		mat._12 = 0.0f;
		mat._13 = 0.0f;
		mat._20 = 0.0f;
		mat._21 = 0.0f;
		mat._22 = c_1 + c;
		mat._23 = 0.0f;
		mat._30 = 0.0f;
		mat._31 = 0.0f;
		mat._32 = 0.0f;
		mat._33 = 1.0f;


		return mat;
	}

	static Mat4 translate(const Vec3& vec) 
	{
		Mat4 mat;

		mat._00 = 1.0f;
		mat._01 = 0.0f;
		mat._02 = 0.0f;
		mat._03 = 0.0f;
		mat._10 = 0.0f;
		mat._11 = 1.0f;
		mat._12 = 0.0f;
		mat._13 = 0.0f;
		mat._20 = 0.0f;
		mat._21 = 0.0f;
		mat._22 = 1.0f;
		mat._23 = 0.0f;
		mat._30 = vec.x;
		mat._31 = vec.y;
		mat._32 = vec.z;
		mat._33 = 1.0f;

		return mat;
	}

	Mat4& translateBy(const Vec3& vec) //optimize
	{
		return *this *= Mat4::translate(vec);
	}

	Mat4& scaleBy(const Vec3& vec) //optimize
	{
		return *this *= Mat4::scale(vec);
	}

	Mat4& scaleBy(float con) //optimize
	{
		return *this *= Mat4::scale(con);
	}

	Mat4& rotateBy(float angle, const Vec3& axis)  //optimize
	{
		return *this *= Mat4::rotation(angle, axis);
	}

	Mat4& rotateByX(float angle) //optimize
	{
		*this *= Mat4::rotationX(angle);
		return *this;
	}

	Mat4& rotateByY(float angle)  //optimize
	{
		*this *= Mat4::rotationY(angle);
		return *this;
	}

	Mat4& rotateByZ(float angle) //optimize
	{
		*this *= Mat4::rotationZ(angle);
		return *this;
	}

	static Mat4 scale(const Vec3& vec) 
	{
		Mat4 mat;

		mat._00 = vec.x;
		mat._01 = 0.0f;
		mat._02 = 0.0f;
		mat._03 = 0.0f;
		mat._10 = 0.0f;
		mat._11 = vec.y;
		mat._12 = 0.0f;
		mat._13 = 0.0f;
		mat._20 = 0.0f;
		mat._21 = 0.0f;
		mat._22 = vec.z;
		mat._23 = 0.0f;
		mat._30 = 0.0f;
		mat._31 = 0.0f;
		mat._32 = 0.0f;
		mat._33 = 1.0f;

		return mat;
	}

	static Mat4 scale(const float scale) 
	{
		Mat4 mat;

		mat._00 = scale;
		mat._01 = 0.0f;
		mat._02 = 0.0f;
		mat._03 = 0.0f;
		mat._10 = 0.0f;
		mat._11 = scale;
		mat._12 = 0.0f;
		mat._13 = 0.0f;
		mat._20 = 0.0f;
		mat._21 = 0.0f;
		mat._22 = scale;
		mat._23 = 0.0f;
		mat._30 = 0.0f;
		mat._31 = 0.0f;
		mat._32 = 0.0f;
		mat._33 = 1.0f;

		return mat;
	}


	Mat4 transpose() const
	{
		Mat4 mat;
		for (int i = 0; i < 4; i++) 
		{
			for (int j = 0; j < 4; j++) 
			{
				mat.arr[i][j] = arr[j][i];
			}
		}
		return mat;
	}


	bool operator==(const Mat4& other) const
	{
		bool ret = true;
		for (int i = 0; i < 4 * 4 && ret; ++i)
		{
			ret &= (other.data[i] == data[i]);
		}
		return ret;
	}


	Mat4 Mat4::operator*(const Mat4 &other) const 
	{
		Mat4 result;

		result._00 = _00 * other._00 + _01 * other._10 + _02 * other._20 + _03 * other._30;
		result._01 = _00 * other._01 + _01 * other._11 + _02 * other._21 + _03 * other._31;
		result._02 = _00 * other._02 + _01 * other._12 + _02 * other._22 + _03 * other._32;
		result._03 = _00 * other._03 + _01 * other._13 + _02 * other._23 + _03 * other._33;

		result._10 = _10 * other._00 + _11 * other._10 + _12 * other._20 + _13 * other._30;
		result._11 = _10 * other._01 + _11 * other._11 + _12 * other._21 + _13 * other._31;
		result._12 = _10 * other._02 + _11 * other._12 + _12 * other._22 + _13 * other._32;
		result._13 = _10 * other._03 + _11 * other._13 + _12 * other._23 + _13 * other._33;

		result._20 = _20 * other._00 + _21 * other._10 + _22 * other._20 + _23 * other._30;
		result._21 = _20 * other._01 + _21 * other._11 + _22 * other._21 + _23 * other._31;
		result._22 = _20 * other._02 + _21 * other._12 + _22 * other._22 + _23 * other._32;
		result._23 = _20 * other._03 + _21 * other._13 + _22 * other._23 + _23 * other._33;

		result._30 = _30 * other._00 + _31 * other._10 + _32 * other._20 + _33 * other._30;
		result._31 = _30 * other._01 + _31 * other._11 + _32 * other._21 + _33 * other._31;
		result._32 = _30 * other._02 + _31 * other._12 + _32 * other._22 + _33 * other._32;
		result._33 = _30 * other._03 + _31 * other._13 + _32 * other._23 + _33 * other._33;

		return result;
	}

	Mat4& Mat4::operator *= (const Mat4& other) 
	{
		return *this = *this * other;
	}

	Mat4& Mat4::operator =(const Mat4& other)
	{
		_00 = other._00;
		_01 = other._01;
		_02 = other._02;
		_03 = other._03;
		_10 = other._10;
		_11 = other._11;
		_12 = other._12;
		_13 = other._13;
		_20 = other._20;
		_21 = other._21;
		_22 = other._22;
		_23 = other._23;
		_30 = other._30;
		_31 = other._31;
		_32 = other._32;
		_33 = other._33;
	}

	Vec4 operator*(const Vec4 &v) const
	{
		Vec4 result;

		result[0] = v[0] * _00 + v[1] * _01 + v[2] * _02 + v[3] * _03;
		result[1] = v[0] * _10 + v[1] * _11 + v[2] * _12 + v[3] * _13;
		result[2] = v[0] * _20 + v[1] * _21 + v[2] * _22 + v[3] * _23;
		result[3] = v[0] * _30 + v[1] * _31 + v[2] * _32 + v[3] * _33;

		return result;
	}

	static Mat4 perspective(float zNear, float zFar, float fovInRadian, float width, float height) 
	{
		float aspect = width / height;
		float range = tanf(fovInRadian / 2.0f) * zNear;
		float sx = (2.0f * zNear) / (range * aspect + range * aspect);
		float sy = zNear / range;
		float sz = -(zFar + zNear) / (zFar - zNear);
		float pz = -(2.0f * zFar * zNear) / (zFar - zNear);

		Mat4 mat(0.0f);

		mat.data[0] = sx;
		mat.data[5] = sy;
		mat.data[10] = sz;
		mat.data[14] = pz;
		mat.data[11] = -1.0f;
		return mat;
	}

	Mat4 inverse() const 
	{
		Mat4 ret;
		float det;

		ret.data[0] = data[5] * data[10] * data[15] -
			data[5] * data[11] * data[14] -
			data[9] * data[6] * data[15] +
			data[9] * data[7] * data[14] +
			data[13] * data[6] * data[11] -
			data[13] * data[7] * data[10];

		ret.data[4] = -data[4] * data[10] * data[15] +
			data[4] * data[11] * data[14] +
			data[8] * data[6] * data[15] -
			data[8] * data[7] * data[14] -
			data[12] * data[6] * data[11] +
			data[12] * data[7] * data[10];

		ret.data[8] = data[4] * data[9] * data[15] -
			data[4] * data[11] * data[13] -
			data[8] * data[5] * data[15] +
			data[8] * data[7] * data[13] +
			data[12] * data[5] * data[11] -
			data[12] * data[7] * data[9];

		ret.data[12] = -data[4] * data[9] * data[14] +
			data[4] * data[10] * data[13] +
			data[8] * data[5] * data[14] -
			data[8] * data[6] * data[13] -
			data[12] * data[5] * data[10] +
			data[12] * data[6] * data[9];

		ret.data[1] = -data[1] * data[10] * data[15] +
			data[1] * data[11] * data[14] +
			data[9] * data[2] * data[15] -
			data[9] * data[3] * data[14] -
			data[13] * data[2] * data[11] +
			data[13] * data[3] * data[10];

		ret.data[5] = data[0] * data[10] * data[15] -
			data[0] * data[11] * data[14] -
			data[8] * data[2] * data[15] +
			data[8] * data[3] * data[14] +
			data[12] * data[2] * data[11] -
			data[12] * data[3] * data[10];

		ret.data[9] = -data[0] * data[9] * data[15] +
			data[0] * data[11] * data[13] +
			data[8] * data[1] * data[15] -
			data[8] * data[3] * data[13] -
			data[12] * data[1] * data[11] +
			data[12] * data[3] * data[9];

		ret.data[13] = data[0] * data[9] * data[14] -
			data[0] * data[10] * data[13] -
			data[8] * data[1] * data[14] +
			data[8] * data[2] * data[13] +
			data[12] * data[1] * data[10] -
			data[12] * data[2] * data[9];

		ret.data[2] = data[1] * data[6] * data[15] -
			data[1] * data[7] * data[14] -
			data[5] * data[2] * data[15] +
			data[5] * data[3] * data[14] +
			data[13] * data[2] * data[7] -
			data[13] * data[3] * data[6];

		ret.data[6] = -data[0] * data[6] * data[15] +
			data[0] * data[7] * data[14] +
			data[4] * data[2] * data[15] -
			data[4] * data[3] * data[14] -
			data[12] * data[2] * data[7] +
			data[12] * data[3] * data[6];

		ret.data[10] = data[0] * data[5] * data[15] -
			data[0] * data[7] * data[13] -
			data[4] * data[1] * data[15] +
			data[4] * data[3] * data[13] +
			data[12] * data[1] * data[7] -
			data[12] * data[3] * data[5];

		ret.data[14] = -data[0] * data[5] * data[14] +
			data[0] * data[6] * data[13] +
			data[4] * data[1] * data[14] -
			data[4] * data[2] * data[13] -
			data[12] * data[1] * data[6] +
			data[12] * data[2] * data[5];

		ret.data[3] = -data[1] * data[6] * data[11] +
			data[1] * data[7] * data[10] +
			data[5] * data[2] * data[11] -
			data[5] * data[3] * data[10] -
			data[9] * data[2] * data[7] +
			data[9] * data[3] * data[6];

		ret.data[7] = data[0] * data[6] * data[11] -
			data[0] * data[7] * data[10] -
			data[4] * data[2] * data[11] +
			data[4] * data[3] * data[10] +
			data[8] * data[2] * data[7] -
			data[8] * data[3] * data[6];

		ret.data[11] = -data[0] * data[5] * data[11] +
			data[0] * data[7] * data[9] +
			data[4] * data[1] * data[11] -
			data[4] * data[3] * data[9] -
			data[8] * data[1] * data[7] +
			data[8] * data[3] * data[5];

		ret.data[15] = data[0] * data[5] * data[10] -
			data[0] * data[6] * data[9] -
			data[4] * data[1] * data[10] +
			data[4] * data[2] * data[9] +
			data[8] * data[1] * data[6] -
			data[8] * data[2] * data[5];

		det = data[0] * ret.data[0] + data[1] * ret.data[4] + data[2] * ret.data[8] + data[3] * ret.data[12];

		if (det == 0)
			return identity();

		det = 1.0f / det;

		for (int i = 0; i < 16; i++)
			ret.data[i] = ret.data[i] * det;

		return ret;
	}

	static Mat4 lookAt(const Vec3& eye, const Vec3& focus, const Vec3& up) 
	{
		Vec3 Z = eye - focus;
		Z.normalize();

		Vec3 Y = up;
		Vec3 X = Y.cross(Z);
		Y = Z.cross(X);

		X.normalize();
		Y.normalize();

		Mat4 mat;


		mat._00 = X.x;
		mat._10 = X.y;
		mat._20 = X.z;
		mat._30 = -X.dot(eye);
		mat._01 = Y.x;
		mat._11 = Y.y;
		mat._21 = Y.z;
		mat._31 = -Y.dot(eye);
		mat._02 = Z.x;
		mat._12 = Z.y;
		mat._22 = Z.z;
		mat._32 = -Z.dot(eye);
		mat._03 = 0;
		mat._13 = 0;
		mat._23 = 0;
		mat._33 = 1.0f;


		return mat;
	}
};

class Mat3 
{
public:
	union 
	{
		struct
		{
			float _00, _01, _02;
			float _10, _11, _12;
			float _20, _21, _22;
		};

		struct 
		{
			float arr[3][3];
		};

		struct 
		{
			float data[3 * 3];
		};

		struct 
		{
			Vec3 row0;
			Vec3 row1;
			Vec3 row2;
		};

		struct 
		{
			Vec3 rows[3];
		};

		struct 
		{
			Vec3 front;
			Vec3 up;
			Vec3 side;
		};

	};

	Mat3() {}

	Mat3(float con) 
	{
		for (int i = 0; i < 3 * 3; i++)
		{
			data[i] = con;
		}
	}

	Mat3(Vec3 v0, Vec3 v1, Vec3 v2) 
	{
		row0 = v0;
		row1 = v1;
		row2 = v2;
	}

	static const Mat3& identity();

	void makeIdentity() 
	{
		_00 = 1.0f;
		_01 = 0.0f;
		_02 = 0.0f;
		_10 = 0.0f;
		_11 = 1.0f;
		_12 = 0.0f;
		_20 = 0.0f;
		_21 = 0.0f;
		_22 = 1.0f;
	}


	Mat3& scaleBy(float s) // optimize
	{
		*this *= Mat3::scaleMatrix(s);
		return *this;
	}

	Mat3& scaleBy(Vec2 v) // optimize
	{
		*this *= Mat3::scaleMatrix(v);
		return *this;
	}

	Mat3& scaleBy(float x, float y) // optimize
	{
		*this *= Mat3::scaleMatrix(x, y);
		return *this;
	}

	Mat3& translateBy(Vec2 v) // optimize
	{
		*this *= Mat3::translationMatrix(v);
		return *this;
	}

	Mat3& translateBy(float x, float y) // optimize
	{
		*this *= Mat3::translationMatrix(x, y);
		return *this;
	}

	Mat3& rotateBy(float angle) // optimize
	{
		*this *= Mat3::rotationMatrix(angle);
		return *this;
	}

	static Mat3 rotationMatrix(float angle)
	{
		Mat3 m;
		m._00 = cos(angle);
		m._01 = -sin(angle);
		m._02 = 0.0f;
		m._10 = -m._01;
		m._11 = m._00;
		m._12 = 0.0f;
		m._20 = 0.0f;
		m._21 = 0.0f;
		m._22 = 1.0f;

		return m;
	}

	static Mat3 translationMatrix(Vec2 pos)
	{
		Mat3 m;
		m._00 = 1.0f;
		m._01 = 0.0f;
		m._02 = 0.0f;
		m._10 = 0.0f;
		m._11 = 1.0f;
		m._12 = 0.0f;
		m._20 = pos.x;
		m._21 = pos.y;
		m._22 = 1.0f;

		return m;
	}

	static Mat3 translationMatrix(float x, float y)
	{
		Mat3 m;
		m._00 = 1.0f;
		m._01 = 0.0f;
		m._02 = 0.0f;
		m._10 = 0.0f;
		m._11 = 1.0f;
		m._12 = 0.0f;
		m._20 = x;
		m._21 = y;
		m._22 = 1.0f;

		return m;
	}


	static Mat3 scaleMatrix(Vec2 scale)
	{
		Mat3 m;
		m._00 = scale.x;
		m._01 = 0.0f;
		m._02 = 0.0f;
		m._10 = 0.0f;
		m._11 = scale.y;
		m._12 = 0.0f;
		m._20 = 0.0f;
		m._21 = 0.0f;
		m._22 = 1.0f;

		return m;
	}

	static Mat3 scaleMatrix(float xScale, float yScale)
	{
		Mat3 m;
		m._00 = xScale;
		m._01 = 0.0f;
		m._02 = 0.0f;
		m._10 = 0.0f;
		m._11 = yScale;
		m._12 = 0.0f;
		m._20 = 0.0f;
		m._21 = 0.0f;
		m._22 = 1.0f;

		return m;
	}

	static Mat3 scaleMatrix(float scale)
	{
		Mat3 m;
		m._00 = scale;
		m._01 = 0.0f;
		m._02 = 0.0f;
		m._10 = 0.0f;
		m._11 = scale;
		m._12 = 0.0f;
		m._20 = 0.0f;
		m._21 = 0.0f;
		m._22 = 1.0f;

		return m;
	}

	bool operator==(const Mat3& other) const
	{
		bool ret = true;
		for (int i = 0; i < 4 * 4 && ret; ++i)
			ret &= (other.data[i] == data[i]);
		return ret;
	}


	Mat3 Mat3::operator*(const Mat3 &other) const 
	{
		Mat3 result;

		result._00 = _00 * other._00 + _01 * other._10 + _02 * other._20;
		result._01 = _00 * other._01 + _01 * other._11 + _02 * other._21;
		result._02 = _00 * other._02 + _01 * other._12 + _02 * other._22;


		result._10 = _10 * other._00 + _11 * other._10 + _12 * other._20;
		result._11 = _10 * other._01 + _11 * other._11 + _12 * other._21;
		result._12 = _10 * other._02 + _11 * other._12 + _12 * other._22;

		result._20 = _20 * other._00 + _21 * other._10 + _22 * other._20;
		result._21 = _20 * other._01 + _21 * other._11 + _22 * other._21;
		result._22 = _20 * other._02 + _21 * other._12 + _22 * other._22;

		return result;
	}


	Mat3 &Mat3::operator *= (const Mat3 &other) 
	{
		return *this = *this * other;
	}

	Mat3& Mat3::operator =(const Mat3& other)
	{
		_00 = other._00;
		_01 = other._01;
		_02 = other._02;
		_10 = other._10;
		_11 = other._11;
		_12 = other._12;
		_20 = other._20;
		_21 = other._21;
		_22 = other._22;
		return *this;
	}


	Vec3 operator*(const Vec3 &v) const
	{
		Vec3 result;

		result[0] = v[0] * _00 + v[1] * _01 + v[2] * _02;
		result[1] = v[0] * _10 + v[1] * _11 + v[2] * _12;
		result[2] = v[0] * _20 + v[1] * _21 + v[2] * _22;

		return result;
	}

};



class Mat2 
{
public:
	union 
	{
		struct 
		{
			float arr[2][2];
		};
		struct 
		{
			float data[2 * 2];
		};
		struct 
		{
			Vec2 row0;
			Vec2 row1;
		};
		struct 
		{
			float _00, _01;
			float _10, _11;
		};

	};

	Mat2() {}

	Mat2(float con) 
	{
		for (int i = 0; i < 3 * 3; i++)
			data[i] = con;
	}

	Mat2(Vec2 v0, Vec2 v1) 
	{
		row0 = v0;
		row1 = v1;
	}

	static Mat2 identity() 
	{
		Mat2 mat;
		mat.makeIdentity();
		return mat;
	}

	void makeIdentity() 
	{
		_00 = 1.0f;
		_01 = 0.0f;
		_10 = 0.0f;
		_11 = 1.0f;
	}

	Mat2 inverse() const
	{
		Mat2 r;
		r._00 = _11;
		r._11 = _00;

		r._10 = -_10;
		r._01 = -_01;

		return r;
	}

	static Mat2 rotation(float angle) 
	{
		float s = sin(angle);
		float c = cos(angle);

		Mat2 mat;

		mat._00 = c;
		mat._01 = -s;
		mat._10 = s;
		mat._11 = c;

		return mat;
	}


	Mat2& scaleBy(const Vec2& vec) 
	{
		_00 *= vec[0];
		_01 *= vec[1];

		_10 *= vec[0];
		_11 *= vec[1];

		return *this;
	}

	Mat2& scaleBy(float con) 
	{
		_00 *= con;
		_01 *= con;

		_10 *= con;
		_11 *= con;

		return *this;
	}

	Mat2& rotateBy(float angle) 
	{
		return *this *= Mat2::rotation(angle);
	}

	static Mat2 scale(const Vec2& vec) 
	{
		Mat2 mat;

		mat._00 = vec.x;
		mat._01 = 0.0f;
		mat._10 = 0.0f;
		mat._11 = vec.y;

		return mat;
	}

	static Mat2 scale(const float scale) 
	{
		Mat2 mat;

		mat._00 = scale;
		mat._01 = 0.0f;
		mat._10 = 0.0f;
		mat._11 = scale;

		return mat;
	}


	Mat2 transpose() const
	{
		Mat2 mat;
		for (int i = 0; i < 2; i++) 
		{
			for (int j = 0; j < 2; j++) 
			{
				mat.arr[i][j] = arr[j][i];
			}
		}
		return mat;
	}


	bool operator==(const Mat2& other) const
	{
		bool ret = true;
		for (int i = 0; i < 4 * 4 && ret; ++i)
			ret &= (other.data[i] == data[i]);
		return ret;
	}


	Mat2 Mat2::operator*(const Mat2 &other) const 
	{
		Mat2 result;

		result._00 = _00 * other._00 + _01 * other._10;
		result._01 = _00 * other._01 + _01 * other._11;

		result._10 = _10 * other._00 + _11 * other._10;
		result._11 = _10 * other._01 + _11 * other._11;

		return result;
	}


	Mat2 &Mat2::operator *= (const Mat2 &other) 
	{
		return *this = *this * other;
	}

	Mat2 &Mat2::operator = (const Mat2 &other)
	{
		_00 = other._00;
		_01 = other._01;
		_10 = other._10;
		_11 = other._11;

		return *this;
	}


	Vec3 operator*(const Vec3 &v) const
	{
		Vec3 result;

		result[0] = v[0] * _00 + v[1] * _01;
		result[1] = v[0] * _10 + v[1] * _11;

		return result;
	}
};