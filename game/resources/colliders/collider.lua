function Collider.onLoad(template)
	if Colliders == nil then
		Colliders = {}
	end
	Colliders[template.name] = template
end

function Collider.init(gameobject, params, template)
	local jsonTable = template.jsonTable
	local pos 
	
	while true do
		pos = getFreePosition(150.0)
		if pos:length() > 200.0 then
			break
		end
	end
	gameobject:setPosition(pos)
	
	local s = 0.3 + math.random() * 0.3
	gameobject:setScale(Vec2.new(s, s))
	gameobject:setRotation(math.random() * math.pi * 2.0)
	local json = template.jsonTable
	
	if json.image ~= nil then
		local texture = gameobject:addTexture(template.basePath .. json.image)
		texture:setWorldMatrix(Mat3.fromScale(json.size.x, json.size.y))
		texture:setColor(Vec4.fromColor(0xFF999999))
	elseif json.images ~= nil then
		local image = json.images[math.random(json.images.size) - 1]
		local texture = gameobject:addTexture(template.basePath .. image)
		texture:setWorldMatrix(Mat3.fromScale(json.size.x, json.size.y))
		texture:setColor(Vec4.fromColor(0xFF999999))
	end
	
	local colliders = json.colliders
	for i=0,colliders.size-1 do
		if colliders[i].type == "quad" then
			local shift = Vec2.new(0.0, 0.0)
			if colliders[i].shift ~= nil then
				shift = colliders[i].shift
			end
			local size = json.size
			if colliders[i].size ~= nil then
				size = colliders[i].size
			end
			local rotation = 0.0
			if colliders[i].rotation ~= nil then
				rotation = colliders[i].rotation
			end
			local radius = 0.0
			if colliders[i].radius ~= nil then
				radius = colliders[i].radius
			end
			gameobject:addRectCollider(shift, size, rotation, radius)
		elseif colliders[i].type == "capsule" then
			gameobject:addCapsuleCollider(colliders[i].pos1, colliders[i].pos2, colliders[i].radius)
		elseif colliders[i].type == "circle" then
			local shift = Vec2.new(0.0, 0.0)
			if colliders[i].shift ~= nil then
				shift = colliders[i].shift
			end
			gameobject:addCircleCollider(shift, colliders[i].radius)
		end
	end
end


