#pragma once
#include "../GpuLayer/GpuLayer.h"
#include "Texture.h"
#include "mesh.h"

#define RGBA_COLOR(r, g, b, a) ((r) | (g << 8) | (b << 16) | (a << 24))

namespace Soft
{
	class Renderer
	{
	public:
		static void Init(uint32_t width, uint32_t height);
		static void Destroy();

		static void Resize(uint32_t width, uint32_t height);

		static void ClearScreen(uint32_t color);
		static void Present();

		static void DrawCircle(int cx, int cy, int r, uint32_t color);

		static void DrawTriangle(Vertex A, Vertex B, Vertex C, uint32_t color,
			Texture2D* Image);

		static void DrawLine(int x0, int y0, int x1, int y1, uint32_t color);

	private:
		inline static uint32_t m_Width, m_Height = 0;
		inline static FrameBuffer* m_Framebuffer = nullptr;
		inline static uint32_t* m_Pixels = nullptr;
	};
}
