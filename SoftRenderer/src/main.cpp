#include "Soft/Application.h"
#include "Soft/Input.h"
#include "Soft/Pipeline.h"
#include "Soft/mesh.h"

void OnResize(uint32_t width, uint32_t height);
std::shared_ptr<Soft::Pipeline> Pipeline;

int main()
{
	// App
	Soft::AppSpecification AppSpec{ 800, 800, "soft" };
	Soft::Application* App = new Soft::Application(AppSpec);
	App->SetOnResizeCallBack(OnResize);
	
	// Renderer
	Soft::Renderer::Init(800, 800);
	
	// Pipeline
	Pipeline = std::make_shared<Soft::Pipeline>();
	Pipeline->SetViewPort(0, 0, 800, 800);

	// start

	// Triangles
	std::shared_ptr<Soft::Mesh> cube_mesh = std::make_shared<Soft::Mesh>("res/cube.fbx");
	IndexedTriangleList Cube = Factory3D::GetIndexedTriangleCube(1.0f);
	
	// Input
	float thetaX = 0.0f;
	float thetaY = 0.0f;
	float thetaZ = 0.0f;

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

		// render

		Pipeline->BindPosition({ 0.0f, 0.0f, 2.0f });
		Pipeline->BindRotation({ thetaX, thetaY, thetaZ });
		Pipeline->BindColor({ 255.0f, 255.0f, 0.0f });
		Pipeline->BindTexture(Tex);

		Pipeline->DrawIndexed(Cube.vertices, Cube.indices);


		Soft::Renderer::Present();

		App->Update();
	}
	Soft::Renderer::Destroy();
}

void OnResize(uint32_t width, uint32_t height)
{
	Soft::Renderer::Resize(width, height);
	Pipeline->SetViewPort(0, 0, width, height);
}