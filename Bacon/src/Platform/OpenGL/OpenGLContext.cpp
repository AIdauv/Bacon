#include "bcpch.h"
#include "Platform/OpenGL/OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <GL/GL.h>

namespace Bacon {

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		BC_CORE_ASSERT(windowHandle, "Window handle is null!");
	}

	void OpenGLContext::Init()
	{
		BC_PROFILE_FUNCTION();

		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		BC_CORE_ASSERT(status, "Failed to initialize Glad!");

		 BC_CORE_INFO("OpenGL Info:");
		 BC_CORE_INFO("  Vendor: {0}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
		 BC_CORE_INFO("  Renderer: {0}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
		 BC_CORE_INFO("  Version: {0}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));

	#ifdef BC_ENABLE_ASSERTS
		 int versionMajor;
		 int versionMinor;
		 glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
		 glGetIntegerv(GL_MINOR_VERSION, &versionMinor);

		 BC_CORE_ASSERT(versionMajor > 4 || (versionMajor == 4 && versionMinor >= 5), "Bacon requires at least OpenGL version 4.5!");
	#endif
	}

	void OpenGLContext::SwapBuffers()
	{
		BC_PROFILE_FUNCTION();

		glfwSwapBuffers(m_WindowHandle);
	}

}