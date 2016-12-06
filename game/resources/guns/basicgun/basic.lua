
function BasicGun.init(gun)
	gun.spreadAngle = 0.0
	gun.crosshairDistance = 350.0
	ShootTimer.InitGun(gun, 0.25)
end


function BasicGun.onTick(gun)
	if gun.data.timeToNextShoot < time - 0.1 then
		gun.spreadAngle = gun.spreadAngle - dt * 0.15
	end
	
	if gun.spreadAngle < 0.0 then
		gun.spreadAngle = 0
	end
	if gun.leftMouseDown then
		if ShootTimer.CheckGun(gun) then
			gun.spreadAngle = gun.spreadAngle + 0.025
			if gun.spreadAngle > 0.15 then
				gun.spreadAngle = 0.15
			end
			local bullet = gun:addBullet()
		end
	end
end
