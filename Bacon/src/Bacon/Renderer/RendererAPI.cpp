#include "bcpch.h"
#include "Bacon/Renderer/RendererAPI.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Bacon {

	RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;

    Scope<RendererAPI> RendererAPI::Create()
    {
        switch (s_API)
        {
        case RendererAPI::API::None:    BC_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL:  return CreateScope<OpenGLRendererAPI>();
        }

        BC_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

} // namespace Bacon