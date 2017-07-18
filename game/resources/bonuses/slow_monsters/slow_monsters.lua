SlowMonsters.buffId = getGlobalUniqueId()

function SlowMonsters.spawn(pos)
	player.globalMonsterSpeedMultiplier:addBuffWithId(SlowMonsters.buffId, 0.4)
	local buff = player.globalMonsterSpeedMultiplier:getBuffInfo(SlowMonsters.buffId)
	if buff:getCurrentBuffAmount() >= 0.99 then
		buff:setBuffFadeInFadeOut(0.4, 0.5)
	else
		buff:setBuffFadeInFadeOut(0.0, 0.5)
	end
	buff:setBuffDuration(8.0 * player.data.bonusDurationMultiplier)
	buff:restart()
	playSound({path = "resources/sounds/slow.ogg", volume = 0.6})
	
	addBuffIcon("SlowMonsters", "resources/bonuses/slow_monsters/icon.png")
	
	if SlowMonsters.data.gameObject == nil then
		SlowMonsters.data.gameObject = addGameObject("SlowMonsters")
	end
end

function SlowMonsters.onTick(gameObject)
	if player.globalMonsterSpeedMultiplier:hasBuffInfo(SlowMonsters.buffId) == false then
		removeBuffIcon("SlowMonsters")
		gameObject.toBeRemoved = true
		SlowMonsters.data.gameObject = nil
	end
end

function SlowMonsters.clear()
	SlowMonsters.buffId = nil
end