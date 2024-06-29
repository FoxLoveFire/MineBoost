local scriptpath = core.get_builtin_path()
local clientpath = scriptpath .."client"..DIR_DELIM
dofile(clientpath .. "particle.lua")

core.functions = {
    ["MineBoost"] = {
		["Fast Place"] = "fast_place",
		["Node Stroker"] = "nodestrok",
		["FunctionHUD"] = "function_hud",
		["Build inside player"] = "enable_build_where_you_stand",
		["Particles"] = "particles",
		
 	},
    ["Settings"] = {
 		["Autojump"] = "autojump",
 		["Autoforward"] = "continuous_forward",
 		["Pitch move"] = "pitch_move",
 		["Fly"] = "free_move",
 		["Noclip"] = "noclip",
 		["Fast Move"] = "fast_move",
    },
    
    ["Graphics"] = {
    	["3D clouds"] = "enable_3d_clouds",
    	["Fog"] = "enable_fog",
	["Fullbright"] = "night",
	["Shoow coords"] = "show_coords",
    }
}

function core.register_function(functionname, category, func)
	core.functions[category] = core.functions[category] or {}
	core.functions[category][functioname] = func
end


