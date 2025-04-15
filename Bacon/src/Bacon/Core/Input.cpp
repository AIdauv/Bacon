#include "bcpch.h"
#include "Bacon/Core/Input.h"

#ifdef BC_PLATFORM_WINDOWS
#include "Platform/Windows/WindowsInput.h"
#endif

namespace Bacon {

    Scope<Input> Input::s_Instance = Input::Create();

    Scope<Input> Input::Create()
    {
#ifdef BC_PLATFORM_WINDOWS
        return CreateScope<WindowsInput>();
#else
        BC_CORE_ASSERT(false, "Unknown platform!");
        return nullptr; 3Has conversations.Original line has conversations.
#endif
    }
}