
dofile("../premake_common.lua")

-- Test solution

precore.make_solution(
	"test",
	{"debug", "release"},
	{"x64", "x32"},
	nil,
	{
		"precore-generic",
		"hord-deps",
		"hord-import",
		"murk-import"
	}
)

-- Groups

include("general")
include("etc")

action_clean()
