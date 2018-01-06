
function calcRandomSpawns()
    missionData.spawnRates = {}
    local monsterTypeCount = getAllMonsterTypeCount()
    missionData.totalChanceSpawn = 0.0
    
    local m = ""
    for i = 0, monsterTypeCount - 1 do
        local monsterType = getMonsterTypeAt(i)
        if monsterType.scriptTable.spawnChanceInMission ~= nil then
            local chance = monsterType.scriptTable.spawnChanceInMission(missionData, min)
            if chance > 0.0 then
                missionData.totalChanceSpawn = missionData.totalChanceSpawn + chance
                missionData.spawnRates[monsterType.scriptName] = chance
            end
        end
    end
end

function addRandomMonster(forceType, cannotBecomeBoss, cannotShootBullets, levelReduce)
    local min = missionTime / 60.0 +  math.random() * 1.5
    
    if missionData.extraMin ~= nil then
        min = min + missionData.extraMin
    end
    if levelReduce ~= nil then
        min = min - levelReduce
    end
    
    if forceType ~= nil then
        m = forceType
    else
        local randChance = math.random() * missionData.totalChanceSpawn
        for key,value in pairs(missionData.spawnRates) do
            randChance = randChance - value
            if randChance <= 0.000001 then
                m = key
                break
            end
        end

        if m == "" then
            log("error not picked mon type")
            m = "Alien"
        end
    end
    
    local monster = addMonster(m)
    
    monster.hitPoint = math.floor(monster.hitPoint * (1.0 + clamp(min * 0.05) * 1.0))
    monster.experienceMultiplier = math.random() * 0.4 + 0.8
    monster.experienceMultiplier = 0.9 + math.random() * 0.2
    monster.scoreMultiplier = 0.9 + math.random() * 0.2
    
    monster.data.cannotBecomeBoss = cannotBecomeBoss
    monster.data.cannotShootBullets = cannotShootBullets
    
    if cannotBecomeBoss ~= true and monster.scriptTable.makeBoss ~= nil and missionData.lastBossSpawn + 15.0 - clamp(min/7) * 6 < missionTime and missionTime > 1.0 then
        missionData.lastBossSpawn = missionTime
        monster.data.isBoss = true
        monster.scriptTable.makeBoss(monster, min)
    else
        local r = 0.9 + 0.1 * math.random()
        local g = 0.9 + 0.1 * math.random()
        local b = 0.9 + 0.1 * math.random()
        local ra = math.random()
        if ra < 0.333 then
            r = 1.0
        elseif ra < 0.666 then
            g = 1.0
        else
            b = 1.0
        end
        
        monster.colorMultiplier:addBuff(Vec4.new(r, g, b, 1.0))
    end
    
    if monster.scriptTable.buffStats ~= nil then
        monster.scriptTable.buffStats(monster, min)
    end
    
    return monster
end

GhostBuffId = getGlobalUniqueId()

