
StunController = {}

function StunController.init(monster)
    data.stunDuration = 0.02
    data.slowDuration = 0.15
    data.slowMultiplier = 0.4
    data.stunTime = 0.0
    data.slowTime = 0.0
end

function StunController.buffStats(monster, min)

end

function StunController.getSlowAmount(monster)
    local mul = 1.0

    if data.stunTime > 0.0 then
        mul = 0.0
        data.stunTime = data.stunTime - dt
    elseif data.slowTime > 0.0 then
        mul = data.customSlowdownAmount * data.slowMultiplier * (data.slowDuration - data.slowTime)/ data.slowDuration
        data.slowTime = data.slowTime - dt
    end
    return mul
end

function StunController.onHit(monster, damage, dir, args)
    if args.noSlowdown == true then
        return
    end
    local data = monster.data
    if args.customSlowdownAmount ~= nil then
        data.customSlowdownAmount = args.customSlowdownAmount
    else
        data.customSlowdownAmount = 1.0
    end
    if args.slowdownOnly then
        data.slowTime = data.slowDuration
    elseif args.doNotStun ~= true then
        data.stunTime = data.stunDuration
        data.slowTime = data.slowDuration
    end
end



MeleeHitImage = {}

function MeleeHitImage.build(monster)
    local gameObject =  addGameObject("MeleeHitImage")
    gameObject:setLevel(RenderableLevel.player + 5)
    gameObject.data.startTime = time
    gameObject.data.renderable = gameObject:addTexture("~/resources/monsters/melee_hit.png", "~/resources/default")
    gameObject.data.renderable:setWorldMatrix(Mat3.fromScale(20.0, 20.0))
    gameObject:setPosition(player.position)
    gameObject:setRotation(-math.pi * 0.5 - monster.moveAngle)
    return gameObject
end

function MeleeHitImage.init(gameObject)
end

function MeleeHitImage.onTick(gameObject)
    local timeDiff = time - gameObject.data.startTime
    local alpha
    if timeDiff < 0.05 then
        alpha = timeDiff / 0.05
    else 
        alpha = 1.0 - (timeDiff - 0.3) / 0.2
        if alpha < 0.0 then 
            gameObject.toBeRemoved = true
            alpha = 0.0
        elseif alpha > 1.0 then
            alpha = 1.0
        end
    end
    gameObject.data.renderable:setColor(Vec4.new(1.0, 1.0, 1.0, alpha))
end

function MeleeHitImage.clear(monster)

end

MonsterGroupHelper = {}

function MonsterGroupHelper.init(monster) 
    monster.data.closestMonsterIndex = -1
    monster.data.playerIgnoreDistance = 0.0
end

function MonsterGroupHelper.fixAngle(monster, angle) 
    local closestMonster = nil
    if data.closestMonsterIndex ~= -1 then
        closestMonster = getMonster(data.closestMonsterIndex)
        if closestMonster ~= nil then
            if closestMonster.position:distanceSquared(monster.position) > 30.0 * 30.0 then
                closestMonster = nil
            end
        end
    end
    
    if closestMonster == nil then
        closestMonster = getClosestMonsterInRangeWithIgnoreId(monster.position, 25.0, {monster.id})
    else
        local newClosestMonster = getClosestMonsterInRangeWithIgnoreId(monster.position, 25.0, {monster.id})
        
        if newClosestMonster ~= nil and newClosestMonster ~= closestMonster then
            if closestMonster.position:distanceSquared(monster.position) > newClosestMonster.position:distanceSquared(monster.position) + 15 * 15 then
                closestMonster = newClosestMonster
            end
        end
    end
    
    if closestMonster ~= nil then
        data.closestMonsterIndex = closestMonster.id
        local toOther = closestMonster.position - monster.position;
        local c = 1.0 - toOther:length() / 30.0
            
        local cPlayer = (distanceToPlayer - data.playerIgnoreDistance) / 100
        if cPlayer < 0.0 then
            cPlayer = 0.0
        end
        local dot = toOther:sideVec():dot(diffToPlayer)
        if dot > 0.0 then
            angle = angle + 0.6 * c * cPlayer
        else
            angle = angle - 0.6 * c * cPlayer
        end
    end
    return angle
end

MonsterMeleeHelper = {}
MonsterMeleeHelper.slowBuffId = getGlobalUniqueId()

function MonsterMeleeHelper.init(monster) 
    monster.data.moving = true
    monster.data.lastHitTime = -1.0
    monster.data.hitInterval = 1.5
    monster.data.hitWaitTime = 0.10
    monster.data.minDamage = 5
    monster.data.maxDamage = 11
    monster.data.slowdownAmount = 0.6
    monster.data.slowdownDuration = 0.12
    monster.data.canHit = true
end

function MonsterMeleeHelper.buffStats(monster, min)
    local data = monster.data
    data.hitWaitTime = data.hitWaitTime  * (1.0 - clamp(min * 0.1) * 0.8)
    data.hitInterval = data.hitInterval  * (1.0 - clamp(min * 0.1) * 0.4)
    data.minDamage = math.floor(data.minDamage * (1.0 + min * 0.2))
    data.maxDamage = math.floor(data.maxDamage * (1.0 + min * 0.2))
end

