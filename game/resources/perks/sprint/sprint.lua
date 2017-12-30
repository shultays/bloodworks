
Sprint.buffId = getGlobalUniqueId()

function Sprint.init()
    player.data.sprintOldHp = player.hitPoints
    player.data.sprinting = false
end


function Sprint.onTick()
    if player.data.sprintOldHp > player.hitPoints then
        local id = player.maxSpeed:addBuffWithId(Sprint.buffId, 2.0)
        player.maxSpeed:getBuffInfo(id):setBuffDuration(0.7)
        player.maxSpeed:getBuffInfo(id):setBuffFadeInFadeOut(0.0, 0.35)
    end

    player.data.sprintOldHp = player.hitPoints
end
