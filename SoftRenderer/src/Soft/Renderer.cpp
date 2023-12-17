#include "Renderer.h"

namespace
{
	int edgeFunction(vec3 a, vec3 b, vec3 c)
	{
		return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
	};

	int edgeFunction(vertex a, vertex b, vertex c)
	{
		return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
	};
}

namespace Soft
{
	void Renderer::Init(uint32_t width, uint32_t height)
	{
		m_Width = width;
		m_Height = height;
		m_Framebuffer = new FrameBuffer(width, height);
		m_Pixels = new uint32_t[width * height];
	}

	void Renderer::Destroy()
	{
		delete[] m_Pixels;
		m_Pixels = nullptr;
		m_Width = 0;
		m_Height = 0;
	}

	void Renderer::Resize(uint32_t width, uint32_t height)
	{
		m_Framebuffer->Resize(width, height);
		delete[] m_Pixels;
		m_Pixels = new uint32_t[width * height];
		m_Width = width;
		m_Height = height;
	}

	void Renderer::ClearScreen(uint32_t color)
	{
		// clear screen
		for (size_t i = 0; i < m_Width * m_Height; i++)
			m_Pixels[i] = color;
	}

	void Renderer::Present()
	{
		m_Framebuffer->Present(m_Pixels);
	}

	void Renderer::DrawCircle(int cx, int cy, int r, uint32_t color)
	{
		float half_raduis = 0.5f * r;
		float topleft_x = cx - half_raduis;
		float topleft_y = cy - half_raduis;

		float bottomright_x = cx + half_raduis;
		float bottomright_y = cy + half_raduis;

		for (int py = topleft_y; py < bottomright_y; py++)
		{
			for (int px = topleft_x; px < bottomright_x; px++)
			{
				float x = px - cx;
				float y = py - cy;
				float dist = sqrtf(x * x + y * y);

				if (dist <= half_raduis)
					if (px > 0 && py > 0 && px < m_Width && py < m_Height)
						m_Pixels[py * m_Width + px] = color;
			}
		}
	}

	void Renderer::DrawTriangle(vertex A, vertex B, vertex C, uint32_t color, Texture2D* Image)
	{
		// Calculate the edge function for the whole triangle (ABC)
		int ABC = edgeFunction(A, B, C);

		if (ABC < 0.0f) // back face culling
			return;

		// Initialise our Vertex
		vertex P;

		// Get the bounding box of the triangle
		int minX = std::min(std::min(A.x, B.x), C.x);
		int minY = std::min(std::min(A.y, B.y), C.y);
		int maxX = std::max(std::max(A.x, B.x), C.x);
		int maxY = std::max(std::max(A.y, B.y), C.y);

		// Loop through all the pixels of the bounding box
		for (P.y = minY; P.y < maxY; P.y++)
		{
			for (P.x = minX; P.x < maxX; P.x++)
			{
				// Calculate our edge functions
				int ABP = edgeFunction(A, B, P);
				int BCP = edgeFunction(B, C, P);
				int CAP = edgeFunction(C, A, P);

				// Normalise the edge functions by dividing by the total area to get the barycentric coordinates
				float weightA = (float)BCP / ABC;
				float weightB = (float)CAP / ABC;
				float weightC = (float)ABP / ABC;

				// If all the edge functions are positive, the Vertex is inside the triangle
				if (weightA >= 0 && weightB >= 0 && weightC >= 0)
				{
					if (!Image)
					{
						// Draw the pixel
						if (P.x < m_Width && P.x > 0 && P.y < m_Height && P.y > 0)
						{
							m_Pixels[(int)P.y * m_Width + (int)P.x] = color;
						}
					}
					else
					{
						// Interpolate texture coordinates with perspective correction
						float u = weightA * A.u + weightB * B.u + weightC * C.u;
						float v = weightA * A.v + weightB * B.v + weightC * C.v;

						// Sample the texture using the interpolated texture coordinates
						int textureX = static_cast<int>(u * Image->Width) % Image->Width;
						int textureY = static_cast<int>(v * Image->Height) % Image->Height;

						// Draw the pixel
						if (P.x < m_Width && P.x > 0 && P.y < m_Height && P.y > 0)
						{
							uint32_t textureColor = Image->pixels[textureY * Image->Width + textureX];
							m_Pixels[(int)P.y * m_Width + (int)P.x] = textureColor;
						}
					}
				}
			}
		}
	}

	void Renderer::DrawLine(int x0, int y0, int x1, int y1, uint32_t color)
	{
		int dx = abs(x1 - x0);
		int dy = abs(y1 - y0);
		int sx = (x0 < x1) ? 1 : -1;
		int sy = (y0 < y1) ? 1 : -1;
		int err = dx - dy;

		float intensity;

		while (true) {
			// Calculate intensity based on the fractional part of coordinates
			/*
			intensity = 1.0f - (float)(dx + dy) / std::max(dx, dy);

			uint8_t r = (color >> 16) & 0xFF;
			uint8_t g = (color >> 8) & 0xFF;
			uint8_t b = color & 0xFF;

			// Adjust color based on intensity
			r = static_cast<uint8_t>(r * intensity);
			g = static_cast<uint8_t>(g * intensity);
			b = static_cast<uint8_t>(b * intensity);

			uint32_t interpolatedColor = (r << 16) | (g << 8) | b;
			*/
			// Draw anti-aliased pixel
			if(x0 > 0 && x0 < m_Width - 1 && y0 > 0 && y0 < m_Height - 1)
				m_Pixels[(int)y0 * m_Width + (int)x0] = color;

			if (x0 == x1 && y0 == y1) {
				break;
			}

			int e2 = 2 * err;
			if (e2 > -dy) {
				err -= dy;
				x0 += sx;
			}
			if (e2 < dx) {
				err += dx;
				y0 += sy;
			}
		}
	}
}