function MonsterMeleeHelper.onTick(monster)
    if data.canHit == false then
        return
    end
    local range = 20.0
    if data.moving == false then
        range = 30.0
    end
    
    if distanceToPlayer < range + monster.collisionRadius and player.isDead == false then
        if data.lastHitTime + data.hitInterval < time then
            data.lastHitTime = time
            data.moving = false
            monster:playAnimation("attack")
            data.willHit = true
        end
        if data.willHit and data.lastHitTime + data.hitWaitTime < time then
            data.willHit = false
            local damage = player:doDamage(math.floor(data.minDamage + math.random() *(data.maxDamage - data.minDamage)), angleToPlayer)
            if damage > 0 then
                playSound({path = "~/resources/sounds/melee_woosh.ogg", volume = 0.3})
                if player.data.noSlowdownOnHit == nil then
                    player.maxSpeed:addBuffWithId(MonsterMeleeHelper.slowBuffId, data.slowdownAmount)
                    player.maxSpeed:setBuffDuration(MonsterMeleeHelper.slowBuffId, data.slowdownDuration)
                    player.maxSpeed:setBuffFadeInFadeOut(MonsterMeleeHelper.slowBuffId, data.slowdownDuration/4, data.slowdownDuration/4)
                end
            end
            
            MeleeHitImage.build(monster)
        end
    else
        data.willHit = false
        if data.moving == false then
            data.moving = true
            monster:playAnimation("walk", math.random())
        end
    end
end



BulletShooter = {}

function BulletShooter.init(monster)
    data.bulletMinDamage = 2
    data.bulletMaxDamage = 6
    data.bulletSpeed = 300
    data.bulletRate = 3.0
    data.bulletRandom = 0.3
    data.shootsBullets = false
end

function BulletShooter.buffStats(monster, min)
    local data = monster.data
    
    if data.isBoss ~= true and data.cannotShootBullets ~= true and math.random() > 0.98 - clamp(min * 0.2) * 0.05 then
        data.shootsBullets = true
        data.bulletMinDamage = math.floor(data.bulletMinDamage * (1.0 + min * 0.3))
        data.bulletMaxDamage = math.floor(data.bulletMaxDamage * (1.0 + min * 0.4))
        data.bulletRate = data.bulletRate - clamp(min * 0.1) * 2.0
        data.bulletRandom = data.bulletRandom - clamp(min * 0.15) * 0.2
        monster.colorMultiplier:addBuff(Vec4.new(0.8, 0.95, 0.8, 1.0))
        monster:modifyDrawLevel(1)
    end
end

function BulletShooter.onTick(monster)
    if data.shootsBullets then
        if data.bulletTimer == nil then
            data.bulletTimer = data.bulletRate + math.random() * 2.0
        end
        data.bulletTimer = data.bulletTimer - dt
        if data.bulletTimer < 0.0 then
            data.bulletTimer = data.bulletRate + math.random() * 2.0
            
            local bullet = addCustomBullet({monsterBullet = true})
            bullet.damage = math.floor(math.random(data.bulletMinDamage, data.bulletMaxDamage))
            bullet.position = monster.position + monster.moveDir * 6.0
            bullet.moveSpeed = data.bulletSpeed
            bullet.moveAngle = monster.moveAngle + math.random() * data.bulletRandom * 2.0 - data.bulletRandom
            bullet.radius = 6.0
            bullet:addRenderableTextureWithSize("~/resources/monsters/bullet.png", Vec2.new(18.0, 18.0))
        end
    end
end

function addCustomOnTick(monster, tick)
    if monster.data.customTick == nil then
      monster.data.customTick = {}
      monster.data.customTickCount = 0
    end
    monster.data.customTick[monster.data.customTickCount] = tick
    monster.data.customTickCount = monster.data.customTickCount + 1
end

function customMonsterOnTick(monster)
    if monster.data.customTick ~= nil then
        for key,tick in pairs( monster.data.customTick) do
            tick(monster)
        end
    end
end

function addCustomOnKill(monster, kill)
    if monster.data.customKill == nil then
      monster.data.customKill = {}
      monster.data.customKillCount = 0
    end
    monster.data.customKill[monster.data.customKillCount] = kill
    monster.data.customKillCount = monster.data.customKillCount + 1
end

function customMonsterOnKill(monster, damage, dir, args)
    if monster.data.customKill ~= nil then
        for key,kill in pairs(monster.data.customKill) do
            kill(monster, damage, dir, args)
        end
    end
end

function addCustomOnHit(monster, hit)
    if monster.data.customHit == nil then
      monster.data.customHit = {}
      monster.data.customHitCount = 0
    end
    monster.data.customHit[monster.data.customHitCount] = hit
    monster.data.customHitCount = monster.data.customHitCount + 1
end

function customMonsterOnHit(monster, damage, dir, args)
    if monster.data.customHit ~= nil then
        for key,hit in pairs(monster.data.customHit) do
            damage = hit(monster, damage, dir, args)
            if damage <= 0 then
                break
            end
        end
    end
    return damage
end

function addCustomShouldHit(monster, shouldHit)
    if monster.data.customShouldHit == nil then
      monster.data.customShouldHit = {}
      monster.data.customShouldHitCount = 0
    end
    monster.data.customShouldHit[monster.data.customShouldHitCount] = shouldHit
    monster.data.customShouldHitCount = monster.data.customShouldHitCount + 1
end

function customMonsterShouldHit(monster, gun, bullet)
    if monster.data.customShouldHit ~= nil then
        for key,shouldHit in pairs(monster.data.customShouldHit) do
            local r = shouldHit(monster, gun, bullet)
            if r == false then
                return false
            end
        end
    end
    return true
end