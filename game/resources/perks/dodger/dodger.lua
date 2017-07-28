
function Dodger.init()

end

function Dodger.onPlayerDamaged(damage, dir, params)
	if damage <= 0 then
		return damage
	end
	if params.doNotDodge ~= true and math.random() < 0.1 then
		return -1
	end
	return damage
end
