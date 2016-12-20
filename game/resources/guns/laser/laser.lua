
function Laser.init(gun)
	gun.spreadAngle = 0.0
	gun.crosshairDistance = 550.0
	gun.data.maxRangeUniform = gun.laser:addUniformFloat("maxRange", 250.0)
	ShootTimer.initGun(gun, 0.1)
end


function Laser.onTick(gun)
	if gun.isTriggered then
		local range = 250.0
		local result = getClosestMonsterOnLine(player.gunPos, player.aimDir * range)
		if result.monster ~= nil then
			range = result.distance
		end
		
		gun.laser:setLength(range)
		if ShootTimer.checkGun(gun) then
			if result.monster ~= nil then
				local args = {doNotStun = true}
				result.monster:doDamageWithArgs(gun:getRandomDamage(), player.aimDir, args)
			end
		end
	end
end
