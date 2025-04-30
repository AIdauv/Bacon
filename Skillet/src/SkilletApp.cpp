#include <Bacon.h>
#include <Bacon/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Bacon {

    class Skillet : public Application
    {
    public:
        Skillet(ApplicationCommandLineArgs args)
            : Application("Skillet", args)
        {
            PushLayer(new EditorLayer());
        }

        ~Skillet()
        {
        }
    };

	Application* CreateApplication(ApplicationCommandLineArgs args)
    {
        return new Skillet(args);
    }

}
