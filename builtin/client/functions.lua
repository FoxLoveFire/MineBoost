local scriptpath = core.get_builtin_path()
local clientpath = scriptpath .."client"..DIR_DELIM
dofile(clientpath .. "particles.lua")
dofile(clientpath .. "keystrokes.lua")

core.functions = {
    ["MineBoost"] = {
		["Fast Place"] = "fast_place",
		["Node Stroker"] = "nodestrok",
		["FunctionHUD"] = "function_hud",
		["Build inside player"] = "enable_build_where_you_stand",
		["Particles"] = "particles",
		["Keystrokes"] = "show_keys",
		
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
		["Sun off"] = "sun_off",
		["Moon off"] = "moon_off",
		["Stars off"] = "stars_off",
    }
}

function core.register_function(functionname, category, func)
	core.functions[category] = core.functions[category] or {}
	core.functions[category][functioname] = func
end


core.register_globalstep(function(dtime)
	if core.camera then
		local pos = core.camera:get_pos()
		local dir = core.camera:get_look_dir()
		local raycast = core.raycast(pos, vector.add(pos, vector.multiply(dir, 4)), true, false)
		local texture = "hit.png"
		local amount = 120
		if core.settings:get_bool("particles") then
			register_hits(pos, dir, texture, raycast, amount)
		end
	end
end)