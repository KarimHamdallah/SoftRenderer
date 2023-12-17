#pragma once
#include "Renderer.h"

namespace Soft
{
	class Pipeline
	{
	public:
		void SetViewPort(int x, int y, uint32_t width, uint32_t height)
		{
			m_ViewportWidth = width;
			m_ViewportHeight = height;

			m_ViewportX = x;
			m_ViewportY = y;
		}

		void BindPosition(const vec3& Position) { m_Position = Position; };
		void BindRotation(const vec3& Rotation) { m_Rotation = Rotation; };
		void BindColor(const vec3& Color) { m_Color = Color; }
		void BindTexture(const std::shared_ptr<Texture2D>& Texture) { m_Texture = Texture; }

		void DrawIndexed(const std::vector<vertex>& Vertices, const std::vector<uint32_t> Indices)
		{
			const auto& TransformedVertices = ProcessVertices(Vertices);
			AssembleTriangles(TransformedVertices, Indices);
		}

	private:
		// in Vertices >> Vertex Transformer Stage >> out Transformed Vertices
		std::vector<vertex> ProcessVertices(const std::vector<vertex>& Vertices)
		{
			std::vector<vertex> vertices = Vertices;

			for (auto& v : vertices)
			{
				mat3 rot =
					mat3::GetRotationX(m_Rotation.x * ToRad) *
					mat3::GetRotationY(m_Rotation.y * ToRad) *
					mat3::GetRotationZ(m_Rotation.z * ToRad);

				vec3 transf_v = { v.x, v.y, v.z };

				transf_v = rot * transf_v; // rotation
				transf_v += m_Position;

				v.x = transf_v.x;
				v.y = transf_v.y;
				v.z = transf_v.z;
			}

			return vertices;
		}

		// in Transformed Vertices And Indices >> Assemble Triangles Stage >> Triangles
		void AssembleTriangles(const std::vector<vertex>& TransformedVertices, const std::vector<uint32_t>& Indices)
		{
			for (int i = 0; i < Indices.size() / 3; i++)
			{
				int index1 = Indices[i * 3 + 0];
				int index2 = Indices[i * 3 + 1];
				int index3 = Indices[i * 3 + 2];


				auto v1 = TransformedVertices[index1];
				auto v2 = TransformedVertices[index2];
				auto v3 = TransformedVertices[index3];

				GenerateTriangle(v1, v2, v3);
			}
		}
		
		// get each triangle and process it using geometry shader >> post process
		void GenerateTriangle(vertex& v1, vertex& v2, vertex& v3)
		{
			// TODO:: make operations on triangle vertices before rendering them (vs, gs)
			PostProcessTriangleVertices(v1, v2, v3);
		}

		// convert from NDC to Screen Space + perspective division
		void PostProcessTriangleVertices(vertex& v1, vertex& v2, vertex& v3)
		{
			FromNDCToScreenSpace(v1); // from ndc to screen space
			FromNDCToScreenSpace(v2); // from ndc to screen space
			FromNDCToScreenSpace(v3); // from ndc to screen space

			uint32_t color = RGBA_COLOR((int)m_Color.x, (int)m_Color.y, (int)m_Color.z, 1);

			Soft::Renderer::DrawTriangle(
				v1,
				v2,
				v3,
				color,
				m_Texture.get());
		}

	private:
		void FromNDCToScreenSpace(vertex& v)
		{
			const float zInv = 1.0f / v.z;
			v.x = (v.x * zInv + 1.0f) * m_ViewportWidth * 0.5f;
			v.y = (-v.y * zInv + 1.0f) * m_ViewportHeight * 0.5f;
		}

	private:
		uint32_t m_ViewportWidth, m_ViewportHeight = 0;
		uint32_t m_ViewportX, m_ViewportY = 0;

		vec3 m_Rotation = vec3(0.0f);
		vec3 m_Position = { 0.0f, 0.0f, 2.0f };
		vec3 m_Color = vec3(0.0f);
		std::shared_ptr<Texture2D> m_Texture = nullptr;
	};
}

/*
			vec3 edge1(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z);
			vec3 edge2(v3.x - v1.x, v3.y - v1.y, v3.z - v1.z);
			vec3 normal = edge1.cross(edge2);

			vec3 viewVector = -vec3(v1.x, v1.y, v1.z);
			float dotProduct = normal.dot(viewVector);
*/