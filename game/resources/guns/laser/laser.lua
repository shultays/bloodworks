
function Laser.init(gun)
	gun.spreadAngle = 0.0
	gun.crosshairDistance = 350.0
	ShootTimer.initGun(gun, 0.25)
	SpreadHelper.initGun(gun)
end


function Laser.onTick(gun)
	SpreadHelper.onTick(gun)
	if gun.isTriggered then
		if ShootTimer.checkGun(gun) then
			SpreadHelper.onShoot(gun)
			gun:addBullet()
		end
	end
end
