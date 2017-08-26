
function LongerBonuses.init()
    player.data.bonusDurationMultiplier = player.data.bonusDurationMultiplier * 1.3
end

function LongerBonuses.reset()
    player.data.bonusDurationMultiplier = 1.0
end
