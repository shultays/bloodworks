
local newMonsterAlphaColorID = getGlobalUniqueId()


function addMonsterCampaign(spawnData)
    if spawnData.randomMonsterList == nil then
        spawnData.randomMonsterList = createWeightedRandomList()
        spawnData.randomMonsterList.add("Alien", 1.0)
    end
    local monster = addMonster(spawnData.randomMonsterList.get())
    
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
    
    if spawnData.gunOnFirstMonster ~= false and missionData.firstKilled ~= true then
        addCustomOnKill(monster, function (monster)
            if missionData.firstKilled ~= true then
                missionData.firstKilled = true
                spawnRandomGun(monster.position)
            end
        end)
    end
    return monster
end

function addMonsterWithParticles(spawnData)
    local monster = addMonsterCampaign(spawnData)

    monster.colorMultiplier:addBuffWithId(newMonsterAlphaColorID, Vec4:new(1.0, 1.0, 1.0, 0.0))
    local colorbuff = monster.colorMultiplier:getBuffInfo(newMonsterAlphaColorID)
    colorbuff:setBuffDuration(0.4)
    colorbuff:setBuffFadeInFadeOut(-1.0, 0.4)

    monster.moveSpeedMultiplier:addBuff(spawnData.monsterSpeed)
    monster.hitPoint = math.ceil(monster.hitPoint * spawnData.hitPoint)
    
    monster.data.minDamage = math.floor(monster.data.minDamage * spawnData.damage)
    monster.data.maxDamage = math.floor(monster.data.maxDamage * spawnData.damage)
    
    monster.data.randomMove = math.random() < spawnData.randomMoveChance

    monster.moveSpeedMultiplier:addBuffWithId(newMonsterAlphaColorID, 0.0)
    local moveBuff = monster.moveSpeedMultiplier:getBuffInfo(newMonsterAlphaColorID)
    moveBuff:setBuffDuration(0.8)
    moveBuff:setBuffFadeInFadeOut(-1.0, 0.8)
    
    return monster
end


function randomizeMonsterPosAroundPlayer(monster, a, b)
    if a == nil then
        a = 400.0
    end
    if b == nil then
        b = a * 0.5
    end
    
    local t = math.sqrt(math.random())
    
    local tr = 10
    while tr > 0 do
        tr = tr - 1
        local p = player.position + Vec2.randDir() * (t * (a - b) + b)
        if isGoodSpawnPos(p) then
            monster.position = p
            break
        end
    end
    
    monster.moveAngle = (player.position - monster.position):getAngle()
end


function initTimedMonster() 

    local timedMonsterData = {}

    timedMonsterData.monsterSpeed = 1.0
    timedMonsterData.hitPoint = 1.0
    timedMonsterData.randomMoveChance = 0.0
    timedMonsterData.damage = 1.0
    
    timedMonsterData.maxRandomMonsterAtOnce = 5.0
    timedMonsterData.randomMonsterTimer = 0.0
    timedMonsterData.randomMonsterTimerInterval = 0.7
    timedMonsterData.randomMonsterCount = 0
    
    timedMonsterData.randomMonsterWaiting = true
    timedMonsterData.randomMonsterWaitTime = 2.0
    
    timedMonsterData.randomMonsterMax = 20
    timedMonsterData.randomMonsterCur = 0
    
    timedMonsterData.getMaxMonster = function()
        return timedMonsterData.randomMonsterMax
    end
    
    function scopeTemp()
        checkTimedMonster(timedMonsterData)
    end
    addCustomTick(scopeTemp)
    
    return timedMonsterData
end

