
function Regen.init()
	player.data.regen = 1
	player.data.lastRegenTick = time
end


function Regen.onTick()
	if player.data.regen == 1 then
		if player.data.lastRegenTick + 1.0 < time then
			player.data.lastRegenTick = time
			player:doHeal(1)		
		end
	end
end
