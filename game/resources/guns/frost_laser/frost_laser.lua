
function FrostLaser.init(gun)
	gun.spreadAngle = 0.0
	gun.crosshairDistance = 550.0
	gun.data.maxRangeUniform = gun.laser:addUniformFloat("maxRange", 350.0)
	gun.data.timeUniform = gun.laser:addUniformFloat("time", 0.0)
	gun.data.buffId = getUniqueId()
	ShootTimer.initGun(gun, 0.1)
end


function FrostLaser.onTick(gun)
	gun.laser:setVisible(false)
	if gun.isTriggered and gun:hasAmmo() then
		gun.laser:setVisible(true)
		local range = 350.0
		local result = getClosestMonsterOnLine(player.gunPos, player.aimDir * range)
		if result.monster ~= nil then
			range = result.distance
		end
		
		gun.laser:setLength(range)
		gun.laser:addUniformFloat("time", time)
		if ShootTimer.checkGun(gun) then
			gun:consumeAmmo()
			if result.monster ~= nil then
				local args = {doNotStun = true}
				result.monster:doDamageWithArgs(gun:getRandomDamage(), player.aimDir, args)
				
				local buff
				local colorbuff
				if result.monster.moveSpeedMultiplier:hasBuffInfo(gun.data.buffId) == false then
					result.monster.moveSpeedMultiplier:addBuffWithId(gun.data.buffId, 1.0)
					buff = result.monster.moveSpeedMultiplier:getBuffInfo(gun.data.buffId)
					buff:setBuffDuration(0.8)
					buff:setBuffFadeInFadeOut(-1.0, 0.3)
					
					result.monster.colorMultiplier:addBuffWithId(gun.data.buffId, Vec4:new(1.0, 1.0, 1.0, 1.0))
					colorbuff = result.monster.colorMultiplier:getBuffInfo(gun.data.buffId)
					colorbuff:setBuffDuration(0.8)
					colorbuff:setBuffFadeInFadeOut(-1.0, 0.3)
					
				else
					buff = result.monster.moveSpeedMultiplier:getBuffInfo(gun.data.buffId)
					colorbuff = result.monster.colorMultiplier:getBuffInfo(gun.data.buffId)
				end
				local amount = buff:getCurrentBuffAmount()
				amount = amount * 0.80
				amount = math.max(amount, 0.2)
				
				buff:setBuffAmount(amount)
				buff:restart()
				
				amount = 1.0 - amount
				colorbuff:setBuffAmount(Vec4:new(1.0 - amount, 1.0 - amount * 0.8, 1.0 - amount * 0.2, 1.0))
				colorbuff:restart()
			end
		end
	end
end
