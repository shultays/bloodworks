
function Sprint.init()
	player.data.sprintOldHp = player.hitPoints
	player.data.sprinting = false
end


function Sprint.onTick()
	if player.data.sprintOldHp > player.hitPoints then
		if player.data.sprinting == false then
			player.data.sprinting = true
		player.moveSpeedMult = player.moveSpeedMult * 2.0
		end
		player.data.sprintStartTime = time
	end

	player.data.sprintOldHp = player.hitPoints
	if player.data.sprinting and player.data.sprintStartTime + 1.0 < time then
		player.data.sprinting = false
		player.moveSpeedMult = player.moveSpeedMult / 2.0
	end
end
