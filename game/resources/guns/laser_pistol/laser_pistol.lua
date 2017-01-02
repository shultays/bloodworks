
function LaserPistol.init(gun)
	gun.spreadAngle = 0.0
	gun.crosshairDistance = 350.0
	ShootTimer.initGun(gun, 0.40)
	SpreadHelper.initGun(gun)
end


function LaserPistol.onTick(gun)
	SpreadHelper.onTick(gun)
	if gun.isTriggered and gun:hasAmmo() then
		if ShootTimer.checkGun(gun) then
			gun:consumeAmmo()
			SpreadHelper.onShoot(gun)
			gun:addBullet()
		end
	end
end
