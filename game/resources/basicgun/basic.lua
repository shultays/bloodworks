
function BasicGun.init()
	local gun = guns[gunId]
	gun.timeToNextShoot = 0.0
	gun.spreadAngle = 0.0
	gun.maxCrosshairRange = 350.0
end


function BasicGun.onTick()
	local gun = guns[gunId]
	gun.spreadAngle = gun.spreadAngle - dt * 0.15
	if gun.spreadAngle < 0.0 then
		gun.spreadAngle = 0
	end
	if leftMouseDown then
		if gun.timeToNextShoot < time then
			gun.timeToNextShoot = time + 0.25
			gun.spreadAngle = gun.spreadAngle + 0.25
			if gun.spreadAngle > 0.20 then
				gun.spreadAngle = 0.20
			end
			addBullet();
		end
	end
end