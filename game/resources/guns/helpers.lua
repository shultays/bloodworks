ShootTimer = {}

function ShootTimer.InitGun(gun, interval)
	gun.data.shootInterval = interval
	gun.data.timeToNextShoot = 0.0
end

function ShootTimer.IsGunReady(gun)
	return gun.data.timeToNextShoot < time
end

function ShootTimer.GunShot(gun)
	gun.data.timeToNextShoot = time + gun.data.shootInterval
end


function ShootTimer.CheckGun(gun)
	if gun.data.timeToNextShoot < time then
		gun.data.timeToNextShoot = time + gun.data.shootInterval
		return true
	end
	return false
end
