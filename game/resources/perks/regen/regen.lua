
function Regen.init(level)
    player.data.regenTimer = 1.25 - (level - 1) * 0.25
    player.data.lastRegenTick = time
end


function Regen.onTick()
    if player.data.lastRegenTick + player.data.regenTimer < time then
        player.data.lastRegenTick = time
        player:doHeal(1)        
    end
end
