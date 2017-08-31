
function Experience.spawn(bonus, pos)
    player:gainExperience(math.floor(player.experienceForNextLevel / 5))
    playSound({path = Experience.basePath .. "bonus.ogg"})
end
