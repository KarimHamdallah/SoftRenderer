#include "Soft/Application.h"
#include "Soft/Input.h"
#include "Soft/Pipeline.h"
#include "Soft/mesh.h"

void OnResize(uint32_t width, uint32_t height);
std::shared_ptr<Soft::Pipeline<NormaledVertexEffect>> NormaledTrianglePipeline;

int main()
{
	// App
	Soft::AppSpecification AppSpec{ 800, 800, "soft" };
	Soft::Application* App = new Soft::Application(AppSpec);
	App->SetOnResizeCallBack(OnResize);
	
	// Renderer
	Soft::Renderer::Init(800, 800);
	
	// Pipeline
	NormaledTrianglePipeline = std::make_shared<Soft::Pipeline<NormaledVertexEffect>>(0, 0, 800, 800);
	NormaledTrianglePipeline->EnableBackFaceCulling(true);

	// start

	// models
	std::shared_ptr<Soft::Mesh> model = std::make_shared<Soft::Mesh>("res/bunny.obj");

	
	// Input
	float thetaX = 0.0f;
	float thetaY = 0.0f;
	float thetaZ = 0.0f;

	vec3 Pos = { 0.0f, 0.0f, 10.0f };

	std::shared_ptr<Texture2D> Tex = std::make_shared<Texture2D>("res/flag.png", true);

	while (!App->ShouldClose())
	{
		Soft::Renderer::ClearScreen(0x292b29);


		// update
		// Handle keyboard input
		if (Soft::Input::IsKeyPressed(Soft::Key::Q))
			thetaX -= 1.0f;
		if (Soft::Input::IsKeyPressed(Soft::Key::W))
			thetaY += 10.0f;
		if (Soft::Input::IsKeyPressed(Soft::Key::E))
			thetaZ += 1.0f;

		if (Soft::Input::IsKeyPressed(Soft::Key::Up))
			Pos.z += 1.0f;
		if (Soft::Input::IsKeyPressed(Soft::Key::Down))
			Pos.z -= 1.0f;
		if (Soft::Input::IsKeyPressed(Soft::Key::Right))
			Pos.x += 1.0f;
		if (Soft::Input::IsKeyPressed(Soft::Key::Left))
			Pos.x -= 1.0f;


		// render
		Soft::Renderer::BeginFrame();


		NormaledTrianglePipeline->BindPosition(Pos);
		NormaledTrianglePipeline->BindRotation({ thetaX, thetaY, thetaZ });
		NormaledTrianglePipeline->BindTexture(Tex);

		NormaledTrianglePipeline->DrawIndexed(model->GetVertices(), model->GetIndices());

		//thetaY -= 5.0f;


		Soft::Renderer::Present();

		App->Update();
	}
	Soft::Renderer::Destroy();
}

void OnResize(uint32_t width, uint32_t height)
{
	Soft::Renderer::Resize(width, height);
	NormaledTrianglePipeline->SetViewPort(0, 0, width, height);
}
