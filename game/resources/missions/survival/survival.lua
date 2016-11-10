
function Survival.init()
	mission.level = 0
end

function Survival.onTick()
	if getMonsterCount() == 0 then
		mission.level = mission.level + 1
		local t = addGameObject("level", "FadeOutImage", {startTime = time, fadeOutStartTime = 2.0, fadeInDuration = 0.5})
		addTextToGameObject(t, {alignment = "center", position = {0, -200}}, "Level : " .. mission.level)
		
		local v = vector(math.random() * 600 - 300, math.random() * 500 - 250)
		local ang = math.pi * 2.0 * math.random()
		local vShift = vector(math.cos(ang), math.sin(ang)) * 50.0
		for i = 1, 5 do
			local monster = addMonster("monster")
			monster.position.x = v.x
			monster.position.y = v.y
			v = v + vShift
		end
	
	end
end


function Survival.clear()


end

FadeOutImage = {}

function FadeOutImage.init(gameObjectId)
	local image = gameObjects[gameObjectId]
	if image.args.fadeOutStartTime == nil then
		image.args.fadeOutStartTime = 1.0
	end
	if image.args.fadeOutDuration == nil then
		image.args.fadeOutDuration = 1.0
	end
	if image.args.fadeInDuration == nil then
		image.args.fadeInDuration = 0.0
	end
end


function FadeOutImage.onTick(gameObjectId)
	local image = gameObjects[gameObjectId]
	local timeDiff = time - image.args.startTime
	local alpha
	if timeDiff < image.args.fadeInDuration then
		alpha = timeDiff / image.args.fadeInDuration
	else 
		alpha = 1.0 - (timeDiff - image.args.fadeOutStartTime) / image.args.fadeOutDuration
		if alpha < 0.0 then 
			image.toBeRemoved = true
			alpha = 0.0
		elseif alpha > 1.0 then
			alpha = 1.0
		end
	end
	local a =  math.floor(255 * alpha)
	local args = { color = (a<<24) | 0x00FFFFFF }
	updateRenderableParams(gameObjectId, -1, args);
end

function FadeOutImage.clear(gameObjectId)

end