
dofile("premake_common.lua")

-- Core solution

precore.make_solution(
	"Hord",
	{"debug", "release"},
	{"x64", "x32"},
	nil,
	{
		"precore-generic",
		"hord-strict",
		"hord-deps"
	}
)

-- Core library

precore.make_project(
	"hord",
	"C++", "SharedLib",
	"lib/", "out/",
	nil, nil
)

configuration {"debug"}
	targetsuffix("_d")

configuration {}
	includedirs {
		"include/"
	}
	files {
		"src/Hord/**.cpp"
	}

precore.action_clean("out", "lib")
