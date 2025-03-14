#include <Bacon.h>

class ExampleLayer : public Bacon::Layer
{
public:
	ExampleLayer()
		:Layer("Example")
	{

	}


	void OnUpdate() override
	{
		//BC_INFO("ExampleLayer::Update");

		if (Bacon::Input::IsKeyPressed(BC_KEY_TAB))
			BC_TRACE("Tab key is pressed!");
	}

	void OnEvent(Bacon::Event& event) override
	{
		//BC_TRACE("{0}", event.ToString());
	}
};

class Sandbox : public Bacon::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}
	
	~Sandbox()
	{

	}

private:

};

Bacon::Application* Bacon::CreateApplication()
{
	return new Sandbox();
}