function makeBossDefault(monster, forceType)
    local min = missionTime / 60.0 +  math.random() * 1.5 + missionData.extraMin
    monster.hitPoint = math.floor(monster.hitPoint * 1.5)
    
    monster.data.minDamage = math.floor(monster.data.minDamage * 1.1)
    monster.data.maxDamage = math.floor(monster.data.maxDamage * 1.1)
    
    monster.data.maxMoveSpeed = monster.data.maxMoveSpeed * 1.05
    monster.data.randomMove = monster.data.randomMove or math.random() > 0.3
    monster.data.playerSeeRange = monster.data.playerSeeRange * 1.5
    
    monster.experienceMultiplier = 5.0 + math.random() * 2.0
    monster.scoreMultiplier = 5.0 + math.random() * 2.0
    monster:modifyDrawLevel(3)
    
    local t = 0
    repeat
        t = math.random(11)
    until missionTime > 30.0 or t ~= 8 
    
    if forceType ~= nil then
        t = forceType
    end
    
    
    if t == 1 then -- huge & tank
        monster.hitPoint = monster.hitPoint * 6
        monster.data.maxMoveSpeed = monster.data.maxMoveSpeed * 0.7
        monster.colorMultiplier:addBuff(Vec4.new(0.9, 0.8, 0.3, 1.0))
        monster:setScale(1.3 + math.random() * 0.1)
        monster.knockbackResistance:addBuff(0.07)
        monster.data.stunDuration = 0.0
        monster.data.slowDuration = 0.0
    elseif t == 2 then -- ghost
        monster.colorMultiplier:addBuffWithId(GhostBuffId, Vec4.new(0.5, 0.5, 0.5, 0.0))
        monster:setScale(monster.scale * 0.85)
        monster.hasCollision = false
        monster.hasBlood = false
        monster.hasGibs = false
        monster.data.randomMove = false
        monster.data.maxRotateSpeed = monster.data.maxRotateSpeed * 0.8
        addCustomOnTick(monster, function (monster)
            local diffToPlayer = player.position - monster.position 
            local distanceToPlayer = diffToPlayer:length()
            monster.colorMultiplier:addBuffWithId(GhostBuffId, Vec4.new(0.5, 0.5, 0.5, 0.7 * clamp( 1.0 - (distanceToPlayer - 150.0) / 300 ) ) )
        end)
    elseif t == 3 then -- hits hard
        monster.colorMultiplier:addBuff(Vec4.new(1.0, 0.3, 0.3, 1.0))
        monster.data.minDamage = math.floor(monster.data.minDamage * 3.0)
        monster.data.maxDamage = math.floor(monster.data.maxDamage * 4.0)
        monster.data.hitWaitTime = monster.data.hitWaitTime * 0.5
        monster.hitPoint = math.floor(monster.hitPoint * 1.1)
        monster.data.maxMoveSpeed = monster.data.maxMoveSpeed * 1.1
        monster.data.randomMove = false
    elseif t == 4 then -- fast
        monster.colorMultiplier:addBuff(Vec4.new(0.2, 0.7, 1.0, 1.0))
        monster.data.maxMoveSpeed = monster.data.maxMoveSpeed * 1.65
        monster.data.maxRotateSpeed = monster.data.maxRotateSpeed * 2.0
        monster.data.minDamage = math.floor(monster.data.minDamage * 0.8)
        monster.data.maxDamage = math.floor(monster.data.maxDamage * 0.8)
        monster.data.hitWaitTime = monster.data.hitWaitTime * 0.2
        monster.knockbackResistance:addBuff(0.4)
        monster.hitPoint = math.floor(monster.hitPoint * 0.6)
    elseif t == 5 then -- shoots bullets (fast)
        monster.colorMultiplier:addBuff(Vec4.new(0.2, 0.7, 0.3, 1.0))
        monster.data.shootsBullets = true
        monster.data.bulletMinDamage = math.floor(monster.data.bulletMinDamage * (1.0 + min * 0.1))
        monster.data.bulletMaxDamage = math.floor(monster.data.bulletMaxDamage * (1.0 + min * 0.1))
        
        monster.data.bulletRate = 1.0 - clamp(min * 0.1) * 0.4
        monster.data.bulletRandom = 0.2 - clamp(min * 0.15) * 0.15
        monster.data.bulletCount = 5 + math.floor( min / 2 ) * 2
    elseif t == 6 then -- spawns 2 on death
        monster.data.remainingLife = 3
        monster.colorMultiplier:addBuff(Vec4.new(0.7, 0.2, 0.7, 1.0))
        monster:setScale(monster.scale * (1.1 + math.random() * 0.1))
        monster.experienceMultiplier = monster.experienceMultiplier * 0.3
        monster.scoreMultiplier = monster.scoreMultiplier * 0.3
        monster.data.hitPoint = monster.hitPoint
        monster.data.onKillFuncSplit = function (monster)
            if monster.data.remainingLife > 0 then
                monster.data.remainingLife = monster.data.remainingLife - 1
                for i = 1,2 do
                    if monster.hitPoint > 0 then
                        return
                    end
                    local newMonster = addMonster(monster.monsterTemplate.name)
                    newMonster.data.remainingLife = monster.data.remainingLife
                    newMonster.position = monster.position
                    newMonster:setScale(math.max(0.40, monster.scale * 0.80))
                    newMonster.colorMultiplier:addBuff(Vec4.new(0.7, 0.2, 0.7, 1.0))
                    newMonster:copyIgnoreId(monster)
                    
                    newMonster.data.playerSeeRange = monster.data.playerSeeRange
                    newMonster.data.maxMoveSpeed = monster.data.maxMoveSpeed * 0.8
                    newMonster.data.maxRotateSpeed = monster.data.maxRotateSpeed

                    newMonster.data.hitWaitTime = monster.data.hitWaitTime
                    newMonster.data.hitInterval = monster.data.hitInterval
                    newMonster.data.minDamage = monster.data.minDamage
                    newMonster.data.maxDamage = monster.data.maxDamage

                    newMonster.experienceMultiplier = monster.experienceMultiplier * 0.5
                    newMonster.scoreMultiplier = monster.scoreMultiplier * 0.5

                    newMonster.hitPoint = math.floor(monster.data.hitPoint * 0.5)
                    newMonster.data.hitPoint = newMonster.hitPoint
                    newMonster.moveAngle = monster.moveAngle + math.pi * (i - 0.5)
                    newMonster.data.onKillFuncSplit = monster.data.onKillFuncSplit
                    addCustomOnKill(newMonster, monster.data.onKillFuncSplit)
                    
                    newMonster.data.invulTime = time + 0.3
                    
                    addCustomOnHit(newMonster, function(monster, damage, args)
                        if time < monster.data.invulTime then
                            return 0
                        end
                        return damage
                    end)
                end
            end
        end
        addCustomOnKill(monster, monster.data.onKillFuncSplit)
    elseif t == 7 then -- spawns 8 on death
        monster.colorMultiplier:addBuff(Vec4.new(0.2, 0.2, 0.2, 1.0))
        monster:setScale(1.0 + math.random() * 0.2)
        monster.experienceMultiplier = monster.experienceMultiplier * 0.5
        monster.scoreMultiplier = monster.scoreMultiplier * 0.5
        monster.data.hitPoint = monster.hitPoint
        addCustomOnKill(monster, function (monster)
            if monster.hitPoint > 0 then
                return
            end
            for i = 1,8 do
                local newMonster = addMonster(monster.monsterTemplate.name)
                newMonster.position = monster.position
                newMonster:setScale(math.max(0.40, monster.scale * 0.50))
                newMonster.colorMultiplier:addBuff(Vec4.new(0.2, 0.2, 0.2, 1.0))
                newMonster:copyIgnoreId(monster)
                
                newMonster.data.playerSeeRange = monster.data.playerSeeRange
                newMonster.data.maxMoveSpeed = monster.data.maxMoveSpeed * 1.2
                newMonster.data.maxRotateSpeed = monster.data.maxRotateSpeed * 0.7

                newMonster.data.hitWaitTime = monster.data.hitWaitTime
                newMonster.data.hitInterval = monster.data.hitInterval
                newMonster.data.minDamage = monster.data.minDamage / 2
                newMonster.data.maxDamage = monster.data.maxDamage / 2

                newMonster.experienceMultiplier = monster.experienceMultiplier * 0.1
                newMonster.scoreMultiplier = monster.scoreMultiplier * 0.1

                newMonster.hitPoint = math.floor(monster.data.hitPoint * 0.5)
                newMonster.data.hitPoint = newMonster.hitPoint
                    
                newMonster.data.randomMove = true
                newMonster.moveAngle = monster.moveAngle + math.pi * i / 8
                
                newMonster.data.invulTime = time + 0.3
                addCustomOnHit(newMonster, function(monster, damage, args)
                    if time < monster.data.invulTime then
                        return 0
                    end
                    return damage
                end)
            end
        end)
    elseif t == 8 then -- angel
        monster.colorMultiplier:addBuff(Vec4.new(2.0, 2.0, 2.0, 1.0))
        monster.data.maxMoveSpeed = math.min(500.0, monster.data.maxMoveSpeed * 3.0)
        monster.data.originalSpeed = monster.data.maxMoveSpeed
        monster.data.maxRotateSpeed =  monster.data.maxRotateSpeed * 3.0
        monster.data.originalRotateSpeed =  monster.data.maxRotateSpeed
            
        monster.data.minDamage = math.floor(monster.data.minDamage * 0.5)
        monster.data.maxDamage = math.floor(monster.data.maxDamage * 0.5)
        
        monster.data.hitWaitTime = monster.data.hitWaitTime * 0.1
        monster.data.hitInterval = monster.data.hitInterval * 0.4
        monster.data.targetShift = Vec2.new(0.0, 0.0)
        monster.data.randomMove = false
        monster.knockbackResistance:addBuff(0.0)
        addCustomOnTick(monster, function (monster)
            local diffToPlayer = player.position - monster.position 
            local distanceToPlayer = diffToPlayer:length()
            local angleToPlayer = diffToPlayer:getAngle() - math.pi
            local angle = 0.2
            if distanceToPlayer < 150 then
                angle = angle + 1.4 * (1.0 - distanceToPlayer / 150.0)
            end
            if distanceToPlayer > 1000 or math.abs(angleDiff(angleToPlayer, player.aimAngle)) < angle then
                monster.data.maxMoveSpeed = 0.0
                monster.animationSpeed = 0.0
                monster.moveSpeed = 0.0
                monster.data.maxRotateSpeed = 0.0
                monster.data.canHit = false
            else
                monster.data.maxMoveSpeed = monster.data.originalSpeed
                monster.animationSpeed = 1.0
                monster.data.maxRotateSpeed = monster.data.originalSpeed
                monster.data.canHit = true
            end
        end)
    elseif t == 9 then -- invulnerability after hit
        monster.colorMultiplier:addBuff(Vec4.new(0.7, 1.7, 0.7, 1.0))
        addCustomOnHit(monster, function(monster, damage, args)
            if time - monster.data.lastHitTime < 1.0 then
                damage = 0
            end
            
            local buffId = monster.moveSpeedMultiplier:addBuffWithId(monster.id, 0.3)
            monster.moveSpeedMultiplier:setBuffDuration(buffId, 1.0)
            
            buffId = monster.colorMultiplier:addBuff(Vec4.new(1.0, 1.0, 1.0, 0.2))
            monster.colorMultiplier:setBuffDuration(buffId, 1.0)
            monster.data.lastHitTime = time
            return damage
        end)
        addCustomShouldHit(monster, function(monster, gun, bullet)
            return monster.data.lastHitTime == nil or time - monster.data.lastHitTime > 1.0
        end)
    elseif t == 10 then -- blinks
        monster.data.blinkParticle = monster:addParticleSpawner("CriticalParticle", {});
        monster.colorMultiplier:addBuff(Vec4.new(0.7, 1.7, 1.7, 1.0))
        monster.hitPoint = math.floor(monster.hitPoint * 1.5)

        addCustomOnHit(monster, function(monster, damage, args)
            if monster.hitPoint > damage then
                local t = math.random() * math.pi * 2.0
                local r = math.random() * 100.0 + 100.0
                local v = Vec2.fromAngle(t) * r
                for i=1,8 do
                    monster:spawnParticle(monster.data.blinkParticle, {initialScale = 15.0, moveSpeed = 150.0})
                end
                monster.position = monster.position + v
                for i=1,8 do
                    monster:spawnParticle(monster.data.blinkParticle, {initialScale = 15.0, moveSpeed = 150.0})
                end
                
                playSound({path = "~/resources/sounds/shimmer.ogg"})
            end
            return damage
        end)
    elseif t == 11 then -- spawn little clones
        monster.colorMultiplier:addBuff(Vec4.new(0.722, 0.369, 0.176, 1.0))
        monster.moveSpeed = monster.moveSpeed * 0.5
        monster.data.maxHitpoint = monster.hitPoint
        monster:setScale(0.8 + math.random() * 0.3)
        
        addCustomOnTick(monster, function (monster)
            if monster.data.spawnTimer == nil then
                monster.data.spawnTimer = 3.0
            end
            monster.data.spawnTimer = monster.data.spawnTimer - dt
            
            if monster.data.spawnTimer < 0.0 then
                monster.data.spawnTimer = monster.data.spawnTimer + 5.0
                
                if missionData.ignoreMonsterCount < 100 then
                    local newMonster = addMonster(monster.monsterTemplate.name)
                    newMonster.position = monster.position
                    newMonster:setScale(math.max(0.3, monster.scale * 0.60))
                    newMonster.colorMultiplier:addBuff(Vec4.new(0.722, 0.369, 0.176, 1.0))
                    
                    newMonster.data.playerSeeRange = monster.data.playerSeeRange
                    newMonster.data.maxMoveSpeed = monster.data.maxMoveSpeed * 0.8
                    newMonster.data.maxRotateSpeed = monster.data.maxRotateSpeed

                    newMonster.data.hitWaitTime = monster.data.hitWaitTime
                    newMonster.data.hitInterval = monster.data.hitInterval
                    newMonster.data.minDamage = math.ceil(monster.data.minDamage * 0.4)
                    newMonster.data.maxDamage = math.ceil(monster.data.maxDamage * 0.4)

                    newMonster.experienceMultiplier = 0.0
                    newMonster.scoreMultiplier = 0.0

                    newMonster.hitPoint = math.floor(monster.data.maxHitpoint * 0.2)
                    newMonster.data.randomMove = true
                    newMonster.moveAngle = -monster.moveAngle
                    
                    ignoreMonsterForCount(newMonster)
                end
            end
        end)
    end
