
function Rifle.init(gun)
	gun.spreadAngle = 0.0
	gun.crosshairDistance = 350.0
	ShootTimer.InitGun(gun, 0.55)
end


function Rifle.onTick(gun)
	if gun.data.timeToNextShoot < time - 0.1 then
		gun.spreadAngle = gun.spreadAngle - dt * 0.20
	end
	
	if gun.spreadAngle < 0.0 then
		gun.spreadAngle = 0
	end
	if gun.leftMouseDown then
		if ShootTimer.CheckGun(gun) then
			gun.spreadAngle = gun.spreadAngle + 0.020
			if gun.spreadAngle > 0.15 then
				gun.spreadAngle = 0.15
			end
			local bullet = gun:addBullet()
			local particle = bullet:addTrailParticle("BulletTrailParticle", Vec2.new(0.0, 0.0), 15.0, {})
			particle.args.initialScale = 3.0
			particle.args.fadeOutSpeed = 0.8
		end
	end
end
