SlowMonsters.buffId = getGlobalUniqueId()

function SlowMonsters.spawn(bonus, pos)
    player.globalMonsterSpeedMultiplier:addBuffWithId(SlowMonsters.buffId, 0.4)
    local buff = player.globalMonsterSpeedMultiplier:getBuffInfo(SlowMonsters.buffId)
    if buff:getCurrentBuffAmount() >= 0.99 then
        buff:setBuffFadeInFadeOut(0.4, 0.5)
    else
        buff:setBuffFadeInFadeOut(0.0, 0.5)
    end
    buff:setBuffDuration(8.0 * player.data.bonusDurationMultiplier)
    buff:restart()
    playSound({path = SlowMonsters.basePath .. "slow.ogg", volume = 0.6})

    addBuffIcon("SlowMonsters", SlowMonsters.basePath .. "icon.png")
    
    bonus:setActive(true)
end

function SlowMonsters.onTick(bonus)
    if player.globalMonsterSpeedMultiplier:hasBuffInfo(SlowMonsters.buffId) == false then
        removeBuffIcon("SlowMonsters")
        bonus:setActive(false)
    end
end
