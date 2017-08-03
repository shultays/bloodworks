

function Collider.init(gameobject, params, template)
	local jsonTable = template.jsonTable
	gameobject:setPosition(Vec2.new(100.0, 0.0))
	gameobject:addCircleCollider(Vec2.new(0.0, 0.0), 20.0)
end


function Collider.onTick(gameobject)
	gameobject:setPosition(gameobject:getPosition() + Vec2.new(dt * -5.0, 0.0))
end