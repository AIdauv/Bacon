#include "bcpch.h"
#include "Bacon/Core/Window.h"

#ifdef BC_PLATFORM_WINDOWS
#include "Platform/Windows/WindowsWindow.h"
#endif

namespace Bacon
{

    Scope<Window> Window::Create(const WindowProps& props)
    {
#ifdef BC_PLATFORM_WINDOWS
        return CreateScope<WindowsWindow>(props);
#else
        BC_CORE_ASSERT(false, "Unknown platform!");
        return nullptr;
#endif
    }

}