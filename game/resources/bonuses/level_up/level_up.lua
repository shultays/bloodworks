
function LevelUp.spawn(bonus, pos)
    player:gainExperience(math.floor(player.experienceForNextLevel))
    playSound({path = Experience.basePath .. "bonus.ogg"})
end

function LevelUp.variableBonusChance()
    if missionData.levelUpBonusChance ~= nil then
        return missionData.levelUpBonusChance
    end
    return 0.05
end