
function BananaBomb.init(gun)
    gun.crosshairDistance = 550.0
    gun.data.shooting = -1.0
end

function BananaBomb.onTick(gun)
    local data = gun.data
    if gun.isTriggered and gun:hasAmmo() and data.shooting < 0.0 then
        gun:consumeAmmo()
        data.shooting = 0.5
        
        local gameObject = addGameObject("BananaBombObject")
        gameObject.data.speed = player.aimDir
        gameObject.data.speed = gameObject.data.speed * (math.random() * 30.0 + 100.0)
        gameObject.data.throwDuration = gameObject.data.throwDuration * 0.8
        gameObject:setPosition(player.position)
    end
    
    if data.shooting >= 0.0 then
        data.shooting = data.shooting - dt
        
    end
end

BananaBombObject = {}


function BananaBombObject.init(gameObject)
    local renderable = gameObject:addAnimation("BananaAnimation")
    renderable:playAnimation("rotate", math.random() * 0.8)
    gameObject.data.renderable = renderable
    
    gameObject.data.startTime = time
    gameObject.data.i = 0
    gameObject.data.speed = Vec2.fromAngle(math.random() * math.pi * 2.0) * (math.random() * 30.0 + 100.0)
    gameObject.data.throwDuration = math.random() * 0.8 + 0.8
    gameObject:setRotation(math.random() * math.pi * 2.0)
    BananaBombObject.onTick(gameObject)
end

function BananaBombObject.onTick(gameObject)
    local data = gameObject.data
    local t = time - data.startTime
    local tnormal = t / data.throwDuration
    
    if tnormal > 1.0 then
        gameObject.toBeRemoved = true
        
        addExplosion(gameObject:getPosition(), 50.0, 100.0, 70, 150)
        if BananaBombObject.lastExplosionSound == nil or time - BananaBombObject.lastExplosionSound > 0.1 then
            playSound({path = "~/resources/sounds/explode.ogg"})
            BananaBombObject.lastExplosionSound = time
        end
        
        if data.i < 3 then
            local r = math.random(2) + 4 - data.i
            for i = 1,r do 
                
                local child = addGameObject("BananaBombObject")
                child.data.i = data.i + 1
                child:setRotation(math.random() * math.pi * 2.0)
                child:setPosition(gameObject:getPosition())
            end
        end
    end
    
    local m = data.i / 3.0
    
    gameObject:setRotation(gameObject:getRotation() + dt * 5.0)
    gameObject:setPosition(gameObject:getPosition() + data.speed * dt * (1.0 - m * 0.5))
    
    local s = (0.6 + math.abs(math.sin(tnormal * math.pi)) * 0.4)
    gameObject:setScale(Vec2:new(s, s))
    
    s = 0.6 + tnormal * 0.3
    data.renderable:setColor(Vec4:new(s, s, s, 1.0))
end