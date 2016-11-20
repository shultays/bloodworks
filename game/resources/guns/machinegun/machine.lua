
function MachineGun.init(gun)
	gun.data.timeToNextShoot = 0.0
	gun.spreadAngle = 0.10
	gun.crosshairDistance = 350.0
end


function MachineGun.onTick(gun)
	if gun.leftMouseDown then
		if gun.data.timeToNextShoot < time then
			gun.data.timeToNextShoot = time + 0.1
			gun:addBullet();
		end
	end
end
