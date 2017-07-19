FasterMovementBonus.buffId = getGlobalUniqueId()

function FasterMovementBonus.spawn(pos)
	player.maxSpeed:addBuffWithId(FasterMovementBonus.buffId, 1.7)
	local buff = player.maxSpeed:getBuffInfo(FasterMovementBonus.buffId)
	if buff:getCurrentBuffAmount() >= 0.99 then
		buff:setBuffFadeInFadeOut(0.4, 0.5)
	else
		buff:setBuffFadeInFadeOut(0.0, 0.5)
	end
	buff:setBuffDuration(12.0 * player.data.bonusDurationMultiplier)
	buff:restart()
	playSound({path = "resources/sounds/movement_bonus.ogg"})
	
	addBuffIcon("FasterMovementBonus", "resources/bonuses/faster_movement/icon.png")
	
	if FasterMovementBonus.data.gameObject == nil then
		FasterMovementBonus.data.gameObject = addGameObject("FasterMovementBonus")
	end
end

function FasterMovementBonus.onTick(gameObject)
	if player.maxSpeed:hasBuffInfo(FasterMovementBonus.buffId) == false then
		removeBuffIcon("FasterMovementBonus")
		gameObject.toBeRemoved = true
		FasterMovementBonus.data.gameObject = nil
	end
end

function FasterMovementBonus.clear()
	FasterMovementBonus.buffId = nil
end