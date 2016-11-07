
function BasicGun.init()
	local gun = guns[gunId]
	gun.lastShootTime = 0.0
	gun.spreadAngle = 0.0
	gun.maxCrossHairRange = 0.0
end

function BasicGun.onTick()
end