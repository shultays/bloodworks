
function IronSkin.init(level)
	IronSkin.level = 1 + (level - 1) * 2
end

function IronSkin.onPlayerDamaged(damage, params)
	if params.doNotReduce ~= true and damage > 0 then
		return damage - IronSkin.level
	end
	return damage
end
