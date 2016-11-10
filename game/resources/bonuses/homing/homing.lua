
function HomingOrb.spawn(pos)
	local gameObject = addGameObject("HomingOrb")
	local texture = gameObject:addTexture("resources/bonuses/homing/bullet.png", "resources/default")
	texture.textureSize = Vec2.new(50, 50)
	texture:update()
end


function HomingOrb.init(gameObjectId)
end

function HomingOrb.onTick(gameObjectId)
end