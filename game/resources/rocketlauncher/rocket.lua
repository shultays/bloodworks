
function RocketLauncher.init(gun)
	gun.data.timeToNextShoot = 0.0
	gun.spreadAngle = 0.0
	gun.crosshairDistance = 350.0
end


function RocketLauncher.onTick(gun)
	if gun.data.timeToNextShoot < time - 0.1 then
		gun.spreadAngle = gun.spreadAngle - dt * 0.15
	end
	
	if gun.spreadAngle < 0.0 then
		gun.spreadAngle = 0
	end
	if gun.leftMouseDown then
		if gun.data.timeToNextShoot < time then
			gun.data.timeToNextShoot = time + 0.8
			
			gun.spreadAngle = gun.spreadAngle + 0.025
			if gun.spreadAngle > 0.15 then
				gun.spreadAngle = 0.15
			end
			local bullet = gun:addBullet()
			bullet:addTrailParticle("RocketSmoke", Vec2.new(0.0, -8.0), {})
			bullet.moveSpeed = 250
		end
	end
end

function RocketLauncher.onBulletHit(bullet, monster)
	addExplosion(bullet.position)
	damageMonstersInRangeWithIgnoreData(bullet.position, 50, 40, 60, true, "ignoreBullet" .. bullet.index)
end
