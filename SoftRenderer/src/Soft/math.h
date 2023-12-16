#pragma once
#include <cmath>

#define ToRad 0.01745329251

struct vec3
{
	vec3() = default;
	vec3(float x, float y, float z)
		: x(x), y(y), z(z) {}

	vec3 operator+(const vec3& v) const
	{
		return { x + v.x, y + v.y, z + v.z };
	}
	
	void operator+=(const vec3& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
	}

	vec3 operator-(const vec3& v) const
	{
		return { x - v.x, y - v.y, z - v.z };
	}

	vec3 operator-() const
	{
		return { -x, -y, -z };
	}


	float dot(const vec3& other) const
	{
		return x * other.x + y * other.y + z * other.z;
	}

	vec3 cross(const vec3& other) const
	{
		return vec3
			(
				y * other.z - z * other.y,
				z * other.x - x * other.z,
				x * other.y - y * other.x
			);
	}
	
	float x, y, z;
};

struct mat3
{
	static mat3 GetIdentity() 
	{
		return
		{
			1.0f,   0.0f,   0.0f,
			0.0f,   1.0f,   0.0f,
			0.0f,   0.0f,   1.0f
		};
	}
	static mat3 GetRotationX(float theta) // in radians
	{
		float sin = std::sinf(theta);
		float cos = std::cosf(theta);

		return
		{
			1.0f,   0.0f,   0.0f,
			0.0f,   cos,    sin,
			0.0f,   -sin,   cos
		};
	}
	static mat3 GetRotationY(float theta) // in radians
	{
		float sin = std::sinf(theta);
		float cos = std::cosf(theta);

		return
		{
			cos,   0.0f,    -sin,
			0.0f,   1.0f,   0.0f,
			sin,   0.0f,    cos
		};
	}
	static mat3 GetRotationZ(float theta) // in radians
	{
		float sin = std::sinf(theta);
		float cos = std::cosf(theta);

		return
		{
			cos,    sin,   0.0f,
			-sin,   cos,   0.0f,
			0.0f,   0.0f,   1.0f
		};
	}
	// Multiply mat3 by vec3
	vec3 operator*(const vec3& v) const
	{
		return {
			m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z,
			m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z,
			m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z
		};
	}
	// Multiply two mat3 matrices
	mat3 operator*(const mat3& other) const
	{
		mat3 result;

		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				result.m[i][j] = m[i][0] * other.m[0][j] +
					m[i][1] * other.m[1][j] +
					m[i][2] * other.m[2][j];
			}
		}

		return result;
	}

	float m[3][3];
};