end

function removeBuffIcon(name, dontUpdate)
    local buffData = missionData.buffs[name] 
    if buffData ~= nil then
        missionData.buffObject:removeTexture(buffData.renderable)
        missionData.buffs[name] = nil
        missionData.buffCount = missionData.buffCount - 1
        
        if dontUpdate ~= true then
            updateBuffPositions()
        end
    end
end

function addBuffIcon(name, path)
    if missionData.buffObject == nil then
        missionData.buffObject = addGameObject("BuffIcons")
        missionData.buffObject:setAlignment(RenderableAlignment.top)
        missionData.buffObject:setLevel(RenderableLevel.GUI)
        BuffIcons.repositionGUI(missionData.buffObject)
        missionData.buffs = {}
        missionData.buffCount = 0
    end
    
    removeBuffIcon(name, true)
    
    local buffObject = missionData.buffObject
    local renderable = buffObject:addTexture(path, "~/resources/default")
    renderable:setAlignment(RenderableAlignment.top)
    
    local buffData = {}
    buffData.renderable = renderable
    buffData.name = name
    buffData.id = getUniqueId()
    missionData.buffs[name] = buffData
    missionData.buffCount = missionData.buffCount + 1
    
    updateBuffPositions()
end

function updateBuffPositions()
    local p = 0
    local shift = (missionData.buffCount - 1) * 34.0 / 2
    
    for key,value in pairs(missionData.buffs) do
        value.renderable:setWorldMatrix(Mat3.fromPositionAndScale(Vec2.new(p * 34 - shift, 0), Vec2.new(15.0, 15.0)))
        p = p + 1
    end
