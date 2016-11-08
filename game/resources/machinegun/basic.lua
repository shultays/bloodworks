
function MachineGun.init()
	local gun = guns[gunId]
	gun.timeToNextShoot = 0.0
	gun.spreadAngle = 0.10
	gun.maxCrosshairRange = 350.0
end


function MachineGun.onTick()
	local gun = guns[gunId]
	
	if leftMouseDown then
		if gun.timeToNextShoot < time then
			gun.timeToNextShoot = time + 0.1
			local bulletId = addBullet();
		end
	end
end
