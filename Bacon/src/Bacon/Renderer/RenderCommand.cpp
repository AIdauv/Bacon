#include "bcpch.h"
#include "RenderCommand.h"

#include "platform/OpenGL/OpenGLRendererAPI.h"

namespace Bacon{

	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
}