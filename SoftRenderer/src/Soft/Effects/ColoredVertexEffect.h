#pragma once
#include <random>
#include "../Texture.h"

class ColoredVertexEffect
{
public:

	struct vertex
	{
		float x, y, z, r, g, b = 0.0f;

		vertex operator+(const vertex& rhs) const
		{
			return { x + rhs.x, y + rhs.y, z + rhs.z, r + rhs.r, g + rhs.g, b + rhs.b };
		}

		vertex operator*(float rhs) const
		{
			return { x * rhs, y * rhs, z * rhs, r * rhs, g * rhs, b * rhs };
		}
	};

	struct PixelShader
	{
		uint32_t GetColor(const vertex& input)
		{
			int r = input.r * 255;
			int g = input.g * 255;
			int b = input.b * 255;
			
			return((r) | (g << 8) | (b << 16) | (1 << 24));
		}
	};

	vertex v;
	PixelShader ps;
};
