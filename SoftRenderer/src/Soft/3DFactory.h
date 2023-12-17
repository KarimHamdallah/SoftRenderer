#pragma once
#include "Effects/TexturedVertexEffect.h"
#include "Effects/ColoredVertexEffect.h"
#include <vector>

template<class vertex>
struct IndexedTriangleList
{
	std::vector<vertex> vertices;
	std::vector<uint32_t> indices;
};

class Factory3D
{
public:
	static IndexedTriangleList<TexturedVertexEffect::vertex> GetIndexedTriangleCubeTextured(float size = 1.0f)
	{
		float side = size * 0.5f;
		IndexedTriangleList<TexturedVertexEffect::vertex> list;
		list.vertices.push_back({ -side, -side, -side, 0.0f, 1.0f });
		list.vertices.push_back({ side, -side, -side, 1.0f, 1.0f });
		list.vertices.push_back({ -side, side, -side, 0.0f, 0.0f });
		list.vertices.push_back({ side, side, -side, 1.0f, 0.0f });
		list.vertices.push_back({ -side, -side, side, 1.0f, 1.0f });
		list.vertices.push_back({ side, -side, side, 0.0f, 1.0f });
		list.vertices.push_back({ -side, side, side, 1.0f, 0.0f });
		list.vertices.push_back({ side, side, side, 0.0f, 0.0f });

		list.indices =
		{
			0,2,1,  2,3,1,
			1,3,5,  3,7,5,
			2,6,3,  3,6,7,
			4,5,7,  4,7,6,
			0,4,2,  2,4,6,
			0,1,4,  1,5,4
		};

		return list;
	}

	static IndexedTriangleList<ColoredVertexEffect::vertex> GetIndexedTriangleCubeColored(float size = 1.0f)
	{
		float side = size * 0.5f;
		IndexedTriangleList<ColoredVertexEffect::vertex> list;
		list.vertices.push_back({ -side, -side, -side, 0.5f, 1.0f, 0.3f });
		list.vertices.push_back({ side, -side, -side,  1.0f, 0.3f, 0.1f });
		list.vertices.push_back({ -side, side, -side,  0.0f, 0.5f, 0.7f });
		list.vertices.push_back({ side, side, -side,   1.0f, 0.5f, 0.5f });
		list.vertices.push_back({ -side, -side, side,  1.0f, 0.0f, 0.3f });
		list.vertices.push_back({ side, -side, side,   0.0f, 1.0f, 1.0f });
		list.vertices.push_back({ -side, side, side,   0.3f, 0.0f, 0.8f });
		list.vertices.push_back({ side, side, side,    0.0f, 1.0f, 0.3f });

		list.indices =
		{
			0,2,1,  2,3,1,
			1,3,5,  3,7,5,
			2,6,3,  3,6,7,
			4,5,7,  4,7,6,
			0,4,2,  2,4,6,
			0,1,4,  1,5,4
		};

		return list;
	}
};
