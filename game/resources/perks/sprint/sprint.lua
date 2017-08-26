
function Sprint.init()
    player.data.sprintOldHp = player.hitPoints
    player.data.sprinting = false
end


function Sprint.onTick()
    if player.data.sprintOldHp > player.hitPoints and player.hitPoints < 40 then
        local id = player.maxSpeed:addBuff(1.5)
        player.maxSpeed:getBuffInfo(id):setBuffDuration(0.8)
    end

    player.data.sprintOldHp = player.hitPoints
end
