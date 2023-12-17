#include "Soft/Application.h"
#include "Soft/Input.h"
#include "Soft/Pipeline.h"
#include "Soft/Effects/TexturedVertexEffect.h"
#include "Soft/Effects/ColoredVertexEffect.h"

void OnResize(uint32_t width, uint32_t height);
std::shared_ptr<Soft::Pipeline<TexturedVertexEffect>> TexturedTrianglePipeline;
std::shared_ptr<Soft::Pipeline<ColoredVertexEffect>> ColoredTrianglePipeline;

int main()
{
	// App
	Soft::AppSpecification AppSpec{ 800, 800, "soft" };
	Soft::Application* App = new Soft::Application(AppSpec);
	App->SetOnResizeCallBack(OnResize);
	
	// Renderer
	Soft::Renderer::Init(800, 800);
	
	// Pipeline
	TexturedTrianglePipeline = std::make_shared<Soft::Pipeline<TexturedVertexEffect>>(0, 0, 800, 800);

	ColoredTrianglePipeline = std::make_shared<Soft::Pipeline<ColoredVertexEffect>>(0, 0, 800, 800);

	// start

	// Triangles
	IndexedTriangleList<TexturedVertexEffect::vertex> TexturedCube = Factory3D::GetIndexedTriangleCubeTextured(1.0f);
	IndexedTriangleList<ColoredVertexEffect::vertex> ColoredCube = Factory3D::GetIndexedTriangleCubeColored(1.0f);
	
	// Input
	float thetaX = 0.0f;
	float thetaY = 0.0f;
	float thetaZ = 0.0f;

	vec3 Pos = { 0.0f, 0.0f, 2.0f };

	std::shared_ptr<Texture2D> Tex = std::make_shared<Texture2D>("res/wood.jpg");

	while (!App->ShouldClose())
	{
		Soft::Renderer::ClearScreen(0xFF000000);


		// update
		// Handle keyboard input
		if (Soft::Input::IsKeyPressed(Soft::Key::Q))
			thetaX++;
		if (Soft::Input::IsKeyPressed(Soft::Key::W))
			thetaY++;
		if (Soft::Input::IsKeyPressed(Soft::Key::E))
			thetaZ++;

		if (Soft::Input::IsKeyPressed(Soft::Key::Up))
			Pos.z += 0.1f;
		if (Soft::Input::IsKeyPressed(Soft::Key::Down))
			Pos.z -= 0.1f;
		if (Soft::Input::IsKeyPressed(Soft::Key::Right))
			Pos.x += 0.1f;
		if (Soft::Input::IsKeyPressed(Soft::Key::Left))
			Pos.x -= 0.1f;


		// render
		Soft::Renderer::BeginFrame();


		TexturedTrianglePipeline->BindPosition(Pos);
		TexturedTrianglePipeline->BindRotation({ thetaX, thetaY, thetaZ });
		TexturedTrianglePipeline->BindTexture(Tex);

		TexturedTrianglePipeline->DrawIndexed(TexturedCube.vertices, TexturedCube.indices);

		
		ColoredTrianglePipeline->BindPosition({ 0.5f, 0.0f, 2.0f });
		ColoredTrianglePipeline->BindRotation({ thetaX, thetaY, thetaZ });

		ColoredTrianglePipeline->DrawIndexed(ColoredCube.vertices, ColoredCube.indices);

		Soft::Renderer::Present();

		App->Update();
	}
	Soft::Renderer::Destroy();
}

void OnResize(uint32_t width, uint32_t height)
{
	Soft::Renderer::Resize(width, height);
	TexturedTrianglePipeline->SetViewPort(0, 0, width, height);
	ColoredTrianglePipeline->SetViewPort(0, 0, width, height);
}
