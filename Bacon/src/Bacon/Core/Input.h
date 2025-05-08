#pragma once

#include "Bacon/Core/KeyCodes.h"
#include "Bacon/Core/MouseCodes.h"

#include <glm/glm.hpp>

namespace Bacon {

    class Input
    {
    public:
        static bool IsKeyPressed(KeyCode key);

        static bool IsMouseButtonPressed(MouseCode button);
        static glm::vec2 GetMousePosition();
        static float GetMouseX();
        static float GetMouseY();
    };
	
}
