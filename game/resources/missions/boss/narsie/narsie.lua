
function NarSie.init(monster, min)
    data = monster.data

    monster.moveSpeed = 0.0
    monster.moveAngle = 0.0
    monster.spriteAngle = math.pi * -0.5
    
    monster:playAnimation("stand")
    
    monster:setDrawLevel(RenderableLevel.player + 20)

    monster.knockbackResistance:addBuff(0.01)
    
    monster.canGetOneShooted = false
    monster.hasCollision = false
    
    data.inhaleTimer = 0.0
    data.currentAttack = nil
    data.currentAttackTimer = 6.0
    
    data.diff = 0.0
    
    NarSie.resetTarget(monster)
    data.attacks = { NarSie.ringBullets }
    
    data.timeToNextHit = 0.0
    
    data.shootTimer = 0.
    
    data.startTime = time
    data.firstTick = true
    monster:setVisible(false)
    
    addCustomOnHit(monster, NarSie.onCustomHit)
end

function NarSie.spawnChanceInMission(missionData, min)
    return 0.0
end

function NarSie.onCustomHit(monster, damage, dir, args)
    if monster.data.spawning then
        return 0
    end
    return damage
end

function NarSie.onTick(monster)
    data = monster.data
    
    if data.firstTick == true then
        data.firstTick = false
        
        data.blackHole = addPostProcess("resources/post_process/blackhole.ps")
        data.blackHole:setEnabled(true)
        
        playSound({path = BlackHole.basePath .. "black_hole.ogg", position = monster.position, volume = 1.5})
        data.spawning = true
    end
    
    if data.spawning then
        local t = (time - data.startTime) * 1.5
    
        local v = convertToScreenPosition(monster.position)
        data.blackHole:addUniformVec2("uRelBlackHolePos", v)
        
        local size = 0.8
        if t < 1.0 then
            data.blackHole:setShaderWeight(t * size)
        elseif t < 8.0 then
            data.blackHole:setShaderWeight(size)
        elseif t < 9.0 then
            monster:setVisible(true)
            data.blackHole:setShaderWeight((9.0 - t) * size)
            local scale = (t - 8.0) * 2.0 + 0.2
            if scale > 1.0 then
                scale = 1.0
            end
            monster:setScale(scale)
        else
            monster:setScale(1.0)
            data.blackHole:setEnabled(false)
            data.spawning = false
        end
        return
    end
    
    
            
    local d = data.target:distanceSquared(monster.position)
    addCircle(data.target, 20.0)
    if d < 10000 then
        NarSie.resetTarget(monster)
    else
        local toTarget = data.target - monster.position
        local angle = toTarget:getAngle()
        monster.moveAngle = approachAngle(monster.moveAngle, angle, 3.0 * dt)
    end
    
    
    local diffToPlayer = player.position - monster.position
    local distanceToPlayer = diffToPlayer:length()
    
    if distanceToPlayer < 90 and data.timeToNextHit < time and player.isDead == false then
        local damage = player:doDamage(math.floor(15 + math.random() * 5), angleToPlayer)
        data.timeToNextHit = time + 1.0
        if damage > 0 then
            playSound({path = "~/resources/sounds/melee_woosh.ogg", volume = 0.3})
            if player.data.noSlowdownOnHit == nil then
                player.maxSpeed:addBuffWithId(MonsterMeleeHelper.slowBuffId, 0.5)
                player.maxSpeed:setBuffDuration(MonsterMeleeHelper.slowBuffId, 0.1)
                player.maxSpeed:setBuffFadeInFadeOut(MonsterMeleeHelper.slowBuffId, 0.1, 0.1)
            end
        end
        
        local t = MeleeHitImage.build(monster)
        
        t.data.renderable:setWorldMatrix(Mat3.fromScale(30.0, 30.0))
        t:setPosition(player.position)
        t:setRotation(-math.pi * 0.5 - diffToPlayer:getAngle())
    end
    
        
    data.inhaleTimer = data.inhaleTimer - dt
    
    if data.inhaleTimer < 0.0 then
        data.inhaleTimer = 1.0 + math.random() * 1.0
        monster:playAnimation("inhale")
    end
    
    if data.currentAttack == nil then
        
        monster.moveSpeed = 20.0
    
        data.currentAttackTimer = data.currentAttackTimer - dt
        if data.currentAttackTimer < 0.0 then
            data.currentAttack = data.attacks[ math.random( #data.attacks ) ]
            data.currentAttack(monster, true)
            data.currentAttackTimer = 5.0
        end
        
        data.shootTimer = data.shootTimer - dt
        
        if data.shootTimer < 0.0 then
            data.shootTimer = data.shootTimer + lerp(1.2, 0.6, clamp(min /10.0))
            
            local bullet = addCustomBullet()
            bullet.damage = math.floor(math.random(20, 30))
            bullet.moveSpeed =  300.0 + min * 30.0
            
            
            local s = 1.0
            if math.random() < 0.5 then
                s = -1.0
            end
            
            bullet.position = monster.position + Vec2.new( s * 50, -30.0)
            
            local d = player.position:distance(bullet.position) / bullet.moveSpeed
            if d > 0.5 then
                --d = 0.5
            end 
            
            local futurePos = player.position + player.moveVelocity * d
            
            bullet.moveAngle = (futurePos - monster.position):getAngle()
            
            bullet.monsterBullet = true
            bullet.radius = 6.0
            bullet:addRenderableTextureWithSize(monster.monsterTemplate.basePath .. "bullet.png", Vec2.new(4.0, 4.0))
            
            particle = bullet:addTrailParticle("FlameParticle", Vec2.new(0.0, 0.0), 4.0, {initialScale = 150.0})
            
            particle = bullet:addTrailParticle("RocketSmokeParticle", Vec2.new(0.0, 0.0), 2.0, {})
            particle:addLinearRandom("fadeOutSpeed", 1.0, 1.4)
            
            
            bullet.lifeTime = 9.0
            
        end
        
    else 
        monster.moveSpeed = 0.0
        data.currentAttack(monster, false)
    end
    
    if isKeyPressed(keys.F) then
        loadScript("resources/missions/boss/narsie/narsie.lua")
    end
end

function NarSie.resetTarget(monster)
    local t = math.random()
    
    monster.data.target = Vec2.randDir() * 300.0 * t * t
end


function NarSie.ringBullets(monster, init)
    local data = monster.data
    if init then
        data.timeToShoot = 0.0
        data.shift = 0.0
        data.count = 0
    end
    
    data.timeToShoot = data.timeToShoot - dt
    
    if data.timeToShoot < 0.0 then
        data.timeToShoot = data.timeToShoot + lerp(0.6, 0.3, clamp(min * 0.1))
        
        local count = math.floor(10 + min * 1 )
        for i = 0, count - 1 do
            local bullet = addCustomBullet()
            bullet.damage = math.floor(math.random(20, 30))
            bullet.moveSpeed =  100.0 + min * 2.0
            bullet.moveAngle = data.shift + i * math.pi * 2.0 / count
            bullet.position = monster.position + Vec2.fromAngle(bullet.moveAngle) * 40.0
            bullet.monsterBullet = true
            bullet.radius = 2.0
            bullet:addRenderableTextureWithSize("~/resources/monsters/bullet.png", Vec2.new(18.0, 18.0))
            bullet.lifeTime = 9.0
        end
        
        data.shift = data.shift + 0.4 * math.pi * 2.0 / count
        data.count = data.count + 1
        if data.count > lerp(10, 20, clamp(min * 0.1)) then
            data.currentAttack = nil
        end
    end
end
