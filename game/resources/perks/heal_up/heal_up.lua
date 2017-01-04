
function HealUp.init()
	player.data.healingUp = true
	player.data.lastHeal = time - 5.0
end


function HealUp.onTick()
	if player.data.healingUp and player.data.lastHeal - 0.1 < time then
		player.data.lastHeal = time
		if player.hitPoints < player.maxHitPoints then
			player:doHeal(3)
		else 
			player.data.healingUp = false
		end
	end
end
