ShootTimer = {}

function ShootTimer.InitGun(gun, interval)
	gun.data.shootInterval = interval
	gun.data.timeToNextShoot = 0.0
end

function ShootTimer.IsGunReady(gun)
	return gun.data.timeToNextShoot < time and dt > 0.0
end

function ShootTimer.GunShot(gun)
	gun.data.timeToNextShoot = time + gun.data.shootInterval * player.shootSpeedMult
end


function ShootTimer.CheckGun(gun)
	if gun.data.timeToNextShoot < time and dt > 0.0 then
		gun.data.timeToNextShoot = time + gun.data.shootInterval * player.shootSpeedMult
		return true
	end
	return false
end
