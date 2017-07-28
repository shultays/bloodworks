
function Explosion.spawn(bonus, pos)
	addExplosion(pos, 200.0, 200.0, 250, 350)
	playSound({path = "~/resources/sounds/explode.ogg"})
end

