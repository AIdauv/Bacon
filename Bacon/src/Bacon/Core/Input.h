#pragma once

#include "Bacon/Core/Base.h"
#include "Bacon/Core/KeyCodes.h"
#include "Bacon/Core/MouseCodes.h"

namespace Bacon {

    class Input
    {
    public:
        static bool IsKeyPressed(KeyCode key);

        static bool IsMouseButtonPressed(MouseCode button);
        static std::pair<float, float> GetMousePosition();
        static float GetMouseX();
        static float GetMouseY();
    };
	
}