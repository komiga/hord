
-- Core

dofile("premake_common.lua")

solution("Hord")
	configurations {"debug", "release"}
	platforms {"x32", "x64"}

-- Core library
create_project("hord", "StaticLib", "lib/", ".")

configuration {}
	includedirs {
		"include/"
	}
	files {
		"src/Hord/**"
	}
