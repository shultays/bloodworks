
function Pistol.init(gun)
	gun.spreadAngle = 0.0
	gun.crosshairDistance = 350.0
	ShootTimer.initGun(gun, 0.35)
	
	SpreadHelper.initGun(gun)
	gun.data.minSpread = 0.0
	gun.data.maxSpread = 0.10
	gun.data.spreadDecreaseSpeed = 0.25
	gun.data.spreadIncreasePerShoot = 0.02
end


function Pistol.onTick(gun)
	SpreadHelper.onTick(gun)
	if gun.isTriggered and gun:hasAmmo() then
		if ShootTimer.checkGun(gun) then
			gun:consumeAmmo()
			SpreadHelper.onShoot(gun)
			local bullet = gun:addBullet()
			local particle = bullet:addTrailParticle("BulletTrailParticle", Vec2.new(0.0, 14.0), 15.0, {})
			particle.args.initialScale = 2.0
			particle.args.fadeOutSpeed = 1.2
		end
	end
end
