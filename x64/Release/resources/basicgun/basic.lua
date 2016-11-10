
function BasicGun.init(gunId)
	local gun = guns[gunId]
	gun.timeToNextShoot = 0.0
	gun.spreadAngle = 0.0
	gun.maxCrosshairRange = 350.0
end


function BasicGun.onTick(gunId)
	local gun = guns[gunId]
	
	if gun.timeToNextShoot < time - 0.1 then
		gun.spreadAngle = gun.spreadAngle - dt * 0.15
	end
	
	if gun.spreadAngle < 0.0 then
		gun.spreadAngle = 0
	end
	if leftMouseDown then
		if gun.timeToNextShoot < time then
			gun.timeToNextShoot = time + 0.25
			gun.spreadAngle = gun.spreadAngle + 0.025
			if gun.spreadAngle > 0.15 then
				gun.spreadAngle = 0.15
			end
			local bulletId = addBullet();
		end
	end
end
