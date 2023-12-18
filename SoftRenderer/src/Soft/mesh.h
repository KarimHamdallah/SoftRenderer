#pragma once
#include <filesystem>
#include "Effects/NormaledVertex.h"

#define INDICES_PER_TRIANGLE 3

namespace Soft
{

	class Mesh
	{
	public:
		Mesh(const std::filesystem::path& path);

		~Mesh() = default;

		uint32_t GetIndicesCount() const { return m_Indices.size() * INDICES_PER_TRIANGLE; }

		const std::vector<NormaledVertexEffect::vertex>& GetVertices() { return m_Vertices; }
		const std::vector<uint32_t>& GetIndices() { return m_Indices; }

	private:
		std::vector<NormaledVertexEffect::vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;
	};
}
