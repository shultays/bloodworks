
function LaserMachineGun.init(gun)
	gun.spreadAngle = 0.10
	gun.crosshairDistance = 350.0
	ShootTimer.InitGun(gun, 0.1)
end


function LaserMachineGun.onTick(gun)
	if gun.isTriggered then
		if ShootTimer.CheckGun(gun) then
			gun:addBullet();
		end
	end
end
