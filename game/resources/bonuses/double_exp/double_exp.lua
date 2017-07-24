DoubleExp.buffId = getGlobalUniqueId()
		
function DoubleExp.spawn(pos)

	local duration = 15.0 * player.data.bonusDurationMultiplier
	
	player.monsterExperienceMultiplier:addBuffWithId(DoubleExp.buffId, 2.0)
	player.monsterExperienceMultiplier:setBuffDuration(DoubleExp.buffId, duration)
	
	addBuffIcon("DoubleExp", DoubleExp.basePath .. "icon.png")
	
	playSound({path = "~/resources/sounds/double_exp.ogg", volume = 0.8})
	if DoubleExp.data.gameObject == nil then
		DoubleExp.data.gameObject = addGameObject("DoubleExp")
	end
end

function DoubleExp.onTick(gameObject)
	if player.monsterExperienceMultiplier:hasBuffInfo(DoubleExp.buffId) == false then
		removeBuffIcon("DoubleExp")
		gameObject.toBeRemoved = true
		DoubleExp.data.gameObject = nil
	end
end
