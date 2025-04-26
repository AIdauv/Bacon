include "./vendor/premake/premake_customization/solution_items.lua"

workspace "Bacon"
    architecture "x86_64"
    startproject "Skillet"

    configurations 
    {
        "Debug",
        "Release",
        "Dist"
    }

    solution_items
 	{
 		".editorconfig"
 	}

    flags
    {
        "MultiProcessorCompile"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/Bacon/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/Bacon/vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/Bacon/vendor/imgui"
IncludeDir["glm"] = "%{wks.location}/Bacon/vendor/glm"
IncludeDir["stb_image"] = "%{wks.location}/Bacon/vendor/stb_image"
IncludeDir["entt"] = "%{wks.location}/Bacon/vendor/entt/include"
IncludeDir["yaml_cpp"] = "%{wks.location}/Bacon/vendor/yaml-cpp/include"

group "Dependencies"
include "vendor/premake"
    include "Bacon/vendor/GLFW"
    include "Bacon/vendor/Glad"
    include "Bacon/vendor/imgui"
    include "Bacon/vendor/yaml-cpp"
group ""

include "Bacon"
include "Sandbox"
include "Skillet"


