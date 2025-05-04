workspace "gay"
architecture "x86_64"
configurations {"debug", "release"}
startproject "gay"

language "C++"
cppdialect "C++latest"
cdialect "C17"
staticruntime "On"

targetdir "%{wks.location}/bin/%{cfg.system}_%{cfg.buildcfg}"
objdir "%{wks.location}/bin_int/%{cfg.system}_%{cfg.buildcfg}"

stringpooling "on"
flags "MultiProcessorCompile"

kind "StaticLib"

filter "configurations:debug"
runtime "Debug"
optimize "Debug"
symbols "On"
defines "_DEBUG"

filter "configurations:release"
runtime "Release"
optimize "Speed"
symbols "On"
defines "NDEBUG"

filter "system:windows"
systemversion "latest"
defines { "NOMINMAX", "WIN32_LEAN_AND_MEAN"}
buildoptions { "/EHsc", "/Zc:throwingNew", "/Zc:preprocessor", "/Zc:__cplusplus", "/experimental:c11atomics"}

include "source/build.lua"

group "dependencies"
for _, matchedfile in ipairs(os.matchfiles("premake/*.lua")) do
    include(matchedfile)
end