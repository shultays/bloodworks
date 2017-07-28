DoubleExp.buffId = getGlobalUniqueId()
		
function DoubleExp.spawn(bonus, pos)

	local duration = 15.0 * player.data.bonusDurationMultiplier
	
	player.monsterExperienceMultiplier:addBuffWithId(DoubleExp.buffId, 2.0)
	player.monsterExperienceMultiplier:setBuffDuration(DoubleExp.buffId, duration)
	
	addBuffIcon("DoubleExp", DoubleExp.basePath .. "icon.png")
	
	playSound({path = "~/resources/sounds/double_exp.ogg", volume = 0.8})
	bonus:setActive(true)
end

function DoubleExp.onTick(bonus)
	if player.monsterExperienceMultiplier:hasBuffInfo(DoubleExp.buffId) == false then
		removeBuffIcon("DoubleExp")
		bonus:setActive(false)
	end
end
