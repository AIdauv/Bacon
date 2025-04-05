workspace "Bacon"
    architecture "x64"
    startproject "Sandbox"

    configurations 
    {
        "Debug",
        "Release",
        "Dist"
    }


outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"


IncludeDir = {}
IncludeDir["GLFW"] = "Bacon/vendor/GLFW/include"
IncludeDir["Glad"] = "Bacon/vendor/Glad/include"
IncludeDir["ImGui"] = "Bacon/vendor/imgui"
IncludeDir["glm"] = "Bacon/vendor/glm"
IncludeDir["stb_image"] = "Bacon/vendor/stb_image"

include "Bacon/vendor/GLFW"
include "Bacon/vendor/Glad"
include "Bacon/vendor/imgui"

project "Bacon"
    location "Bacon"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime"on"

    targetdir("bin/" .. outputdir .. "/%{prj.name}")
    objdir("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "bcpch.h"
    pchsource "Bacon/src/bcpch.cpp"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/vendor/stb_image/**.h",
        "%{prj.name}/vendor/stb_image/**.cpp",
        "%{prj.name}/vendor/glm/glm/**.hpp",
        "%{prj.name}/vendor/glm/glm/**.inl"
    }

    defines
    {
        "_CRT_SECURE_NO_WARNINGS"
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.stb_image}"
    }

    links 
    {
        "GLFW",
        "Glad",
        "ImGui",
        "opengl32.lib",
        --"gdi32",
        --"user32",
        --"shell32",
        --"ole32",
        --"Dwmapi.lib"
    }

    filter "system:windows"
        systemversion "latest"
        --buildoptions { "/utf-8" }

        defines
        {
            "BC_PLATFORM_WINDOWS",
            "BC_BUILD_DLL",
            "GLFW_INCLUDE_NONE"
        }


    filter "configurations:Debug"
        defines "BC_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "BC_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "BC_DIST"
        runtime "Release"
        optimize "on"

project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime"on"

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
        "Bacon/src",
        "Bacon/vendor",
        "%{IncludeDir.glm}"
    }

    links
    {
        "Bacon"
    }

    filter "system:windows"
        systemversion "latest"
        --buildoptions { "/utf-8" }

        defines
        {
            "BC_PLATFORM_WINDOWS"
        }


    filter "configurations:Debug"
        defines "BC_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "BC_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "BC_DIST"
        runtime "Release"
        optimize "on"