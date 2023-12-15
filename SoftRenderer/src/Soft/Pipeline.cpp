#include "Pipeline.h"
#include <gtc/matrix_transform.hpp>
#include "Renderer.h"

namespace
{
	int edgeFunction(Vertex a, Vertex b, Vertex c)
	{
		return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
	};

	uint32_t Vec4ToUint32Color(const glm::vec4& Color)
	{
		int r = Color.r * 255;
		int g = Color.g * 255;
		int b = Color.b * 255;
		int a = Color.a * 255;

		return RGBA_COLOR(r, g, b, a);
	}
}

namespace Soft
{

	void Pipeline::SetViewPort(int x, int y, uint32_t width, uint32_t height)
	{
		m_ViewPort_x = x;
		m_ViewPort_y = y;
		m_ViewPortWidth = width;
		m_ViewPortHeight = height;
	}

	void Pipeline::DrawIndexed(const std::vector<Vertex>& Vertices, const std::vector<uint32_t> Indices)
	{
		const auto& transformed_vertices = ProcessVertices(Vertices);
		AssembleTriangles(transformed_vertices, Indices);
	}

	std::vector<Vertex> Pipeline::ProcessVertices(const std::vector<Vertex>& Vertices)
	{
		std::vector<Vertex> TransformedVertices;

		glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)m_ViewPortWidth / (float)m_ViewPortHeight,
			0.01f, 1000.0f);

		glm::mat4 model =
			glm::translate(glm::mat4(1.0f), m_Position)
			* glm::rotate(glm::mat4(1.0f), m_Rotation.x, glm::vec3(1.0f, 0.0f, 0.0f))
			* glm::rotate(glm::mat4(1.0f), m_Rotation.y, glm::vec3(0.0f, 1.0f, 0.0f))
			* glm::rotate(glm::mat4(1.0f), m_Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

		for (Vertex vertex : Vertices)
		{
			glm::vec4 vertex_transf = { vertex.x, vertex.y, vertex.z, vertex.w };

			vertex_transf = proj * model * vertex_transf;

			TransformedVertices.push_back
			(
				{
				   vertex_transf.x, vertex_transf.y, vertex_transf.z, vertex_transf.w,
				   vertex.u, vertex.v
				}
			);
		}

		return TransformedVertices;
	}

	void Pipeline::AssembleTriangles(const std::vector<Vertex>& TransformedVertices, const std::vector<uint32_t>& Indices)
	{
		for (size_t indx = 0; indx < Indices.size() / 3; indx++)
		{
			// get triangle indices

			uint32_t Index1 = Indices[3 * indx + 0];
			uint32_t Index2 = Indices[3 * indx + 1];
			uint32_t Index3 = Indices[3 * indx + 2];

			// get triangle transformed vertices

			Vertex v1 = TransformedVertices[Index1];
			Vertex v2 = TransformedVertices[Index2];
			Vertex v3 = TransformedVertices[Index3];

			if(m_BackFaceCulling)
			{
				// Calculate vectors representing two edges of the triangle
				// Calculate the normal vector using the cross product of the edges
				glm::vec3 edge1(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z);
				glm::vec3 edge2(v3.x - v1.x, v3.y - v1.y, v3.z - v1.z);
				glm::vec3 normal = glm::cross(edge1, edge2);

				// Assuming your camera is at the origin (0, 0, 0)
				glm::vec3 viewVector = -glm::vec3(v1.x, v1.y, v1.z);  // Vector from the camera to v1
				// Calculate the dot product of the normal and viewVector
				float dotProduct = glm::dot(normal, viewVector);

				// Perform back-face culling
				if (dotProduct < 0)
				{
					// The triangle is a back face, so you can cull it
					// Do not render or process this triangle
				}
				else
				{
					// The triangle is a front face, proceed with rendering or processing
					GenerateTriangle(v1, v2, v3);
				}
			}
			else
			{
				GenerateTriangle(v1, v2, v3);
			}
		}
	}

	void Pipeline::GenerateTriangle(Vertex& v1, Vertex& v2, Vertex& v3)
	{
		// geometry shader here
		PostProcessTriangleVertices(v1, v2, v3);
	}

	void Pipeline::PostProcessTriangleVertices(Vertex& v1, Vertex& v2, Vertex& v3)
	{
		FromNDCToScreenSpace(v1);
		FromNDCToScreenSpace(v2);
		FromNDCToScreenSpace(v3);

		PerspectiveDevide(v1);
		PerspectiveDevide(v2);
		PerspectiveDevide(v3);

		if(m_FillTriangles)
			Renderer::DrawTriangle(v1, v2, v3, Vec4ToUint32Color(m_Color), m_Texture.get());
		else
		{
			uint32_t color = Vec4ToUint32Color(m_Color);
			Renderer::DrawLine(v1.x, v1.y, v2.x, v2.y, color);
			Renderer::DrawLine(v2.x, v2.y, v3.x, v3.y, color);
			Renderer::DrawLine(v1.x, v1.y, v3.x, v3.y, color);
		}
	}

	void Pipeline::FromNDCToScreenSpace(Vertex& v)
	{
		v.x = (v.x * m_ViewPortWidth * 0.5f) + (v.w * m_ViewPortWidth * 0.5f);
		v.y = (v.y * m_ViewPortHeight * -0.5f) + (v.w * m_ViewPortHeight * 0.5f);

		v.x += m_ViewPort_x;
		v.y += m_ViewPort_y;
	}

	void Pipeline::PerspectiveDevide(Vertex& v)
	{
		v.x /= v.w;
		v.y /= v.w;
		v.z /= v.w;
	}
}
