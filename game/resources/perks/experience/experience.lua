
function ExperiencePerk.init(level)
	player:gainExperience(math.floor((player.experienceForNextLevel - player.experience) * 0.8))
end
