
function Survival.init()
	mission.level = 0
end

function Survival.onTick()
	if getMonsterCount() == 0 then
		mission.level = mission.level + 1
		local gameObject = addGameObject("FadeOutImage")
		gameObject.data.startTime = time
		gameObject.data.fadeOutStartTime = 2.0
		gameObject.data.fadeInDuration = 0.5
		gameObject:setLevel(RenderableLevel.gui + 5)
		gameObject.data.renderable = gameObject:addText("Level : " .. mission.level, "resources/fontData.txt")
        gameObject.data.renderable.alignment = RenderableAlignment.bottom
        gameObject.data.renderable:update()
        gameObject:setPosition(Vec2.new(0, 50))
		
		local v = Vec2.new(math.random() * 600 - 300, math.random() * 500 - 250)
		local ang = math.pi * 2.0 * math.random()
		local vShift = Vec2.new(math.cos(ang), math.sin(ang)) * 50.0
		for i = 1, 6 do
			local monster = createMonster("monster")
			monster.position = v
			monster.moveAngle =  (player.position - v):getAngle()
			v = v + vShift
		end
	
	end
end


function Survival.clear()


end

FadeOutImage = {}

function FadeOutImage.init(gameObject)
	if gameObject.data.fadeOutStartTime == nil then
		gameObject.data.fadeOutStartTime = 1.0
	end
	if gameObject.data.fadeOutDuration == nil then
		gameObject.data.fadeOutDuration = 1.0
	end
	if gameObject.data.fadeInDuration == nil then
		gameObject.data.fadeInDuration = 0.0
	end
end


function FadeOutImage.onTick(gameObject)
	local timeDiff = time - gameObject.data.startTime
	local alpha
	if timeDiff < gameObject.data.fadeInDuration then
		alpha = timeDiff / gameObject.data.fadeInDuration
	else 
		alpha = 1.0 - (timeDiff - gameObject.data.fadeOutStartTime) / gameObject.data.fadeOutDuration
		if alpha < 0.0 then 
			gameObject.toBeRemoved = true
			alpha = 0.0
		elseif alpha > 1.0 then
			alpha = 1.0
		end
	end
	local a =  math.floor(255 * alpha)
    
	gameObject.data.renderable.color = (a * 2 ^ 24) + 0x00FFFFFF 
	gameObject.data.renderable:update()
end

function FadeOutImage.clear(gameObjectId)

end