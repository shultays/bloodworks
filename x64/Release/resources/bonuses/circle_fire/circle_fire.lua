
function CircleFire.spawn(pos)
	local t = addGameObject("level", "CircleFire", {position = Vec2.new(pos.x, pos.y)})
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
			
			local bullet = addCustomBullet()
			bullet.damage = math.floor(math.random() * 30.0 + 30)
			bullet.position = gameObject.args.position
			bullet.moveSpeed = 450.0
			bullet.moveAngle = angle
			
			bullet:addRenderableTexture("resources/bonuses/circle_fire/bullet.png")
			
		end
		
		gameObject.lastShootIndex = gameObject.lastShootIndex + 1
		
		if gameObject.lastShootIndex == 40
		then
			gameObject.toBeRemoved = true
		end
	end
	
end