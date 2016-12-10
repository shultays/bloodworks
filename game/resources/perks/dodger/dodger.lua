
function Dodger.init()

end

function Dodger.onPlayerDamaged(damage, params)
	if params.doNotDodge ~= true and math.random() < 0.1 then
		return -1
	end
	return damage
end
