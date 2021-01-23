#pragma once
#include <algorithm>
#define PI 3.1415927f

struct IntVec2
{
	int x, y;
};

struct Vector2
{
	float x, y;
};

 struct Vector3
{
	float x, y, z;
};

 struct Quaternion
{
	float x, y, z, w;
};

struct Matrix4x4
{
	float m00, m01, m02, m03;
	float m10, m11, m12, m13;
	float m20, m21, m22, m23;
	float m30, m31, m32, m33;
};

inline Vector3 Subtract(Vector3 src, Vector3 dst)
{
	Vector3 diff;
	diff.x = src.x - dst.x;
	diff.y = src.y - dst.y;
	diff.z = src.z - dst.z;
	return diff;
}

inline float Magnitude(Vector3 vec)
{
	return sqrtf(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
}

inline float Distance(Vector3 src, Vector3 dst)
{
	Vector3 diff = Subtract(src, dst);
	return Magnitude(diff);
}

inline float Distance2D(Vector3 first, Vector3 Second)
{
	return sqrtf(powf((Second.x - first.x), 2) + powf((Second.y - first.y), 2) + powf((Second.z - first.z), 2));
}











