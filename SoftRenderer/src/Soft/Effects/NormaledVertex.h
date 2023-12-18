#pragma once
#include <random>
#include "../Texture.h"

class NormaledVertexEffect
{
public:

	struct vertex
	{
		float x, y, z, u, v, nx, ny, nz = 0.0f;

		vertex operator+(const vertex& rhs) const
		{
			return { x + rhs.x, y + rhs.y, z + rhs.z, u + rhs.u, v + rhs.v, nx + rhs.nx, ny + rhs.ny, nz + rhs.nz };
		}

		vertex operator+(float rhs) const
		{
			return { x + rhs, y + rhs, z + rhs, u + rhs, v + rhs, nx + rhs, ny + rhs, nz + rhs };
		}

		void operator+=(const vertex& rhs)
		{
			x += rhs.x;
			y += rhs.y;
			z += rhs.z;
			u += rhs.u;
			v += rhs.v;
			nx += rhs.nx;
			ny += rhs.ny;
			nz += rhs.nz;
		}

		void operator+=(float rhs)
		{
			x += rhs;
			y += rhs;
			z += rhs;
			u += rhs;
			v += rhs;
			nx += rhs;
			ny += rhs;
			nz += rhs;
		}

		vertex operator-(const vertex& rhs) const
		{
			return { x - rhs.x, y - rhs.y, z - rhs.z, u - rhs.u, v - rhs.v, nx - rhs.nx, ny - rhs.ny, nz - rhs.nz };
		}

		vertex operator-(float rhs) const
		{
			return { x - rhs, y - rhs, z - rhs, u - rhs, v - rhs, nx - rhs, ny - rhs, nz - rhs };
		}

		void operator-=(const vertex& rhs)
		{
			x -= rhs.x;
			y -= rhs.y;
			z -= rhs.z;
			u -= rhs.u;
			v -= rhs.v;
			nx -= rhs.nx;
			ny -= rhs.ny;
			nz -= rhs.nz;
		}

		void operator-=(float rhs)
		{
			x -= rhs;
			y -= rhs;
			z -= rhs;
			u -= rhs;
			v -= rhs;
			nx -= rhs;
			ny -= rhs;
			nz -= rhs;
		}

		vertex operator*(float rhs) const
		{
			return { x * rhs, y * rhs, z * rhs, u *rhs, v * rhs, nx * rhs, ny * rhs, nz * rhs };
		}

		vertex operator*(const vertex& rhs) const
		{
			return { x * rhs.x, y * rhs.y, z * rhs.z, u * rhs.u, v * rhs.v, nx * rhs.nx, ny * rhs.ny, nz * rhs.nz };
		}

		vertex operator/(float rhs) const
		{
			return { x / rhs, y / rhs, z / rhs, u / rhs, v / rhs, nx / rhs, ny / rhs, nz / rhs };
		}

		vertex operator/(const vertex& rhs) const
		{
			return { x / rhs.x, y / rhs.y, z / rhs.z, u / rhs.u, v / rhs.v, nx / rhs.nx, ny / rhs.ny, nz / rhs.nz };
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

			return 0xFFFFFFFF;
		}

		std::shared_ptr<Texture2D> m_Texture = nullptr;
	};

	PixelShader ps;
};