function checkTimedMonster(timedMonsterData)
    if timedMonsterData.randomMonsterMax <= timedMonsterData.randomMonsterCur then
        return
    end
    timedMonsterData.randomMonsterWaitTime = timedMonsterData.randomMonsterWaitTime - dt
    if timedMonsterData.randomMonsterWaitTime < 0.0 then
        if timedMonsterData.randomMonsterWaiting then
            timedMonsterData.randomMonsterWaitTime = 4.0
            timedMonsterData.randomMonsterWaiting = false
        else
            timedMonsterData.randomMonsterWaitTime = 5.0
            timedMonsterData.randomMonsterWaiting = true
        end
    end
    if timedMonsterData.randomMonsterWaiting then
        return
    end
    if timedMonsterData.randomMonsterCount >= timedMonsterData.maxRandomMonsterAtOnce then
        timedMonsterData.randomMonsterTimer = timedMonsterData.randomMonsterTimerInterval * 3.0
    else
        timedMonsterData.randomMonsterTimer = timedMonsterData.randomMonsterTimer - dt
        if timedMonsterData.randomMonsterTimer < 0.0 then
            timedMonsterData.randomMonsterTimer = timedMonsterData.randomMonsterTimer + timedMonsterData.randomMonsterTimerInterval
            
            local monster = addMonsterWithParticles(timedMonsterData)
            
            timedMonsterData.randomMonsterCur = timedMonsterData.randomMonsterCur + 1
            randomizeMonsterPosAroundPlayer(monster)
            timedMonsterData.randomMonsterCount = timedMonsterData.randomMonsterCount + 1
            addCustomOnKill(monster, function (monster)
                timedMonsterData.randomMonsterCount = timedMonsterData.randomMonsterCount - 1
                if timedMonsterData.dontAddObjectiveForRandom ~= true then
                    missionData.curObjective = missionData.curObjective + 1
                end
            end)
        end
    end
end


function initTimedLineMonster() 
    local timedMonsterLineData = {}
    
    timedMonsterLineData.monsterSpeed = 1.0
    timedMonsterLineData.hitPoint = 1.0
    timedMonsterLineData.randomMoveChance = 0.0
    timedMonsterLineData.damage = 1.0
    
    timedMonsterLineData.monsterPerLine = 5
    timedMonsterLineData.randomMonsterTimer = 0.0
    timedMonsterLineData.randomMonsterTimerInterval = 0.4
    timedMonsterLineData.randomMonsterTimerWaitTimeInterval = 4.0
    timedMonsterLineData.randomMonsterCount = 0
    
    timedMonsterLineData.randomMonsterWaiting = true
    timedMonsterLineData.randomMonsterWaitTime = 2.0
    
    timedMonsterLineData.maxLineCount = 5
    timedMonsterLineData.curLineCount = 1
    
    timedMonsterLineData.spawnCount = 0
    timedMonsterLineData.lineDist = 60.0
    
    
    timedMonsterLineData.getMaxMonster = function()
        return timedMonsterLineData.maxLineCount * timedMonsterLineData.monsterPerLine
    end
    
    
    function scopeTemp()
        checkTimedLineMonster(timedMonsterLineData)
    end
    addCustomTick(scopeTemp)
    
    return timedMonsterLineData
end

function checkTimedLineMonster(timedMonsterLineData)
    if timedMonsterLineData.maxLineCount < timedMonsterLineData.curLineCount and timedMonsterLineData.spawnCount == 0 then
        return
    end
    timedMonsterLineData.randomMonsterWaitTime = timedMonsterLineData.randomMonsterWaitTime - dt
    if timedMonsterLineData.randomMonsterWaitTime < 0.0 then

        if timedMonsterLineData.spawnCount <= 0 then
            timedMonsterLineData.spawnCount = timedMonsterLineData.monsterPerLine
            timedMonsterLineData.spawnDir = Vec2.randDir()
            timedMonsterLineData.spawnPos = nil
            timedMonsterLineData.randomMonsterTimer = timedMonsterLineData.randomMonsterTimerInterval
            timedMonsterLineData.curLineCount = timedMonsterLineData.curLineCount + 1
        end
    end
    
    if timedMonsterLineData.spawnCount > 0 then
        timedMonsterLineData.randomMonsterTimer = timedMonsterLineData.randomMonsterTimer - dt
        if timedMonsterLineData.randomMonsterTimer < 0.0 then
            timedMonsterLineData.randomMonsterTimer = timedMonsterLineData.randomMonsterTimer + timedMonsterLineData.randomMonsterTimerInterval
            timedMonsterLineData.spawnCount = timedMonsterLineData.spawnCount - 1
            if timedMonsterLineData.spawnCount <= 0 then
                timedMonsterLineData.randomMonsterWaitTime = timedMonsterLineData.randomMonsterTimerWaitTimeInterval
            end
            local monster = addMonsterWithParticles(timedMonsterLineData)
            if timedMonsterLineData.spawnPos == nil or isGoodSpawnPos(timedMonsterLineData.spawnPos) == false then
                randomizeMonsterPosAroundPlayer(monster)
                timedMonsterLineData.spawnPos = monster.position
            else
                monster.position = timedMonsterLineData.spawnPos
            end
            
            timedMonsterLineData.spawnPos = timedMonsterLineData.spawnPos + timedMonsterLineData.spawnDir * timedMonsterLineData.lineDist
            
            timedMonsterLineData.randomMonsterMax = timedMonsterLineData.randomMonsterMax - 1
            timedMonsterLineData.randomMonsterCount = timedMonsterLineData.randomMonsterCount + 1
            addCustomOnKill(monster, function (monster)
                timedMonsterLineData.randomMonsterCount = timedMonsterLineData.randomMonsterCount - 1
                if timedMonsterLineData.dontAddObjectiveForRandom ~= true then
                    missionData.curObjective = missionData.curObjective + 1
                end
            end)
        end
    end
