
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
    data.currentAttackTimer = 8.0
    
    data.diff = 0.0
    
    NarSie.resetTarget(monster)
    data.attacks = { NarSie.ringBullets, NarSie.arcBullets, NarSie.fireballs, NarSie.spawnSpiders, NarSie.explosions, NarSie.explosionsAroundPlayer }
    
    --data.attacks = { NarSie.fireballs2 }
    --data.currentAttackTimer = 0.0
    
    data.timeToNextHit = 0.0
    data.defaultShootTimer = 2.0
    
    data.startTime = time
    data.firstTick = true
    data.pulling = false
    data.pullTimer = 0.0
    data.damageAmount = 0.0
    monster.removeOnDead = false
    monster:setVisible(false)
    
    addCustomOnHit(monster, NarSie.onCustomHit)
end

function NarSie.spawnChanceInMission(missionData, min)
    return 0.0
end

function NarSie.onCustomHit(monster, damage, dir, args)
    local data = monster.data
    data.damageAmount = data.damageAmount + damage
    local maxDamage = 500
    if data.damageAmount > maxDamage then
        damage = damage - (data.damageAmount - maxDamage)
        data.damageAmount = maxDamage
    end
    if data.spawning then
        return 0
    end
    addScore(math.floor(damage / 10))
    return damage
end

function NarSie.onKilled(monster)
    NarSieFight.onNarSieKilled(monster)
    monster.data.deathTimer = time
    
    playSound({path = missionPath .. "narsie/laugh.ogg", position = monster.position, volume = 1.1})
    
    if hasAchievement( "ACH_NAR_SIE" ) == false and player.isDead == false then
        addAchievement( "ACH_NAR_SIE" ) 
    end
end

