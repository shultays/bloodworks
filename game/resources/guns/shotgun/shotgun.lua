
function Shotgun.init(gun)
	gun.spreadAngle = 0.15
	gun.crosshairDistance = 350.0
	ShootTimer.initGun(gun, 1.05)
end


function Shotgun.onTick(gun)
	if gun.isTriggered and gun:hasAmmo() then
		if ShootTimer.checkGun(gun) then
			gun:consumeAmmo()
			for i = 1, 6 do
				local bullet = gun:addBullet()
				local particle = bullet:addTrailParticle("BulletTrailParticle", Vec2.new(0.0, 14.0), 15.0, {})
				particle.args.initialScale = 2.5
				particle.args.fadeOutSpeed = 1.6
				bullet.moveSpeed = bullet.moveSpeed * (math.random() * 0.3 + 0.7)
			end
		end
	end
end
