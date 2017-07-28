
AmmoManiac.buffId = getGlobalUniqueId()

function AmmoManiac.spawn(bonus, pos)
	local duration = 6.0 * player.data.bonusDurationMultiplier
	player.gunSpreadMultiplier:addBuffWithId(AmmoManiac.buffId, 4.5)
	player.clipCountMultiplier:addBuffWithId(AmmoManiac.buffId, 4.0)
	player.reloadSpeedMultiplier:addBuffWithId(AmmoManiac.buffId, 4.0)
	player.shootSpeedMultiplier:addBuffWithId(AmmoManiac.buffId, 0.25)
	player.damageMultiplier:addBuffWithId(AmmoManiac.buffId, 0.75)
	
	player.clipCountMultiplier:setBuffDuration(AmmoManiac.buffId, duration)
	player.gunSpreadMultiplier:setBuffDuration(AmmoManiac.buffId, duration)
	player.reloadSpeedMultiplier:setBuffDuration(AmmoManiac.buffId, duration)
	player.shootSpeedMultiplier:setBuffDuration(AmmoManiac.buffId, duration)
	player.damageMultiplier:setBuffDuration(AmmoManiac.buffId, duration)

	addBuffIcon("AmmoManiac", AmmoManiac.basePath .. "icon.png")
	
	bonus:setActive(true)
end

function AmmoManiac.onTick(bonus)
	if player.clipCountMultiplier:hasBuffInfo(AmmoManiac.buffId) == false then
		removeBuffIcon("AmmoManiac")
		bonus:setActive(false)
	end
end

