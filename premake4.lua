
-- Hord premake file

newoption {
	trigger="clang",
	description="Use Clang in-place of GCC",
}

if _OPTIONS["clang"] then
	premake.gcc.cc="clang"
	premake.gcc.cxx="clang++"
end

function create_project(name, kind, tdir)
	local proj=project(name)
		proj.language="C++"
		proj.kind=kind

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
		buildoptions {
			--"-Wextra",
			"-Wunused-parameter",
			"-Wuninitialized",
			"-Wmissing-field-initializers",
			"-Wredundant-decls",
			"-Wfloat-equal"
		}
		buildoptions {"-std=c++0x", "-pedantic", "-Wextra"}

	configuration {"clang"}
		buildoptions {"-stdlib=libstdc++"}
		links {
			"stdc++"
		}

	configuration {}
		includedirs {
			--"dep/duct/",
		}
	return proj
end

solution("Hord")
	configurations {"debug", "release"}
	platforms {"x32", "x64"}

-- Core library
create_project("hord", "StaticLib", "lib/")

configuration {}
	includedirs {
		"include/"
	}
	files {
		"src/Hord/**"
	}
