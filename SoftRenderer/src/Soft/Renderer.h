#pragma once
#include "../GpuLayer/GpuLayer.h"
#include "math.h"
#include "3DFactory.h"
#include "Texture.h"
#include "ZBuffer.h"


#define RGBA_COLOR(r, g, b, a) ((r) | (g << 8) | (b << 16) | (a << 24))

namespace Soft
{
	class Renderer
	{
	public:
		static void Init(uint32_t width, uint32_t height);
		static void Destroy();

		static void BeginFrame() { m_ZBuffer->Clear(); }

		static bool TestAndSet(int x, int y, float value) { return m_ZBuffer->Test(x, y, value); }

		static void Resize(uint32_t width, uint32_t height);

		static void ClearScreen(uint32_t color);
		static void Present();

		static uint32_t GetWidth() { return m_Width; }
		static uint32_t GetHeight() { return m_Height; }

		static void DrawCircle(int cx, int cy, int r, uint32_t color);

		static void PutPixel(int x, int y, uint32_t color);

		static void DrawLine(int x0, int y0, int x1, int y1, uint32_t color);

	private:
		inline static uint32_t m_Width, m_Height = 0;
		inline static FrameBuffer* m_Framebuffer = nullptr;
		inline static uint32_t* m_Pixels = nullptr;
		inline static std::shared_ptr<ZBuffer> m_ZBuffer = nullptr;
	};
}
