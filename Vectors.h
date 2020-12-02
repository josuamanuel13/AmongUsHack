#pragma once

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

class Players {
public:
	Vector3 Positions[10];
	int Colors[10];
	bool Impostor[10];
	bool IsDeads[10];
	void SetPosition();
	void setColors();
	void setImpostors();
	void setIsDead();
};









