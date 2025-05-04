project "gay"
kind "WindowedApp"
debugdir "../working"

files{
    "**.c",
    "**.cpp",
    "**.h",
    "**.hpp",
    "**.inl",
}

includedirs{
    "%{prj.location}"
}

filter "system:windows"
links "opengl32"