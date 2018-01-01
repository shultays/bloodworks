
function BananaBomb.init(gun)
    gun.crosshairDistance = 550.0
    gun.data.shooting = -1.0
end

function BananaBomb.onTick(gun)
    local data = gun.data
    if gun.isTriggered and gun:hasAmmo() and data.shooting < 0.0 then
        gun:consumeAmmo()
        data.shooting = 0.5
        
        local gameObject = addGameObject("BananaBombObject", { i = 0, dir = player.aimDir } )
        gameObject.data.throwDuration = gameObject.data.throwDuration * 0.8
        gameObject:setPosition(player.position)
        playSound({path = BananaBomb.basePath .. "swish.ogg"})
        
    end
    
    if data.shooting >= 0.0 then
        data.shooting = data.shooting - dt
        
    end
end

function BananaBomb.spawnChance(gun)
    return ultimateSpawnChance()
end

BananaBombObject = {}


function BananaBombObject.init(gameObject, args)
    local renderable = gameObject:addAnimation("BananaAnimation")
    renderable:playAnimation("rotate", math.random() * 0.8)
    gameObject.data.renderable = renderable
    
    gameObject.data.startTime = time
    gameObject.data.i = args.i
    gameObject.data.throwDuration = math.random() * 0.8 + 0.8
    if args.dir ~= nil then
        gameObject.data.speed = args.dir
        gameObject.data.throwDuration = gameObject.data.throwDuration * 0.6
    else
        gameObject.data.speed = Vec2.fromAngle(math.random() * math.pi * 2.0)
    end
    gameObject.data.speed = gameObject.data.speed * (math.random() * 60.0 + 100.0 - args.i * 15)
    gameObject:setRotation(math.random() * math.pi * 2.0)
    BananaBombObject.onTick(gameObject)
end

function BananaBombObject.onTick(gameObject)
    local data = gameObject.data
    local t = time - data.startTime
    local tnormal = t / data.throwDuration
    
    if tnormal > 1.0 then
        gameObject.toBeRemoved = true
        
        addExplosion(gameObject:getPosition(), 80.0, 100.0, 70, 150, 0.0, false, nil, true)
        if BananaBombObject.lastExplosionSound == nil or time - BananaBombObject.lastExplosionSound > 0.1 then
            playSound({path = "~/resources/sounds/explode.ogg"})
            BananaBombObject.lastExplosionSound = time
        end
        
        if data.i < 3 then
            local r = math.random(2) + 3 - data.i
            for i = 1,r do 
                local child = addGameObject("BananaBombObject", { i = data.i + 1 } )
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
