#include <Bacon.h>

class Sandbox : public Bacon::Application
{
public:
	Sandbox()
	{

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