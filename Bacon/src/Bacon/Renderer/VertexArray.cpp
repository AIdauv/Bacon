#include "bcpch.h"
#include "Bacon/Renderer/VertexArray.h"

#include "Bacon/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Bacon {
	Ref<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    BC_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateRef<OpenGLVertexArray>();
		}
		BC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}