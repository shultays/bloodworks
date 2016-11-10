
function HomingOrb.spawn(pos)
	--[[--
	local t = addGameObject("level", "FadeOutImage", {startTime = time, fadeOutStartTime = 2.0, fadeInDuration = 0.5})
	addTextureToGameObject(t, {}, "resources/bonuses/homing/bullet")
	--]]--
end


function HomingOrb.init(gameObjectId)
end

function HomingOrb.onTick(gameObjectId)
end