function NarSie.onTick(monster)
    data = monster.data
    data.damageAmount = data.damageAmount - dt * 600
    if data.damageAmount < 0.0 then
        data.damageAmount = 0.0
    end
    if data.firstTick == true then
        data.firstTick = false
        
        data.blackHole = addPostProcess("resources/post_process/blackhole.ps")
        data.blackHole:setEnabled(true)
        
        playSound({path = BlackHole.basePath .. "black_hole.ogg", position = monster.position, volume = 0.8})
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
            if data.soundPlayed == nil and t > 1.2 then
                data.soundPlayed = true
                playSound({path = missionPath .. "narsie/spawn.ogg", position = monster.position, volume = 1.5})
            end
            data.blackHole:setShaderWeight(size)
        elseif t < 9.0 then
            if monster:isVisible() == false then
                monster:setVisible(true)
            end
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
    
    if data.deathTimer ~= nil then
        t = (time - data.deathTimer) * 1.5 
        if data.blackHole:isEnabled() == false then
            data.blackHole:setEnabled(true)
        end
        
        local v = convertToScreenPosition(monster.position)
        data.blackHole:addUniformVec2("uRelBlackHolePos", v)
        
        local size = 0.8
        if t < 1.0 then
            data.blackHole:setShaderWeight(t * size)
        elseif t < 1.2 then
            data.blackHole:setShaderWeight(size)
            monster:setScale(1.02- t)
        elseif t < 2.2 then
            monster:setVisible(false)
            data.blackHole:setShaderWeight((2.2 - t) * size)
        else
            monster.removeOnDead = true
        end
        return
    end
    
    local d = data.target:distanceSquared(monster.position)
    if d < 10000 then
        NarSie.resetTarget(monster)
    else
        local toTarget = data.target - monster.position
        local angle = toTarget:getAngle()
        monster.moveAngle = approachAngle(monster.moveAngle, angle, 3.0 * dt)
    end
    
    
    local diffToPlayer = player.position - monster.position
    local distanceToPlayer = diffToPlayer:length()
    
    
    if data.pulling then
        data.pullTimer = data.pullTimer - dt
        
        local m = data.pullTimer * 0.5
    
        m = m * distanceToPlayer / 400.0
            
        if m > 1.0 then
            m = 1.0
        end
        player:addKnockback(-diffToPlayer:normalized() * 700.0 * m, -1.0)
        
        if data.pullTimer < 0.0 then
            data.pulling = false
        end
    else
        local range = 800.0 - min * 30.0 
        if distanceToPlayer > range then
        
            data.pullTimer = data.pullTimer + dt * clamp((distanceToPlayer - range) / 50.0)
            
            local maxStay = 0.8
            local c = 1.0 - data.pullTimer / maxStay
            local buff = player.colorMultiplier:addBuffWithId(monster.id, Vec4.new(1.0, c, c, 1.0))
            player.colorMultiplier:setBuffDuration(buff, 0.1)
            player.colorMultiplier:setBuffFadeInFadeOut(buff, 0.0, 0.1)
            
            if data.pullTimer > maxStay then
                data.pulling = true
                data.pullTimer = 2.0
            end
        else
            data.pullTimer = 0.0
        end
    
    end
    
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
        if monster.moveSpeed < 25.0 then
            monster.moveSpeed = monster.moveSpeed + dt * 4.0
            if monster.moveSpeed > 25.0 then
                monster.moveSpeed = 25.0
            end
        end
    
        data.currentAttackTimer = data.currentAttackTimer - dt
        if data.currentAttackTimer < 0.0 then
            data.currentAttack = data.attacks[ math.random( #data.attacks ) ]
            data.currentAttack(monster, true)
            data.currentAttackTimer = 10.0
            data.defaultShootTimer = 2.0
        end
        
        data.defaultShootTimer = data.defaultShootTimer - dt
        
        if data.defaultShootTimer < 0.0 and data.currentAttackTimer > 1.0 then
            data.defaultShootTimer = data.defaultShootTimer + lerp(1.2, 0.6, clamp(min /10.0))
            local b = NarSie.sendFireBall(monster)
            b.moveAngle = b.moveAngle + (math.random() * 0.5 - 0.25) * lerp(1.0, 0.3, clamp(min * 0.15))
            
        end
    else 
        if monster.moveSpeed > 0.0 then
            monster.moveSpeed = monster.moveSpeed - dt * 4.0
            if monster.moveSpeed < 0.0 then
                monster.moveSpeed = 0.0
            end
        end
        data.currentAttack(monster, false)
    end
    
end

function NarSie.resetTarget(monster)
    local t = math.random()
    
    monster.data.target = Vec2.randDir() * 300.0 * math.sqrt(t)
end

function NarSie.explosionsAroundPlayer(monster, init)
    if init then
        data.expAroundPlayerData = {}
        local expAroundPlayerData = data.expAroundPlayerData
        expAroundPlayerData.timeToShoot = 0.0
        expAroundPlayerData.count = 30 + clamp(min * 3)
        expAroundPlayerData.interval = lerp(0.22, 0.15, clamp(min * 0.1))
    end
    
    local expAroundPlayerData = data.expAroundPlayerData
    
    expAroundPlayerData.timeToShoot = expAroundPlayerData.timeToShoot - dt
    
    if expAroundPlayerData.timeToShoot < 0.0 and expAroundPlayerData.count > 0 then
        expAroundPlayerData.timeToShoot = expAroundPlayerData.timeToShoot + expAroundPlayerData.interval
    
    
        local g = addGameObject("ExpAroundPlayerObject")
        local t = math.random()
        g.data.pos = player.position + Vec2.randDir() * math.sqrt(t) * 200.0
        g.data.maxTime = 1.0
        expAroundPlayerData.count = expAroundPlayerData.count - 1
    end
    
    if expAroundPlayerData.count <= 0 and expAroundPlayerData.timeToShoot < -2.0 then
        data.currentAttack = nil
    end
end 


ExpAroundPlayerObject = {}

function ExpAroundPlayerObject.init(gameObject)
    gameObject:setLevel(RenderableLevel.player + 15)
    
    gameObject.data.ring = gameObject:addTexture(missionPath .. "narsie/ring.png", "~/resources/default")
    gameObject.data.ring:setAlignment(RenderableAlignment.world)
    gameObject.data.ring:setWorldMatrix(Mat3.fromScale(60.0, 60.0))
    gameObject.data.ring:setColor(Vec4.new(1.0, 0.8, 0.0, 0.0))
    gameObject.data.time = 0.0
end

function ExpAroundPlayerObject.onTick(gameObject)
    local t = gameObject.data.time /  gameObject.data.maxTime
    gameObject.data.time = gameObject.data.time + dt
    local alpha
    if t < 0.1 then
        alpha = t / 1.0
    elseif t > 0.98 then
        alpha = (1.0 - t) / 0.02
    else
        alpha = 1.0
    end
    if t > 1.0 then
        gameObject.toBeRemoved = true
        alpha = 0.0
        t = 1.0
        local e = addExplosion(gameObject.data.pos, 50.0, 80.0, 20, 40, 0.2, true, nil, true)
        e.renderable:setColor( Vec4.new( 1.0, 0.4, 0.4, 1.0 ) )
        local d = gameObject.data.pos:distance(player.position)
        d = clamp(1.0 - d / 200.0) * 0.4
        playSound({path = missionPath .. "~/resources/sounds/explode.ogg", position = gameObject.data.pos, volume = 0.1 + d })
        
    end
    gameObject.data.ring:setColor(Vec4.new(1.0, 1.0, 0.3, alpha * 0.6))
    gameObject:setPosition(gameObject.data.pos)
    gameObject:setScale(Vec2.new(1.0 - t * 0.8, 1.0 - t * 0.8))
end


function NarSie.explosions(monster, init)
    if init then
        data.explosionsData = {}
        local explosionsData = data.explosionsData
        explosionsData.timeToShoot = 0.0
        explosionsData.shootSpeed = lerp(1.0, 1.5, clamp(min * 0.1))
        explosionsData.count = 6
        explosionsData.interval = lerp(0.8, 0.4, clamp(min * 0.1))
        
    end
    
    local explosionsData = data.explosionsData
    
    explosionsData.timeToShoot = explosionsData.timeToShoot - dt
    
    if explosionsData.timeToShoot < 0.0 and explosionsData.count > 0 then
        explosionsData.timeToShoot = explosionsData.timeToShoot + explosionsData.interval
    
        local e = addExplosion(monster.position, 500.0, 150.0 * explosionsData.shootSpeed, 30 - explosionsData.count * 3, 40 - explosionsData.count * 3, 0.0, true, nil, explosionsData.count ~= 6 and explosionsData.count ~= 3 )
        
        e.renderable:setColor( Vec4.new( 1.0, 0.4, 0.4, 1.0 ) )
        
        playSound({path = missionPath .. "narsie/explosion07.ogg", position = monster.position, volume = 1.5})
        explosionsData.count = explosionsData.count - 1
    end
    
    if explosionsData.count <= 0 and explosionsData.timeToShoot < -2.0 then
        data.currentAttack = nil
    end
end 

function NarSie.spawnSpiders(monster, init)
    if init then
        data.timeToShoot = 0.0
        data.shift = 0.0
        data.count = 0
        data.numSpiders = math.floor(7 + min * 1 )
        data.interval = lerp(0.15, 0.10, clamp(min * 0.1))
        data.min = min
        data.angleShift = math.random() * 2.0 * math.pi
    end
    data.timeToShoot = data.timeToShoot - dt
    if data.timeToShoot < 0.0 and data.numSpiders > 0 then
        data.timeToShoot = data.timeToShoot + data.interval
       
        local newMonster = addMonster("Spider")
        Spider.buffStats(newMonster, monster.data.min)
        data = monster.data
        newMonster.position = monster.position
        newMonster:setScale(0.5)
        newMonster.colorMultiplier:addBuff(Vec4.new(0.2, 0.2, 0.2, 1.0))

        playSound({path = missionPath .. "narsie/spiderling.ogg", position = monster.position, volume = 1.0})
        
        newMonster.data.randomMove = false

        newMonster.experienceMultiplier = 0
        newMonster.scoreMultiplier = 0
        newMonster.moveSpeed = newMonster.moveSpeed * 1.6
        newMonster.hitPoint = 50
        newMonster.data.moveTimer = 60.0
        newMonster.moveAngle = math.pi * data.numSpiders / 8 + data.angleShift
        
        data.numSpiders = data.numSpiders - 1
        
    end
    
    if data.numSpiders <= 0 and data.timeToShoot < -3.0 then
        data.currentAttack = nil
    end
end

function NarSie.fireballs(monster, init)
    if init then
        data.timeToShoot = 0.0
        data.shift = 0.0
        data.count = 0
        data.numBullets = math.floor(3 + min * 0.1 )
        data.interval = lerp(0.8, 0.3, clamp(min * 0.1))
        data.totalCount = lerp(6, 12, clamp(min * 0.1))
        data.iter = 0
        
        data.s = 1.0
        if math.random() < 0.5 then
            data.s = -1.0
        end
    end
    
    data.timeToShoot = data.timeToShoot - dt
    
    if data.timeToShoot < 0.0 then
        
        local half = math.floor(data.numBullets / 2)
        
        local b = NarSie.sendFireBall(monster, data.s)
    
        b.moveAngle = b.moveAngle + (data.iter - half) * math.pi * 0.3 / data.numBullets
        b.damage = math.floor(b.damage / 2)
        
        data.iter = data.iter + 1
        
        if data.iter == data.numBullets then
            data.iter = 0
            data.timeToShoot = data.timeToShoot + data.interval
            data.totalCount = data.totalCount - 1
            
            if math.random() < 0.5 then
                data.s = -data.s
            end
            if data.totalCount <= 0 then
                data.currentAttack = nil
            end
        else
            data.timeToShoot = 0.1
        end
    end
end



function NarSie.fireballs2(monster, init)
    if init then
        data.timeToShoot = 0.0
        data.shift = 0.0
        data.interval = lerp(0.2, 0.12, clamp(min * 0.1))
        data.totalCount = lerp(20, 30, clamp(min * 0.1))
    end
    
    data.timeToShoot = data.timeToShoot - dt
    
    if data.timeToShoot < 0.0 then
        if data.totalCount > 0 then
            
            data.timeToShoot = data.timeToShoot + data.interval
            local b = NarSie.sendFireBall(monster)
            b.damage = math.floor(b.damage / 2)
            b.moveAngle = b.moveAngle + (math.random() * 0.5 - 0.25) * lerp(1.0, 0.3, clamp(min * 0.15))
            
            data.totalCount = data.totalCount - 1
        end
    end
    
    
    if data.timeToShoot < -1.0 and data.totalCount <= 0 then
        data.currentAttack = nil
    end
end




function NarSie.sendFireBall(monster, s)
    local bullet = addCustomBullet({monsterBullet = true})
    bullet.damage = math.floor(math.random(20, 30))
    bullet.moveSpeed =  300.0 + min * 30.0

    playSound({path = missionPath .. "narsie/fireball.ogg", position = monster.position, volume = 0.5})
    
    if s == nil then
        s = 1.0
        if math.random() < 0.5 then
            s = -1.0
        end
    end
    bullet.position = monster.position + Vec2.new( s * 50, -30.0)
    
    local d = player.position:distance(bullet.position) / bullet.moveSpeed
   
    local futurePos = player.position + player.moveVelocity * d
    
    bullet.moveAngle = (futurePos - bullet.position):getAngle()
    
    bullet.radius = 6.0
    bullet:addRenderableTextureWithSize(monster.monsterTemplate.basePath .. "bullet.png", Vec2.new(4.0, 4.0))
    
    
    -- fix
    -- particle = bullet:addTrailParticle("FlameParticle", Vec2.new(0.0, 0.0), 4.0, {})
    
    particle = bullet:addTrailParticle("RocketSmokeParticle", Vec2.new(0.0, 0.0), 2.0, {})
    particle:addLinearRandom("fadeOutSpeed", 1.0, 1.4)
    
    bullet.lifeTime = 9.0
    
    return bullet
end

function NarSie.addBullet(monster, angle, moveSpeed)
    local bullet = addCustomBullet({monsterBullet = true})
    bullet.damage = math.floor(math.random(20, 30))
    bullet.moveSpeed = data.moveSpeed
    bullet.moveAngle = angle
    bullet.position = monster.position + Vec2.fromAngle(bullet.moveAngle) * 40.0
    bullet.radius = 2.0
    bullet:addRenderableTextureWithSize("~/resources/monsters/bullet.png", Vec2.new(18.0, 18.0))
    bullet.lifeTime = 9.0
    

    return bullet
end

function NarSie.ringBullets(monster, init)
    if init then
        data.timeToShoot = 0.0
        data.shift = 0.0
        data.numBullets = math.floor(10 + min * 1 )
        data.interval = lerp(0.6, 0.3, clamp(min * 0.1))
        data.totalCount = lerp(10, 20, clamp(min * 0.1))
        data.moveSpeed = 100.0 + min * 2.0
    end
    
    data.timeToShoot = data.timeToShoot - dt
    
    if data.timeToShoot < 0.0 then
        data.timeToShoot = data.timeToShoot + data.interval
        
        local count = data.numBullets
        for i = 0, count - 1 do
            local bullet = NarSie.addBullet(monster, data.shift + i * math.pi * 2.0 / count, data.moveSpeed)
        end
        
        playSound({path = missionPath .. "narsie/bullet.ogg", position = monster.position, volume = 0.8})
        
        data.shift = data.shift + 0.4 * math.pi * 2.0 / count
        data.totalCount = data.totalCount - 1
        if data.totalCount <= 0 then
            data.currentAttack = nil
        end
    end
end

function NarSie.arcBullets(monster, init)
    if init then
        data.timeToShoot = 0.0
        data.count = 0
        data.numBullets = math.floor(15 + min * 2 )
        data.interval = lerp(0.6, 0.4, clamp(min * 0.1))
        data.totalCount = lerp(6, 12, clamp(min * 0.1))
        data.moveSpeed = 100.0 + min * 2.0
        data.iter = 0
        data.shift = (math.random() - 0.5) * math.pi  + (player.position - monster.position):getAngle()
        data.bulletSoundCount = 0
    end
    
    data.timeToShoot = data.timeToShoot - dt
    
    if data.timeToShoot < 0.0 then
        local b = NarSie.addBullet(monster, data.shift + (data.iter - data.numBullets / 2.0) * math.pi * 0.033, data.moveSpeed)
        if data.bulletSoundCount == 0 then
            playSound({path = missionPath .. "narsie/bullet2.ogg", position = monster.position, volume = 0.8})
            data.bulletSoundCount = 2
        end
        data.bulletSoundCount = data.bulletSoundCount - 1
        
        b.damage = math.floor(b.damage / 2)
        b.moveSpeed = b.moveSpeed * 1.3
        data.iter = data.iter + 1
        
        if data.iter >= data.numBullets then
            data.iter = 0
            data.timeToShoot = data.timeToShoot + data.interval
            data.totalCount = data.totalCount - 1
            
            data.shift = (math.random() - 0.5) * math.pi  + (player.position - monster.position):getAngle()
            if data.totalCount <= 0 then
                data.currentAttack = nil
            end
        else
            data.timeToShoot = 0.03
        end
        
    end
end

