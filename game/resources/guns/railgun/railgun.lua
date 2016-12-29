
function Railgun.init(gun)
	gun.spreadAngle = 0.0
	gun.crosshairDistance = 350.0
	ShootTimer.initGun(gun, 0.85)
	SpreadHelper.initGun(gun)
	gun.data.maxSpread = 0.05
	gun.data.spreadDecreaseStartTime = 0.35
	gun.data.spreadDecreaseSpeed = 0.40
	gun.data.spreadIncreasePerShoot = 0.03
end


function Railgun.onTick(gun)
	SpreadHelper.onTick(gun)
	if gun.isTriggered then
		if ShootTimer.checkGun(gun) then
			SpreadHelper.onShoot(gun)
			local bullet = gun:addBullet()
			bullet.diesOnHit = false
			
			local particle = bullet:addTrailParticle("RailGunParticle", Vec2.new(0.0, 0.0), 7.0, {})
			
			particle = bullet:addTrailParticle("BulletTrailParticle", Vec2.new(0.0, 14.0), 15.0, {})
			particle.args.initialScale = 1.5
			particle.args.initialAlpha = 0.2
			particle.args.fadeOutSpeed = 0.3
			particle.args.color = Vec3.new(1.0, 1.0, 1.0)
		end
	end
end
