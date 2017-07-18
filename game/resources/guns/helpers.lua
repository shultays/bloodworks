ShootTimer = {}

function ShootTimer.initGun(gun, interval)
	gun.data.shootInterval = interval
	gun.data.timeToNextShoot = 0.0
end

function ShootTimer.IsGunReady(gun)
	return gun.data.timeToNextShoot < time and dt > 0.0
end

function ShootTimer.GunShot(gun)
	player:playShootAnimation()
	gun.data.timeToNextShoot = time + gun.data.shootInterval * player.shootSpeedMultiplier:getBuffedValue()
end


function ShootTimer.checkGun(gun)
	if gun.data.timeToNextShoot < time and dt > 0.0 then
		player:playShootAnimation()
		gun.data.timeToNextShoot = time + gun.data.shootInterval * player.shootSpeedMultiplier:getBuffedValue()
		return true
	end
	return false
end


SpreadHelper = {}

function SpreadHelper.initGun(gun)
	gun.data.timeToNextShoot = time
	gun.spreadAngle = 0.0
	gun.data.minSpread = 0.0
	gun.data.maxSpread = 0.15
	gun.data.spreadDecreaseStartTime = 0.1
	gun.data.spreadDecreaseSpeed = 0.15
	gun.data.spreadIncreasePerShoot = 0.025
end

function SpreadHelper.onTick(gun)
	local data = gun.data
	if data.timeToNextShoot < time - data.spreadDecreaseStartTime then
		gun.spreadAngle = gun.spreadAngle - dt * data.spreadDecreaseSpeed
	end
	
	if gun.spreadAngle < data.minSpread then
		gun.spreadAngle = data.minSpread
	end
end



function SpreadHelper.onShoot(gun)
	local data = gun.data
	gun.spreadAngle = gun.spreadAngle + data.spreadIncreasePerShoot
	if gun.spreadAngle > gun.data.maxSpread then
		gun.spreadAngle = gun.data.maxSpread
	end
end