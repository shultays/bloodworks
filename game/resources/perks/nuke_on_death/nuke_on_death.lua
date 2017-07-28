
function NukeOnDeath.init()
end

function NukeOnDeath.onPlayerDied()
	local object = addGameObject("NukeOnDeathObject")
	addExplosion(player.position, 400.0, 300.0, 550, 850)
	playSound({path = "~/resources/sounds/explode.ogg"})
	object.data.lastHomingTime = time
	object.data.homingCount = 0
	object.data.nukeSpawning = true
	object.data.position = player.position
end

NukeOnDeathObject = {}

function NukeOnDeathObject.onTick(gameObject)
	local data = gameObject.data
	if data.nukeSpawning then
		if data.homingCount < 8 and data.lastHomingTime + 0.15 < time then
			data.lastHomingTime = time
			HomingOrb.spawn(data.position)
			data.homingCount = data.homingCount + 1
		end
	end	
end
