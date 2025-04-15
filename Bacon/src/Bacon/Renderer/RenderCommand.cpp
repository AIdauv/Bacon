#include "bcpch.h"
#include "Bacon/Renderer/RenderCommand.h"

namespace Bacon{

	Scope<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();

}