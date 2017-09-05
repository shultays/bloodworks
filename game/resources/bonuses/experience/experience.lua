
function Experience.spawn(bonus, pos)
    player:gainExperience(math.floor(player.experienceForNextLevel / 3))
    playSound({path = Experience.basePath .. "bonus.ogg"})
end
