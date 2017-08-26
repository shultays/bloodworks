
function FasterReload.init(level)
    local amount = 1.0 + level * 0.1
    if level == 1 then
        FasterReload.id = player.reloadSpeedMultiplier:addBuff(amount)
    else
        player.reloadSpeedMultiplier:setBuffAmount(FasterReload.id, amount)
    end
end

