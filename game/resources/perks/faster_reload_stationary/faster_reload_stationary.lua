
function FasterReloadStationary.init()
	FasterReloadStationary.id = getUniqueId()
	FasterReloadStationary.isFast = false
end


function FasterReloadStationary.onTick()
	local speed = player.moveSpeed
	if speed < 0.1 then
		if FasterReloadStationary.isFast == false then
			FasterReloadStationary.isFast = true
			player.reloadSpeedMultiplier:addBuffWithId(id, 4.0)
			player.reloadSpeedMultiplier:setBuffFadeInFadeOut(0.1, -1.0)
		end
	else
		if FasterReloadStationary.isFast then
			FasterReloadStationary.isFast = false
			player.reloadSpeedMultiplier:addBuffWithId(id, 1.0)
		end
	end
end