end

BuffIcons = {}
function BuffIcons.init()
end
function BuffIcons.repositionGUI(gameObject, size)
    gameObject:setPosition(Vec2.new(0, -90 / getCameraZoom()))
    gameObject:setScale(Vec2.new(1.0 / getCameraZoom(), 1.0 / getCameraZoom()))
end


function ignoreMonsterForCount(monster)
    if monster.data.ignoreForCount ~= true then
        monster.data.ignoreForCount = true
        missionData.ignoreMonsterCount = missionData.ignoreMonsterCount + 1
    end
end

function canSpawnMonster()
    return (missionData.maxMonster == nil or getMonsterCount() < missionData.maxMonster + missionData.ignoreMonsterCount)
end

function canSpawnIgnoredMonster()
    return (missionData.maxMonster == nil or getMonsterCount() < missionData.maxMonster) and (missionData.ignoreMonsterCount == nil or missionData.ignoreMonsterCount < 100)
end

function gameResetTick()
    if missionData.enableReset == true then
        if missionData.scoreText ~= nil then
            missionData.scoreText:setText("Score : " .. getScore())
        end
        
        if isKeyPressed(keys.Space) then
            loadMission(missionScript)
        end
    end
end
    
function showGameReset(title)
    if title == nil then
        title = "You Died"
    end
    local space = 0.0
    if string.match(title, "\n") then
        space = 50.0
    end
    
    local gameObject = addGameObject("FadeOutImage")
    gameObject.data.startTime = time
    gameObject.data.fadeOutStartTime = -1
    gameObject.data.fadeInDuration = 1.0
    gameObject:setLevel(RenderableLevel.GUI + 5)
    gameObject.data.renderable = gameObject:addText(title, "resources/fontData.txt")
    gameObject.data.renderable:setAlignment(RenderableAlignment.center)
    gameObject.data.renderable:setTextAlignment(TextAlignment.center)
    gameObject.data.renderable:setTextSize(120.0)
    gameObject:setPosition(Vec2.new(0, 50 + space))
    
    gameObject = addGameObject("FadeOutImage")
    gameObject.data.startTime = time
    gameObject.data.fadeOutStartTime = -1
    gameObject.data.fadeInDuration = 1.0
    gameObject:setLevel(RenderableLevel.GUI + 5)
    gameObject.data.renderable = gameObject:addText("Press Space to Reset", "resources/fontData.txt")
    gameObject.data.renderable:setAlignment(RenderableAlignment.center)
    gameObject.data.renderable:setTextAlignment(TextAlignment.center)
    gameObject.data.renderable:setTextSize(32.0)
    gameObject:setPosition(Vec2.new(0, -40 -space))
    
    gameObject = addGameObject("FadeOutImage")
    gameObject.data.startTime = time
    gameObject.data.fadeOutStartTime = -1
    gameObject.data.fadeInDuration = 1.0
    gameObject:setLevel(RenderableLevel.GUI + 5)
    gameObject.data.renderable = gameObject:addText("Esc to Exit", "resources/fontData.txt")
    gameObject.data.renderable:setAlignment(RenderableAlignment.center)
    gameObject.data.renderable:setTextAlignment(TextAlignment.center)
    gameObject.data.renderable:setTextSize(32.0)
    gameObject:setPosition(Vec2.new(0, -80-space))
    
    gameObject = addGameObject("FadeOutImage")
    gameObject.data.startTime = time
    gameObject.data.fadeOutStartTime = -1
    gameObject.data.fadeInDuration = 1.0
    gameObject:setLevel(RenderableLevel.GUI + 5)
    gameObject.data.renderable = gameObject:addText("Score : " .. getScore(), "resources/fontData.txt")
    gameObject.data.renderable:setAlignment(RenderableAlignment.center)
    gameObject.data.renderable:setTextAlignment(TextAlignment.center)
    gameObject.data.renderable:setTextSize(32.0)
    gameObject:setPosition(Vec2.new(0, -160-space))
    
    missionData.scoreText = gameObject.data.renderable
    
    missionData.enableReset = true
