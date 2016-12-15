
function LaserMachineGun.init(gun)
	gun.crosshairDistance = 350.0
	
	ShootTimer.initGun(gun, 0.1)
	SpreadHelper.initGun(gun)
	gun.data.minSpread = 0.05
	gun.data.maxSpread = 0.15
	gun.data.spreadIncreasePerShoot = 0.01
end


function LaserMachineGun.onTick(gun)
	SpreadHelper.onTick(gun)
	if gun.isTriggered then
		if ShootTimer.checkGun(gun) then
			SpreadHelper.onShoot(gun)
			gun:addBullet();
		end
	end
end
