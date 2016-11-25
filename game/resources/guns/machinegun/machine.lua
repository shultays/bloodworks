
function MachineGun.init(gun)
	gun.spreadAngle = 0.10
	gun.crosshairDistance = 350.0
	ShootTimer.InitGun(gun, 0.1)
end


function MachineGun.onTick(gun)
	if gun.leftMouseDown then
		if ShootTimer.CheckGun(gun) then
			gun:addBullet();
		end
	end
end