end

function missionInit(missionData)
    missionData.ignoreMonsterCount = 0
end

function debugInit(missionData)
    missionData.extraMin = 0.0
end

function debugTick(missionData)

    if hasCheats() then
        if isKeyReleased(keys.PageUp) then
            missionData.extraMin = missionData.extraMin + 0.5
            log("Extra Min " .. missionData.extraMin)
        end
        if isKeyReleased(keys.PageDown) then
            missionData.extraMin = missionData.extraMin - 0.5
            if missionData.extraMin < 0.0 then
                missionData.extraMin = 0.0
            end
            log("Extra Min " .. missionData.extraMin)
        end
    end
end


function addRandomColliders(count, distanceToMid)
    local colliderSpawnChances = {}
    local totalChance = 0.0
    for name,collider in pairs(Colliders) do 
        local chance = 1.0
        if collider.jsonTable.chance ~= nil then
            chance = collider.jsonTable.chance
        end
        totalChance = totalChance + chance
        colliderSpawnChances[name] = chance
    end
    
    for i=1,count do
        local m = nil
        local randChance = math.random() * totalChance
        for key,value in pairs(colliderSpawnChances) do
            randChance = randChance - value
            if randChance <= 0.000001 then
                m = key
                break
            end
        end
        if m ~= nil then
            addGameObjectUsingTemplate(m, {distanceToMid = distanceToMid})
        end
    end
end

