core.cheats = {
	["Fastest"] = {
		["Fast Place"] = "fast_place",
		["Node Stroker"] = "nodestrok",
		["OptifineHUD"] = "optifine_hud",
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
 		["Show debug"] = "show_minimal_debug",
    },
    ["Graphics"] = {
    	["3D clouds"] = "enable_3d_clouds",
    	["Fog"] = "enable_fog",
	}
}

function core.register_cheat(cheatname, category, func)
	core.cheats[category] = core.cheats[category] or {}
	core.cheats[category][cheatname] = func
end
