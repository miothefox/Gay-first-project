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
    "%{prj.location}",
    "%{wks.location}/vendor/glad/include",
}

links {
    "glad",
}

filter "system:windows"
links "opengl32"