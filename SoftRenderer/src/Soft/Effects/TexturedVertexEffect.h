#pragma once
#include <random>
#include "../Texture.h"

class TexturedVertexEffect
{
public:

	struct vertex
	{
		float x, y, z, u, v = 0.0f;

		vertex operator+(const vertex& rhs) const
		{
			return { x + rhs.x, y + rhs.y, z + rhs.z, u + rhs.u, v + rhs.v };
		}

		vertex operator*(float rhs) const
		{
			return { x * rhs, y * rhs, z * rhs, u * rhs, v * rhs };
		}
	};

	struct PixelShader
	{
		uint32_t GetColor(const vertex& input)
		{
			// Sample the texture using the interpolated texture coordinates
			int textureX = static_cast<int>(input.u * m_Texture->Width) % m_Texture->Width;
			int textureY = static_cast<int>(input.v * m_Texture->Height) % m_Texture->Height;

			uint32_t textureColor = m_Texture->pixels[textureY * m_Texture->Width + textureX];

			return textureColor;
		}

		std::shared_ptr<Texture2D> m_Texture = nullptr;
	};

	vertex v;
	PixelShader ps;
};
