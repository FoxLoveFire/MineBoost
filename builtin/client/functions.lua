core.cheats = {
    ["MineBoost"] = {
		["Fast Place"] = "fast_place",
		["Node Stroker"] = "nodestrok",
		["OptifineHUD"] = "optifine_hud",
		["Build inside player"] = "enable_build_where_you_stand",
		["Particles"] = "particles",
		["Fullbright"] = "night",
		
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
	}
}

function core.register_cheat(cheatname, category, func)
	core.cheats[category] = core.cheats[category] or {}
	core.cheats[category][cheatname] = func
end

--Redone by FoxLoveFire.----

minetest.register_chatcommand("particles", {
    params =  "",
    description = "Toggle hit particles on or off.",
    func = function(params)
        local particles = minetest.settings:get_bool("particles")
        minetest.settings:set_bool("particles",  not particles)
    end
})

function register_hits()
	if not minetest.localplayer then
		return false
	end
    if minetest.localplayer:get_control().dig and not lmbpress then
        lmbpress = true
	local set = {
	    player_pos = core.camera:get_pos(),
	    player_dir = core.camera:get_look_dir(),
	    textures = "hit.png",
	}
        local raycast = minetest.raycast(set.player_pos, vector.add(set.player_pos, vector.multiply(set.player_dir, 4)), true, false)
        for pointed_thing in raycast do
            local new_pos = vector.add(pointed_thing.intersection_point, vector.new(-0.2, -0.4, -0.2))
            local other_pos = vector.add(pointed_thing.intersection_point, vector.new(0.2, 0.4, 0.2))
            if pointed_thing.type == "object" then
                minetest.add_particlespawner({
                    amount = tonumber(minetest.settings:get("particle_amount")),
                    time = 0.1,
                    minpos = new_pos, 
                    maxpos = other_pos,
                    minvel = {x = -1, y = -2, z = -1},
                    maxvel = {x = 1, y = 2, z = 1},
                    minacc = {x = -0.1, y = -0.1, z = -0.1},
                    maxacc = {x = 0.1, y = 0.1, z = 0.1},
                    minexptime = 0.5,
                    maxexptime = 0.7,
                    minsize = 1,
                    maxsize = 2,
                    collisiondetection = true,
                    collision_removal = false,
                    object_collision = false,
                    vertical = false,
                    texture = set.textures
                })
                break
            end
        end
    elseif not minetest.localplayer:get_control().dig then
        lmbpress = false
    end
end

minetest.register_globalstep(function(dtime)
    if minetest.settings:get_bool("particles") == true then
        register_hits()
    end
end)
