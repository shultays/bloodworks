
function CircleFire.spawn(bonus, pos)
    local t = addGameObject("CircleFire")
    t.data.position = Vec2.new(pos.x, pos.y)
end


function CircleFire.init(gameObject)
    gameObject.data.startTime = time
    gameObject.data.lastShootTime = 0.0
    gameObject.data.lastShootIndex = 0
    gameObject.data.i = 0
end

function CircleFire.onTick(gameObject)
    if time - gameObject.data.startTime > 0.04 * gameObject.data.lastShootIndex + 0.04 * 0.25 * gameObject.data.i
    then
        local shift = math.floor(gameObject.data.lastShootIndex/10)*0.25
        
        local angle = math.pi * 2.0 * (gameObject.data.lastShootIndex + gameObject.data.i * 10 + shift) / 40
        local bullet = addCustomBullet()
        bullet.damage = math.floor(math.random() * 30.0 + 30)
        bullet.position = gameObject.data.position
        bullet.moveSpeed = 450.0
        bullet.moveAngle = angle
        bullet:addRenderableTextureWithSize(CircleFire.basePath .. "bullet.png", Vec2.new(14.0, 14.0))
        
        gameObject.data.i = gameObject.data.i + 1
        if gameObject.data.i == 4 then
            if gameObject.data.lastShootIndex % 2 == 0 then
                playSound({path = "~/resources/sounds/laser_machinegun.ogg", position = gameObject.data.position})
            end
            gameObject.data.i = 0
            gameObject.data.lastShootIndex = gameObject.data.lastShootIndex + 1
            
        end
        
        if gameObject.data.lastShootIndex >= 40 * player.data.bonusDurationMultiplier - 0.1
        then
            gameObject.toBeRemoved = true
        end
    end
end