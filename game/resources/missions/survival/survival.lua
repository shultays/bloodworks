
function Survival.init()
	mission.level = 0
end

function Survival.onTick()
	if getMonsterCount() == 0 then
		local v = vector(math.random() * 600 - 300, math.random() * 500 - 250)
		local ang = math.pi * 2.0 * math.random()
		local vShift = vector(math.cos(ang), math.sin(ang)) * 50.0
		for i = 1, 5 do
			local index = addMonster("monster")
			local monster = monsters[index]
			monster.x = v.x
			monster.y = v.y
			v = v + vShift
			monster.scale = math.random() * 0.5 + 0.7
			local r = math.floor(255 * math.random())
			local g = math.floor(255 * math.random())
			local b = math.floor(255 * math.random())
			local ra = math.random()
			if ra < 0.333 then
				r = 0xff
			elseif ra < 0.666 then
				g = 0xff
			else
				b = 0xff
			end
			monster.color = 0xFF000000 | (r << 16) | (g << 8) | b
			resetMonster(index)
		end
	
	end
end


function Survival.clear()


end