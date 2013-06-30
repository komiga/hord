
-- premake common

newoption {
	trigger = "clang",
	description = "Use Clang in-place of GCC",
}

if _OPTIONS["clang"] then
	premake.gcc.cc = "clang"
	premake.gcc.cxx = "clang++"
end

function create_project(name, kind, tdir, root)
	local proj = project(name)
	proj.language = "C++"
	proj.kind = kind

	targetname(name)
	objdir("out/")
	targetdir(tdir)

	configuration {"debug"}
		defines {"DEBUG", "_DEBUG"}
		flags {"ExtraWarnings", "Symbols"}

	configuration {"release"}
		defines {"NDEBUG"}
		flags {"ExtraWarnings", "Optimize"}

	configuration {"linux"}
		buildoptions {"-std=c++11"}
		buildoptions {
			"-pedantic-errors",
			"-Werror",
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

	configuration {"linux", "not clang"}
		-- NB: -Werror is a pita for GCC. Good for testing, though,
		-- since its error checking is better.

	configuration {"linux", "clang"}
		buildoptions {"-Werror"}
		buildoptions {"-stdlib=libstdc++"}
		links {"stdc++"}

	configuration {}
		includedirs {
			root .. "/include/",
			root .. "/dep/duct/",
			root .. "/dep/murk/include/",
			root .. "/dep/trait_wrangler/",
			root .. "/dep/ceformat/"
		}
	return proj
end

function create_test(group, name, src)
	local root = "../.."
	create_project(group .. "_" .. name, "ConsoleApp", ".", root)

	configuration {}
		targetname(name)
		libdirs {
			root .. "/lib/",
			root .. "/dep/murk/lib/"
		}
		includedirs {
			--root .. "/include/"
		}
		files {
			src
		}

	configuration {"debug"}
		links {
			"hord_d",
			"murk_d"
		}
	configuration {"release"}
		links {
			"hord",
			"murk"
		}
end

function create_tests(group, tests)
	for _, t in ipairs(tests) do
		create_test(group, t[1], t[2])
	end
end
