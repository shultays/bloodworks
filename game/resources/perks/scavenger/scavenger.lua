
function Scavenger.init(level)
end


function Scavenger.onPlayerPickedGun(gun)
	player:doHeal(5)
end

function Scavenger.onPlayerPickedBonus(bonus, pos)
	player:doHeal(5)
end

