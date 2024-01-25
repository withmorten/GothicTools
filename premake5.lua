workspace "GothicTools"
	configurations { "Debug", "Release" }
	platforms { "x86", "x64" }

	startproject "GothicZEN"

	location "build"

	characterset ("MBCS")
	toolset ("v141_xp")
	floatingpoint "Fast"

	defines { "WIN32", "WIN32_LEAN_AND_MEAN", "_CRT_SECURE_NO_WARNINGS", "_CRT_NONSTDC_NO_DEPRECATE", "_CRT_NONSTDC_NO_WARNINGS" }

	staticruntime "on"

	filter "configurations:Debug"
		-- editandcontinue "off"
		-- flags { "NoIncrementalLink" }
		defines { "_DEBUG", "_CRTDBG_MAP_ALLOC" }
		symbols "full"
		optimize "off"
		inlining "explicit"
		runtime "debug"

	filter { "configurations:Release" }
		defines { "NDEBUG" }
		runtime "release"
		symbols "on"
		optimize "speed"
		inlining "auto"
		flags { "LinkTimeOptimization" }

	filter { "platforms:x86" }
		vectorextensions "IA32"
		architecture "x86"

	filter { "platforms:x64" }
		architecture "x64"

project "xxhash"
	kind "StaticLib"
	language "C++"
	targetname "xxhash"
	targetdir "build/obj/%{cfg.platform}/%{cfg.buildcfg}/xxhash"

	files { "src/xxhash/*.*" }

	disablewarnings { "4028" }

project "GothicTools"
	kind "StaticLib"
	language "C++"
	targetname "GothicTools"
	targetdir "build/obj/%{cfg.platform}/%{cfg.buildcfg}/GothicTools"

	dependson { "xxhash" }
	links { "xxhash" }

	files { "src/GothicTools.h" }
	files { "src/3D.h" }
	files { "src/Algebra.h" }
	files { "src/Archiver.h" }
	files { "src/Bsp.h" }
	files { "src/Color.h" }
	files { "src/Container.h" }
	files { "src/Disk.h" }
	files { "src/Material.h" }
	files { "src/Memory.h" }
	files { "src/Mesh.h" }
	files { "src/Model.h" }
	files { "src/ModelAni.h" }
	files { "src/MorphMesh.h" }
	files { "src/Object.h" }
	files { "src/ObjectRegistry.h" }
	files { "src/ProgMesh.h" }
	files { "src/ScanDir.h" }
	files { "src/String.h" }
	files { "src/Texture.h" }
	files { "src/Visual.h" }
	files { "src/Vob.h" }
	files { "src/WayNet.h" }
	files { "src/World.h" }

	files { "src/GothicTools.cpp" }
	files { "src/Archiver.cpp" }
	files { "src/Bsp.cpp" }
	files { "src/Disk.cpp" }
	files { "src/Material.cpp" }
	files { "src/Mesh.cpp" }
	files { "src/Model.cpp" }
	files { "src/ModelAni.cpp" }
	files { "src/MorphMesh.cpp" }
	files { "src/Object.cpp" }
	files { "src/ObjectRegistry.cpp" }
	files { "src/ProgMesh.cpp" }
	files { "src/ScanDir.cpp" }
	files { "src/String.cpp" }
	files { "src/Texture.cpp" }
	files { "src/Visual.cpp" }
	files { "src/Vob.cpp" }
	files { "src/VobDiff.cpp" }
	files { "src/WayNet.cpp" }
	files { "src/World.cpp" }

project "GothicZEN"
	kind "ConsoleApp"
	language "C++"
	targetname "GothicZEN"
	targetdir "bin/%{cfg.platform}/%{cfg.buildcfg}"

	dependson { "GothicTools" }
	links { "GothicTools" }

	files { "src/GothicZEN.cpp" }

	if (os.getenv("BIN") ~= nil) then
		postbuildcommands { '{COPYFILE} "%{cfg.buildtarget.abspath}" "' .. os.getenv("BIN") .. "/" .. '%{cfg.buildtarget.name}"' }
	end

project "GothicAnims"
	kind "ConsoleApp"
	language "C++"
	targetname "GothicAnims"
	targetdir "bin/%{cfg.platform}/%{cfg.buildcfg}"

	dependson { "GothicTools" }
	links { "GothicTools" }

	files { "src/GothicAnims.cpp" }

	if (os.getenv("BIN") ~= nil) then
		postbuildcommands { '{COPYFILE} "%{cfg.buildtarget.abspath}" "' .. os.getenv("BIN") .. "/" .. '%{cfg.buildtarget.name}"' }
	end

project "GothicDiff"
	kind "ConsoleApp"
	language "C++"
	targetname "GothicDiff"
	targetdir "bin/%{cfg.platform}/%{cfg.buildcfg}"

	dependson { "GothicTools" }
	links { "GothicTools" }

	files { "src/GothicDiff.cpp" }

	if (os.getenv("BIN") ~= nil) then
		postbuildcommands { '{COPYFILE} "%{cfg.buildtarget.abspath}" "' .. os.getenv("BIN") .. "/" .. '%{cfg.buildtarget.name}"' }
	end
