#include <Bacon.h>
#include <Bacon/Core/EntryPoint.h>

#include "Sandbox2D.h"
#include "ExampleLayer.h"

class Sandbox : public Bacon::Application
{
public:
	Sandbox()
	{
		// PushLayer(new ExampleLayer());
		PushLayer(new Sandbox2D());
	}
	
	~Sandbox()
	{
	}
};

Bacon::Application* Bacon::CreateApplication()
{
	return new Sandbox();
}