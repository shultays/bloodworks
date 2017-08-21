
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
		if data.homingCount < 8 and data.lastHomingTime + 0.05 < time then
			data.lastHomingTime = time
			local bullet = HomingOrb.spawn(nil, data.position + Vec2.fromAngle(math.random() * math.pi * 2.0) * (math.random() * 5.0 + 5.0))
			if bullet ~= nil then
				bullet.damage = bullet.damage * 5
				bullet.moveSpeed = bullet.moveSpeed * (math.random() * 1.0 + 1.0)
				bullet.lifeTime = 1.0
			end
			data.homingCount = data.homingCount + 1
		end
	end	
end
