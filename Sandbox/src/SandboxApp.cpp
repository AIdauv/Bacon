#include <Bacon.h>
#include <Bacon/Core/EntryPoint.h>

#include "perfTest.h"
#include "Sandbox2D.h"
#include "ExampleLayer.h"

class Sandbox : public Bacon::Application
{
public:
	Sandbox(const Bacon::ApplicationSpecification& specification)
		: Bacon::Application(specification)
	{
		// PushLayer(new ExampleLayer());
		// PushLayer(new Sandbox2D());
		PushLayer(new perfTest());
	}
	
	~Sandbox()
	{
	}
};

Bacon::Application* Bacon::CreateApplication(Bacon::ApplicationCommandLineArgs args)
{
	ApplicationSpecification spec;
	spec.Name = "Sandbox";
	spec.WorkingDirectory = "../Skillet";
	spec.CommandLineArgs = args;

	return new Sandbox(spec);
}
