#pragma once
#include <string>
#include <functional>

namespace Soft
{

	struct AppSpecification
	{
		uint32_t Width;
		uint32_t Height;
		std::string Title;
	};

	class Application
	{
	public:
		Application(const AppSpecification& spec);
		~Application();

		static Application& Get() { return *m_Instance; }

		uint32_t GetWidth() { return m_Data.Width; }
		uint32_t GetHeight() { return m_Data.Height; }


		bool ShouldClose();
		void Update();
		void SetOnResizeCallBack(std::function<void(uint32_t, uint32_t)> func) { m_Data.m_OnResizeFuncPtr = func; }
		void* GetNativeWindow() { return m_Data.WindowHandel; }


	private:
		inline static Application* m_Instance = nullptr;
		struct WindowData
		{
			uint32_t Width = 0;
			uint32_t Height = 0;
			void* WindowHandel = nullptr;
			std::function<void(uint32_t, uint32_t)> m_OnResizeFuncPtr = nullptr;
		};

		WindowData m_Data;
	};
}
