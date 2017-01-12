
AmmoManiac.buffId = getGlobalUniqueId()

function AmmoManiac.spawn(pos)
	local duration = 6.0 * player.data.bonusDurationMultiplier
	player.gunSpreadMultiplier:addBuffWithId(buffId, 4.5)
	player.clipCountMultiplier:addBuffWithId(buffId, 4.0)
	player.reloadSpeedMultiplier:addBuffWithId(buffId, 4.0)
	player.shootSpeedMultiplier:addBuffWithId(buffId, 0.25)
	player.damageMultiplier:addBuffWithId(buffId, 0.75)
	
	player.clipCountMultiplier:setBuffDuration(buffId, duration)
	player.gunSpreadMultiplier:setBuffDuration(buffId, duration)
	player.reloadSpeedMultiplier:setBuffDuration(buffId, duration)
	player.shootSpeedMultiplier:setBuffDuration(buffId, duration)
	player.damageMultiplier:setBuffDuration(buffId, duration)

end