end


function initTimedRandomMonster() 

    local timedMonsterData = {}

    timedMonsterData.monsterSpeed = 1.0
    timedMonsterData.hitPoint = 1.0
    timedMonsterData.randomMoveChance = 0.0
    timedMonsterData.damage = 1.0
    
    timedMonsterData.randomMonsterCur = 0
    timedMonsterData.randomMonsterMax = 60
    timedMonsterData.initialSpawn = 30
    timedMonsterData.maxRandomMonsterAtOnce = 30
    
    timedMonsterData.randomMonsterTimerInterval = 0.3
    timedMonsterData.randomMonsterCount = 0
    
    timedMonsterData.randomMonsterTimer = 0.0
    
    timedMonsterData.safeZone = 300
    
    timedMonsterData.getMaxMonster = function()
        return timedMonsterData.randomMonsterMax
    end
    
    function scopeTemp()
        checkTimedRandomMonster(timedMonsterData)
    end
    addCustomTick(scopeTemp)
    
    return timedMonsterData
end

function checkTimedRandomMonster(timedMonsterData)
    if timedMonsterData.randomMonsterMax <= timedMonsterData.randomMonsterCur then
        return
    end
    
    if timedMonsterData.initialSpawn ~= nil and timedMonsterData.initialSpawn > 0 then
        for i = 1, timedMonsterData.initialSpawn do
            addTimedRandomMonster(timedMonsterData)
        end
        timedMonsterData.initialSpawn = 0
    end
    
    if timedMonsterData.randomMonsterCount >= timedMonsterData.maxRandomMonsterAtOnce then
        timedMonsterData.randomMonsterTimer = timedMonsterData.randomMonsterTimerInterval * 3.0
    else
        timedMonsterData.randomMonsterTimer = timedMonsterData.randomMonsterTimer - dt
        if timedMonsterData.randomMonsterTimer < 0.0 then
            timedMonsterData.randomMonsterTimer = timedMonsterData.randomMonsterTimer + timedMonsterData.randomMonsterTimerInterval
            
            addTimedRandomMonster(timedMonsterData)
        end
    end
end

function addTimedRandomMonster(timedMonsterData)
    local monster = addMonsterWithParticles(timedMonsterData)
    monster.position = getRandomPosition( {notNearPlayer=true, notNearMonsters=true, playerRange=timedMonsterData.safeZone})
    
    timedMonsterData.randomMonsterCur = timedMonsterData.randomMonsterCur + 1
    timedMonsterData.randomMonsterCount = timedMonsterData.randomMonsterCount + 1
    addCustomOnKill(monster, function (monster)
        timedMonsterData.randomMonsterCount = timedMonsterData.randomMonsterCount - 1
        if timedMonsterData.dontAddObjectiveForRandom ~= true then
            missionData.curObjective = missionData.curObjective + 1
        end
    end)
end
