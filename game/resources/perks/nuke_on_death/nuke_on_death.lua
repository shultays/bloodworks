
function NukeOnDeath.init()

end

function NukeOnDeath.onTick()
	if player.data.nukeSpawning then
		if player.data.homingCount < 6 and player.data.lastHomingTime + 0.15 < time then
			player.data.lastHomingTime = time
			HomingOrb.spawn(player.position)
			player.data.homingCount = player.data.homingCount + 1
		end

	end	
end

function NukeOnDeath.onPlayerDied()
	addExplosion(player.position, 400.0, 300.0, 550, 850)
	playSound({path = "~/resources/sounds/explode.ogg"})
	player.data.lastHomingTime = time
	player.data.homingCount = 0
	player.data.nukeSpawning = true
end
