
local S, G, R = precore.helpers()

precore.import(G"${DEP_PATH}/duct")
precore.import(G"${DEP_PATH}/trait_wrangler")
precore.import(G"${DEP_PATH}/ceformat")
precore.import(G"${DEP_PATH}/Cacophony")
precore.import(G"${DEP_PATH}/am")

precore.make_config_scoped("hord.env", {
	once = true,
}, {
{global = function()
	precore.define_group("HORD", os.getcwd())
end}})

precore.make_config("hord.strict", nil, {
{project = function()
	configuration {}
		flags {
			"FatalWarnings"
		}

	configuration {"linux"}
		buildoptions {
			"-pedantic-errors",
			"-Wextra",

			"-Wuninitialized",
			"-Winit-self",

			"-Wmissing-field-initializers",
			"-Wredundant-decls",

			"-Wfloat-equal",
			"-Wold-style-cast",

			"-Wnon-virtual-dtor",
			"-Woverloaded-virtual",

			"-Wunused",
			"-Wundef",
		}
end}})

precore.make_config("hord.dep", nil, {
"duct.dep",
"trait_wrangler.dep",
"ceformat.dep",
"cacophony.dep",
"am.dep",
{project = function(p)
	configuration {}
		includedirs {
			G"${HORD_ROOT}/include/",
		}

	if not p.env["NO_LINK"] then
		libdirs {
			G"${HORD_BUILD}/lib/",
		}

		configuration {"debug"}
			links {"hord_d"}

		configuration {"release"}
			links {"hord"}
	end
end}})

precore.make_config_scoped("hord.projects", {
	once = true,
}, {
{global = function()
	precore.make_solution(
		"Beard",
		{"debug", "release"},
		{"x64", "x32"},
		nil,
		{
			"precore.generic",
		}
	)

	precore.make_project(
		"lib",
		"C++", "SharedLib",
		G"${HORD_BUILD}/lib/",
		G"${HORD_BUILD}/out/",
		{
			NO_LINK = true,
		}, {
			"hord.strict",
			"hord.dep",
		}
	)

	configuration {"debug"}
		targetname("hord")
		targetsuffix("_d")

	configuration {}
		files {
			"src/Hord/**.cpp",
		}
end}})

precore.apply_global({
	"precore.env-common",
	"hord.env",
})
