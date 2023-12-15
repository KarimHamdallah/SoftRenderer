#pragma once
#include <functional>
#include "mesh.h"
#include <glm.hpp>
#include "Texture.h"

namespace Soft
{
	template<class V>
	class Triangle
	{
	public:
		V v1;
		V v2;
		V v3;
	};


	class Pipeline
	{
	public:
		void SetViewPort(int x, int y, uint32_t width, uint32_t height);

		// Fixed Pipeline
		// Triangle Vertices And Indices List
		//                                                                    Split
		// * Vertices >> Vertex Transformer Stage (output Transformed Vertices)   |  Indices >> pass without processing
		// * Triangle Assembler Get Vertices And Indices And Store Array Of Triangles Which each one Is A Triangle With 3 Vertices >> Back Face Culling Here We See If This Is Back Face Or Not
		// * Perspective Screen Transformer (Take Assembeled Triangles And Do ScreenSpace Conversion And Perspective Devide)
		// * Triangle Rasterizer (Take output Triangles From Last Stage And Rasterize Triangle Also 
		// Get Interpolate vertices And Store Interpolated Texture coordinates for each pixel)


		void BindPosition(const glm::vec3& Position) { m_Position = Position; };
		void BindRotation(const glm::vec3& Rotation) { m_Rotation = Rotation; };
		void BindColor(const glm::vec4& Color) { m_Color = Color; }
		void BindTexture(const std::shared_ptr<Texture2D>& Texture) { m_Texture = Texture; }
		void EnableBackFaceCulling(bool enable) { m_BackFaceCulling = enable; }
		void EnableFillTriangles(bool fill) { m_FillTriangles = fill; }

		void DrawIndexed(const std::vector<Vertex>& Vertices, const std::vector<uint32_t> Indices);

	private:
		// in Vertices >> Vertex Transformer Stage >> out Transformed Vertices
		std::vector<Vertex> ProcessVertices(const std::vector<Vertex>& Vertices);
		// in Transformed Vertices And Indices >> Assemble Triangles Stage >> Triangles
		void AssembleTriangles(const std::vector<Vertex>& TransformedVertices, const std::vector<uint32_t>& Indices);
		// get each triangle and process it using geometry shader >> post process
		void GenerateTriangle(Vertex& v1, Vertex& v2, Vertex& v3);
		// convert from NDC to Screen Space + perspective division
		void PostProcessTriangleVertices(Vertex& v1, Vertex& v2, Vertex& v3);

	private:
		void FromNDCToScreenSpace(Vertex& v);
		void PerspectiveDevide(Vertex& v);

	private:
		glm::vec3 m_Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		glm::vec4 m_Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		std::shared_ptr<Texture2D> m_Texture;
		bool m_BackFaceCulling = false;
		bool m_FillTriangles = true;

		int m_ViewPort_x, m_ViewPort_y = 0;
		uint32_t m_ViewPortWidth, m_ViewPortHeight = 0;
	};
}
