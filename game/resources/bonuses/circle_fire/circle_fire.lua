
function CircleFire.spawn(x, y)
	local t = addGameObject("level", "CircleFire", {position = {x, y}})
end


function CircleFire.init(gameObjectId)
	local gameObject = gameObjects[gameObjectId]
	gameObject.startTime = time
	gameObject.lastShootTime = 0.0
	gameObject.lastShootIndex = 0
	
end

function CircleFire.onTick(gameObjectId)
	local gameObject = gameObjects[gameObjectId]
	
	if time - gameObject.startTime > 0.02 * gameObject.lastShootIndex
	then
		local shift = math.floor(gameObject.lastShootIndex/10)*0.25
		for i = 0, 3 do
			local angle = math.pi * 2.0 * (gameObject.lastShootIndex + i * 10 + shift) / 40
			addCustomBullet({
				damage = math.floor(math.random() * 30.0 + 30), 
				position = gameObject.args.position, 
				speed = {math.cos(angle) * 450, math.sin(angle) * 450}
			})
		end
		
		gameObject.lastShootIndex = gameObject.lastShootIndex + 1
		
		if gameObject.lastShootIndex == 40
		then
			gameObject.toBeRemoved = true
		end
	end
	
end