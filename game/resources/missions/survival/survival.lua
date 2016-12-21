
function Survival.init()

	local monsters = {"Alien", "Alien2", "Spider"}
	monsters = {"Alien2"}
	for i = 1, 50 do
		local pos = getRandomMonsterSpawnPos()
		local selected = monsters[ math.random( #monsters ) ] 
		local monster = addMonster(selected)
		
		monster.position = pos
		monster.moveAngle =  math.random() * math.pi * 2.0
	end
end

function Survival.onTick()
end


function Survival.clear()


end
