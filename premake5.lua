include "./vendor/premake/premake_customization/solution_items.lua"
include "Dependencies.lua"

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

group "Dependencies"
    include "vendor/premake"
    include "Bacon/vendor/Box2D"
    include "Bacon/vendor/GLFW"
    include "Bacon/vendor/Glad"
    include "Bacon/vendor/imgui"
    include "Bacon/vendor/yaml-cpp"
group ""

group "Core"
	include "Bacon"
	include "Bacon-ScriptCore"
group ""

group "Tools"
	include "Skillet"
group ""

group "Misc"
	include "Sandbox"
group ""


