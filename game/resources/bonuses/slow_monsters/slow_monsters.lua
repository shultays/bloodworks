
function SlowMonsters.spawn(pos)
	if SlowMonsters.buffId == nil then
		SlowMonsters.buffId = getUniqueId()
		player.globalMonsterSpeedMultiplier:addBuffWithId(SlowMonsters.buffId, 0.4)
		
	end
	local buff = player.globalMonsterSpeedMultiplier:getBuffInfo(SlowMonsters.buffId)
	buff.removeAfterEnds = false
	if buff:getCurrentBuffAmount() >= 0.99 then
		buff:setBuffFadeInFadeOut(0.4, 0.5)
	else
		buff:setBuffFadeInFadeOut(0.0, 0.5)
	end
	buff:setBuffDuration(8.0)
	buff:restart()
	playSound({path = "resources/sounds/slow.ogg", volume = 0.6})
end



function SlowMonsters.clear()
	SlowMonsters.buffId = nil
end

