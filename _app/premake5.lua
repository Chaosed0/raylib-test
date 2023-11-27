
baseName = path.getbasename(os.getcwd());

project (baseName)
    kind "ConsoleApp"
    location "../_build"
    targetdir "../_bin/%{cfg.buildcfg}"

    filter "action:vs*"
        debugdir "$(SolutionDir)"
		
	filter {"action:vs*", "configurations:Release"}
		kind "WindowedApp"
		entrypoint "mainCRTStartup"
		
    filter{}

    vpaths 
    {
        ["Header Files/*"] = { "include/**.h",  "include/**.hpp", "src/**.h", "src/**.hpp", "**.h", "**.hpp"},
        ["Source Files/*"] = {"src/**.c", "src/**.cpp","**.c", "**.cpp"},
    }
    files {"**.c", "**.cpp", "**.h", "**.hpp"}
  
    includedirs { "./" }
    includedirs { "src" }
    includedirs { "include" }
    
    link_raylib()
	
	-- To link to a lib use link_to("LIB_FOLDER_NAME")
	link_to("lua54")
	link_to("yyjson")
	link_to("fastwfc")
	
	postbuildcommands {
		"{COPY} ../libs/lua54/lua54.dll %{cfg.targetdir}",
		"{COPY} ../libs/fastwfc/fastwfc.dll %{cfg.targetdir}",
		"{COPYDIR} ../assets/ %{cfg.targetdir}/assets/",
	}