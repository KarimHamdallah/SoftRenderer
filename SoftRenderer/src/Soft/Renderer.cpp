#include "Renderer.h"

namespace Soft
{
	void Renderer::Init(uint32_t width, uint32_t height)
	{
		m_Width = width;
		m_Height = height;
		m_Framebuffer = new FrameBuffer(width, height);
		m_Pixels = new uint32_t[width * height];
		m_ZBuffer = std::make_shared<ZBuffer>(width, height);
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

	void Renderer::PutPixel(int x, int y, uint32_t color)
	{
		m_Pixels[y * m_Width + x] = color;
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
