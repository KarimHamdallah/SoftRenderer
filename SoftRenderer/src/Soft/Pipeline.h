#pragma once
#include "Renderer.h"
#include <algorithm>
#include <GLFW/glfw3.h>

namespace Soft
{
	template<class Effect>
	class Pipeline
	{
		typedef typename Effect::vertex vertex;
	public:
		Pipeline(int x, int y, uint32_t width, uint32_t height)
		{
			SetViewPort(x, y, width, height);
		}

		void SetViewPort(int x, int y, uint32_t width, uint32_t height)
		{
			m_ViewportWidth = width;
			m_ViewportHeight = height;

			m_ViewportX = x;
			m_ViewportY = y;
		}

		void BindPosition(const vec3& Position) { m_Position = Position; };
		void BindRotation(const vec3& Rotation) { m_Rotation = Rotation; };
		void BindTexture(const std::shared_ptr<Texture2D>& Texture) { m_Effect.ps.m_Texture = Texture; }
		void EnableBackFaceCulling(bool cull) { m_CullBackFaces = cull; }


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

			
			DrawTriangle(
				v1,
				v2,
				v3,
				1);

			/*
			Renderer::DrawLine(v1.x, v1.y, v2.x, v2.y, 0xFFFFFFFF);
			Renderer::DrawLine(v2.x, v2.y, v3.x, v3.y, 0xFFFFFFFF);
			Renderer::DrawLine(v3.x, v3.y, v1.x, v1.y, 0xFFFFFFFF);
			*/
		}

	private:
		void FromNDCToScreenSpace(vertex& v)
		{
			const float zInv = 1.0f / v.z;
			v = v * zInv;
			v.x = (v.x + 1.0f) * m_ViewportWidth * 0.5f;
			v.y = (-v.y + 1.0f) * m_ViewportHeight * 0.5f;	
			v.z = zInv;
		}

		void DrawTriangle(vertex A, vertex B, vertex C, uint32_t color)
		{
			// Calculate the edge function for the whole triangle (ABC)
			int ABC = edgeFunction(A, B, C);

			if (ABC < 0.0f && m_CullBackFaces) // back face culling
				return;

			// Initialise our Vertex
			vertex P;


			// Get the bounding box of the triangle
			int minX = std::min(std::min(A.x, B.x), C.x) - 1;
			int minY = std::min(std::min(A.y, B.y), C.y) - 1;
			int maxX = std::max(std::max(A.x, B.x), C.x) + 1;
			int maxY = std::max(std::max(A.y, B.y), C.y) + 1;

			// Loop through all the pixels of the bounding box
			for (P.y = minY; P.y < maxY; P.y++)
			{
				for (P.x = minX; P.x < maxX; P.x++)
				{
					// Calculate our edge functions
					int ABP = edgeFunction(A, B, P);
					int BCP = edgeFunction(B, C, P);
					int CAP = edgeFunction(C, A, P);

					// Normalise the edge functions by dividing by the total area to get the barycentric coordinates
					float weightA = (float)BCP / ABC;
					float weightB = (float)CAP / ABC;
					float weightC = (float)ABP / ABC;

					// If all the edge functions are positive, the Vertex is inside the triangle
					if (ABP >= 0 && BCP >= 0 && CAP >= 0)
					{
						// interpolate whole vertex attributes
						vertex InterpolatedVertex = A * weightA + B * weightB + C * weightC;

						const float z = 1 / InterpolatedVertex.z;
						InterpolatedVertex = InterpolatedVertex * z;

						if (true || Renderer::TestAndSet((int)P.x, (int)P.y, z)) // depth test
						{
							// Draw the pixel
							if (P.x < m_ViewportWidth && P.x > 0 && P.y < m_ViewportHeight && P.y > 0)
							{
								Renderer::PutPixel((int)P.x, (int)P.y, m_Effect.ps.GetColor(InterpolatedVertex));
							}
						}
					}
				}
			}
		}

		int edgeFunction(vertex a, vertex b, vertex c)
		{
			return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
		};


	private:
		Effect m_Effect;

		uint32_t m_ViewportWidth, m_ViewportHeight = 0;
		uint32_t m_ViewportX, m_ViewportY = 0;

		vec3 m_Rotation = vec3(0.0f);
		vec3 m_Position = { 0.0f, 0.0f, 2.0f };
		vec3 m_Color = vec3(0.0f);
		bool m_CullBackFaces = true;
	};
}

/*
			vec3 edge1(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z);
			vec3 edge2(v3.x - v1.x, v3.y - v1.y, v3.z - v1.z);
			vec3 normal = edge1.cross(edge2);

			vec3 viewVector = -vec3(v1.x, v1.y, v1.z);
			float dotProduct = normal.dot(viewVector);
*/