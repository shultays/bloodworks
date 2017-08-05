
function RocketBarrage.init(gun)
	gun.spreadAngle = 0.0
	gun.crosshairDistance = 350.0
	gun.data.shooting = -1.0
end


function RocketBarrage.onTick(gun)
	local data = gun.data
	if gun.isTriggered and gun:hasAmmo() and data.shooting < 0.0 then
		gun:consumeAmmo()
		data.shooting = 2.0
		data.shootDt = 0.0
		local buff = player.maxRotateSpeed:addBuff(0.06)
		player.maxRotateSpeed:setBuffDuration(buff, data.shooting + 0.3)
		player.maxRotateSpeed:setBuffFadeInFadeOut(buff, 0.05, 0.3)
		local buff = player.maxSpeed:addBuff(0.3)
		player.maxSpeed:setBuffDuration(buff, data.shooting)
		player.maxSpeed:setBuffFadeInFadeOut(buff, 0.1, 0.1)
	end
	
	if data.shooting > 0.0 then
		data.shooting = data.shooting - dt
		data.shootDt = data.shootDt + dt * 180
		while data.shootDt > 0.0 do
			data.shootDt = data.shootDt - 1
			local bullet = gun:addBullet()
			bullet.damage = math.floor(110 + math.random() * 40)
			bullet.moveSpeed = 400
			bullet.lifeTime = 0.2 + math.random() * 0.2
			bullet.moveSpeed = 750 + math.random() * 200.0
			local spread = 0.3
			local maxSpeed = 1.8
			bullet.data.rotateSpeed = maxSpeed * (math.random() * 0.5 + 0.5)
			if math.random() < 0.5 then
				bullet.data.rotateSpeed = -bullet.data.rotateSpeed
			end
			bullet.moveAngle = bullet.moveAngle + spread * (math.random() * 2.0 - 1.0)
			
			--if DEBUG ~= true then
				local particle = bullet:addTrailParticle("RocketSmokeParticle", Vec2.new(0.0, 0.0), 4.0, {})
				particle:addLinearRandom("fadeOutSpeed", 1.0, 1.4)
			--end
			local c = math.random() * 0.3 + 0.7
		end
	end
end

function RocketBarrage.onBulletHit(gun, bullet, monster)
	if monster ~= nil then
		addExplosion(bullet.position, 80.0, 180.0, 20, 30)
	end
end

function RocketBarrage.onBulletTick(gun, bullet)
	bullet.moveAngle = bullet.moveAngle + bullet.data.rotateSpeed * dt
	local a = 1.0 - (time - bullet.startTime - bullet.lifeTime + 0.1) / 0.1
	bullet:setColor(Vec4:new(1.0, 1.0, 1.0, a))
end
