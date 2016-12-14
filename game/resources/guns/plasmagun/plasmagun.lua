
function PlasmaGun.init(gun)
	gun.spreadAngle = 0.0
	gun.crosshairDistance = 350.0
	ShootTimer.InitGun(gun, 0.5)
end


function PlasmaGun.onTick(gun)
	if gun.isTriggered then
		if ShootTimer.CheckGun(gun) then
			local bullet = gun:addBullet()
			bullet:addTrailParticle("PlasmaTrailParticle", Vec2.new(0.0, 0.0), 3.0, {})
		end
	end
end

function PlasmaGun.onBulletHit(gun, bullet, monster)
	local ignoreId = "bullet" .. bullet.index
	local m = monster
	
	local oldGameObjectPos = nil
	while m ~= nill do
		m.data[ignoreId] = true
	
		local gameObject = addGameObject("FadeOutImage")
		gameObject.data.startTime = time
		gameObject.data.fadeOutStartTime = 0.0
		gameObject.data.fadeInDuration = 0.0
		gameObject.data.fadeOutDuration = 0.3
		gameObject:setLevel(RenderableLevel.monsters + 5)
		gameObject.data.renderable = gameObject:addTexture("resources/guns/plasmagun/bullet.png", "resources/default")
		gameObject.data.renderable.alignment = RenderableAlignment.world
		gameObject.data.renderable.textureSize = Vec2.new(6.0, 6.0)
		gameObject.data.renderable:update()
		if oldGameObjectPos ~= nil then
			gameObject:setPosition(m.position)
			local gameObject2 = addGameObject("FadeOutImage")
			gameObject2.data.startTime = time
			gameObject2.data.fadeOutStartTime = 0.0
			gameObject2.data.fadeInDuration = 0.0
			gameObject2.data.fadeOutDuration = 0.3
			gameObject2:setLevel(RenderableLevel.monsters + 4)
			gameObject2.data.renderable = gameObject2:addTexture("resources/guns/plasmagun/line.png", "resources/default")
			gameObject2.data.renderable.alignment = RenderableAlignment.world
			gameObject2.data.renderable.textureSize = Vec2.new(1.0, 2.0)
			gameObject2.data.renderable:update()
			gameObject2:setPosition( (m.position + oldGameObjectPos) * 0.5)
			gameObject2:setRotation( -(m.position - oldGameObjectPos):getAngle())
			gameObject2:setScale( Vec2:new((m.position - oldGameObjectPos):length() * 0.5 - 3, 1.0))
			
			m:doDamage(10 + math.floor(10 * math.random()), (m.position - oldGameObjectPos):normalized())
		
			if math.random() < 0.5 then
				oldGameObjectPos = m.position
			end
		else 
			gameObject:setPosition(bullet.position)
			oldGameObjectPos = bullet.position
		end
		
		m = getClosestMonsterInRangeWithIgnoreData(bullet.position, 80.0, ignoreId)
	end
	
end
