#include "bcpch.h"
#include "Framebuffer.h"

#include "Bacon/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLFramebuffer.h"

namespace Bacon {

    Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None:    BC_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL:  return CreateRef<OpenGLFramebuffer>(spec);
        }

        BC_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

}
