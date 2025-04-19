#include <Bacon.h>
#include <Bacon/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Bacon {

    class Skillet : public Application
    {
    public:
        Skillet()
            : Application("Skillet")
        {
            PushLayer(new EditorLayer());
        }

        ~Skillet()
        {
        }
    };

    Application* CreateApplication()
    {
        return new Skillet();
    }

}