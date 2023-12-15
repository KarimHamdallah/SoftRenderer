#pragma once
#include <vector>
#include <filesystem>

struct Vertex
{
	Vertex() = default;
	Vertex(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	Vertex(float x, float y, float z, float w, float u, float v) : x(x), y(y), z(z), w(w), u(u), v(v) {}
	
	float x, y, z, w = 0.0f; // pos
	float u, v = 0.0f; // texcoord
};

class Mesh
{
public:
	Mesh() = default;
	Mesh(const std::vector<Vertex>& Vertices)
		: m_Vertices(Vertices) {}
	Mesh(const std::filesystem::path& path);

private:
	std::vector<Vertex> m_Vertices;
};
