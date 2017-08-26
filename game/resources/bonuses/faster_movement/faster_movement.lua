FasterMovementBonus.buffId = getGlobalUniqueId()

function FasterMovementBonus.spawn(bonus, pos)
    player.maxSpeed:addBuffWithId(FasterMovementBonus.buffId, 1.7)
    local buff = player.maxSpeed:getBuffInfo(FasterMovementBonus.buffId)
    if buff:getCurrentBuffAmount() >= 0.99 then
        buff:setBuffFadeInFadeOut(0.4, 0.5)
    else
        buff:setBuffFadeInFadeOut(0.0, 0.5)
    end
    buff:setBuffDuration(12.0 * player.data.bonusDurationMultiplier)
    buff:restart()
    playSound({path = "~/resources/sounds/movement_bonus.ogg"})
    
    addBuffIcon("FasterMovementBonus", FasterMovementBonus.basePath .. "icon.png")
    
    bonus:setActive(true)
end

function FasterMovementBonus.onTick(bonus)
    if player.maxSpeed:hasBuffInfo(FasterMovementBonus.buffId) == false then
        removeBuffIcon("FasterMovementBonus")
        bonus:setActive(true)
    end
end
