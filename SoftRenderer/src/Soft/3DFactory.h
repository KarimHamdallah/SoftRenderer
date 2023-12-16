#pragma once
#include <vector>

struct vertex
{
	float x, y, z, u, v;
};

struct IndexedTriangleList
{
	std::vector<vertex> vertices;
	std::vector<uint32_t> indices;
};

class Factory3D
{
public:
	static IndexedTriangleList GetIndexedTriangleCube(float size = 1.0f)
	{
		float side = size * 0.5f;
		IndexedTriangleList list;
		list.vertices.push_back({ -side, -side, -side });
		list.vertices.push_back({ side, -side, -side });
		list.vertices.push_back({ -side, side, -side });
		list.vertices.push_back({ side, side, -side });
		list.vertices.push_back({ -side, -side, side });
		list.vertices.push_back({ side, -side, side });
		list.vertices.push_back({ -side, side, side });
		list.vertices.push_back({ side, side, side });

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
