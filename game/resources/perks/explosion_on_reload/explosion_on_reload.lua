


function ExplosionOnReload.init()
end

function ExplosionOnReload.onReload(gun)
	if gun:hasAmmo() == false then
		addExplosion(player.position, 100.0, 200.0, 80, 140)
	end
end
