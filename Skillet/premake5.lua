project "Skillet"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime"on"

    targetdir("%{wks.location}/bin/" .. outputdir .. "/")
    objdir("%{wks.location}/bin-int/" .. outputdir .. "/")

    files
    {
        "src/**.h",
        "src/**.cpp"
    }

    includedirs
    {
        "%{wks.location}/Bacon/vendor/spdlog/include",
        "%{wks.location}/Bacon/src",
        "%{wks.location}/Bacon/vendor",
        "%{IncludeDir.glm}",
        "%{IncludeDir.entt}",
    }

    links
    {
        "Bacon"
    }

    filter "system:windows"
        systemversion "latest"

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