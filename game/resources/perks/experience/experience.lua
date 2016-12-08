
function ExperiencePerk.init(level)
	player.gainExperience(math.floor(player.experienceForNextLevel * 0.5))
end
