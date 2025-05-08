#include <Bacon.h>
#include <Bacon/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Bacon {

    class Skillet : public Application
    {
    public:
        Skillet(const ApplicationSpecification& spec)
            : Application(spec)
        {
            PushLayer(new EditorLayer());
        }
    };

	Application* CreateApplication(ApplicationCommandLineArgs args)
    {
		ApplicationSpecification spec;
		spec.Name = "Skillet";
		spec.CommandLineArgs = args;

		return new Skillet(spec);
    }

}
