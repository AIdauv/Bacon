workspace "Bacon"
	architecture "x64"

	configurations 
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "Bacon/vendor/GLFW/include"

include "Bacon/vendor/GLFW"

project "Bacon"
	location "Bacon"
	kind "SharedLib"
	language "C++"

	targetdir("bin/" .. outputdir .. "/%{prj.name}")
	objdir("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "bcpch.h"
    pchsource "Bacon/src/bcpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
        "%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
        "%{IncludeDir.GLFW}"
	}

    links 
    {
        "GLFW",
        "opengl32.lib",
        --"gdi32",
        --"user32",
        --"shell32",
        --"ole32",
        --"Dwmapi.lib"
    }

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "On"
		systemversion "latest"
        buildoptions { "/utf-8" }

		defines
		{
			"BC_PLATFORM_WINDOWS",
			"BC_BUILD_DLL",
            "BC_ENABLE_ASSERTS"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
		}

	filter "configurations:Debug"
		defines "BC_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "BC_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "BC_DIST"
		optimize "On"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"

	targetdir("bin/" .. outputdir .. "/%{prj.name}")
	objdir("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Bacon/vendor/spdlog/include",
		"Bacon/src"
	}

	links
	{
		"Bacon"
	}

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "On"
		systemversion "latest"
        buildoptions { "/utf-8" }

		defines
		{
			"BC_PLATFORM_WINDOWS"
		}


	filter "configurations:Debug"
		defines "BC_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "BC_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "BC_DIST"
		optimize "On"