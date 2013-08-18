
-- Import precore

local env_name = "PRECORE_ROOT"
local root_path = os.getenv(env_name)

if nil == root_path or 0 == #root_path then
	error(
		"Environment variable '" .. env_name .. "' is not " ..
		"defined or is blank; assign it to precore's root directory"
	)
end

dofile(path.join(os.getenv(env_name), "precore.lua"))

-- Utilities

function action_clean()
	if "clean" == _ACTION then
		for _, pc_sol in pairs(precore.state.solutions) do
			for _, pc_proj in pairs(pc_sol.projects) do
				os.rmdir(path.join(pc_proj.obj.basedir, "out"))
			end
		end
	end
end

function make_test(group, name, srcglob, configs)
	precore.make_project(
		group .. "_" .. name,
		"C++", "ConsoleApp",
		"./", "out/",
		nil, configs
	)

	if nil == configs then
		precore.apply("hord-strict")
	end

	if nil == srcglob then
		srcglob = name .. ".cpp"
	end

	configuration {}
		targetname(name)
		files {
			srcglob
		}
end

function make_tests(group, tests)
	for name, test in pairs(tests) do
		make_test(group, name, test[0], test[1])
	end
end

-- Custom precore configs

precore.make_config(
"hord-strict", {{
project = function()
	-- NB: -Werror is a pita for GCC. Good for testing, though,
	-- since its error checking is better.
	configuration {"clang"}
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

			"-Wunused"
		}
end}})

precore.make_config(
"hord-deps", {{
project = function()
	configuration {}
		includedirs {
			precore.subst("${ROOT}/dep/duct/"),
			precore.subst("${ROOT}/dep/murk/include/"),
			precore.subst("${ROOT}/dep/trait_wrangler/"),
			precore.subst("${ROOT}/dep/ceformat/")
		}
end}})

precore.make_config(
"murk-import", {{
project = function()
	configuration {}
		includedirs {
			precore.subst("${ROOT}/dep/murk/include/")
		}
		libdirs {
			precore.subst("${ROOT}/dep/murk/lib/")
		}

	configuration {"debug"}
		links {"murk_d"}

	configuration {"release"}
		links {"murk"}
end}})

precore.make_config(
"hord-import", {{
project = function()
	configuration {}
		includedirs {
			precore.subst("${ROOT}/include/")
		}
		libdirs {
			precore.subst("${ROOT}/lib/")
		}

	configuration {"debug"}
		links {"hord_d"}

	configuration {"release"}
		links {"hord"}
end}})

-- Initialize precore

precore.init(
	nil,
	{
		"opt-clang",
		"c++11-core",
		"precore-env-root"
	}
)
