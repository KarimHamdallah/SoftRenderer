#pragma once
#include <limits>
#include <cassert>
#include <iostream>

class ZBuffer
{
public:
	ZBuffer(uint32_t Width, uint32_t Height)
	{
		m_Width = Width;
		m_Height = Height;

		m_Buffer = new float[m_Width * m_Height];
	}

	~ZBuffer()
	{
		delete m_Buffer;
		m_Buffer = nullptr;
		m_Width = 0;
		m_Height = 0;
	}

	float& Get(int x, int y)
	{
		return m_Buffer[y * m_Width + x];
	}

	const float& Get(int x, int y) const
	{
		return const_cast<ZBuffer*>(this)->Get(x, y);
	}

	bool Test(int x, int y, float depth)
	{
		float& depthInBuffer = Get(x, y);
		if (depth < depthInBuffer)
		{
			depthInBuffer = depth;
			return true;
		}

		return false;
	}

	void Clear()
	{
		for (size_t i = 0; i < m_Width * m_Height; i++)
		{
			m_Buffer[i] = std::numeric_limits<float>::infinity();
		}
	}

	float* m_Buffer = nullptr;
	uint32_t m_Width, m_Height = 0;
};