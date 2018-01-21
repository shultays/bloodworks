
local newMonsterAlphaColorID = getGlobalUniqueId()


function addMonsterWithParticles(spawnData)
    local monster = addMonster("Alien")

    monster.colorMultiplier:addBuffWithId(newMonsterAlphaColorID, Vec4:new(1.0, 1.0, 1.0, 0.0))
    local colorbuff = monster.colorMultiplier:getBuffInfo(newMonsterAlphaColorID)
    colorbuff:setBuffDuration(0.4)
    colorbuff:setBuffFadeInFadeOut(-1.0, 0.4)

    monster.moveSpeedMultiplier:addBuff(spawnData.monsterSpeed)
    
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
    
    monster.position = player.position + Vec2.randDir() * (t * (a - b) + b)
    
    monster.moveAngle = (player.position - monster.position):getAngle()
end

function initTimedMonster() 
    missionData.maxRandomMonsterAtOnce = 5.0
    missionData.randomMonsterTimer = 0.0
    missionData.randomMonsterTimerInterval = 0.7
    missionData.randomMonsterData = {}
    missionData.randomMonsterData.monsterSpeed = 0.7
    missionData.randomMonsterCount = 0
    
    missionData.randomMonsterWaiting = true
    missionData.randomMonsterWaitTime = 2.0
    
    missionData.randomMonsterMax = 20
    
    addCustomTick(checkTimedMonster)
end

function checkTimedMonster()
    if missionData.randomMonsterMax == 0 then
        return
    end
    missionData.randomMonsterWaitTime = missionData.randomMonsterWaitTime - dt
    if missionData.randomMonsterWaitTime < 0.0 then
        if missionData.randomMonsterWaiting then
            missionData.randomMonsterWaitTime = 4.0
            missionData.randomMonsterWaiting = false
        else
            missionData.randomMonsterWaitTime = 5.0
            missionData.randomMonsterWaiting = true
        end
    end
    if missionData.randomMonsterWaiting then
        return
    end
    if missionData.randomMonsterCount >= missionData.maxRandomMonsterAtOnce then
        missionData.randomMonsterTimer = missionData.randomMonsterTimerInterval * 3.0
    else
        missionData.randomMonsterTimer = missionData.randomMonsterTimer - dt
        if missionData.randomMonsterTimer < 0.0 then
            missionData.randomMonsterTimer = missionData.randomMonsterTimer + missionData.randomMonsterTimerInterval
            
            local monster = addMonsterWithParticles(missionData.randomMonsterData)
            
            missionData.randomMonsterMax = missionData.randomMonsterMax - 1
            randomizeMonsterPosAroundPlayer(monster)
            missionData.randomMonsterCount = missionData.randomMonsterCount + 1
            addCustomOnKill(monster, function (monster)
                missionData.randomMonsterCount = missionData.randomMonsterCount - 1
                if missionData.dontAddObjectiveForRandom ~= true then
                    missionData.curObjective = missionData.curObjective + 1
                end
            end)
        end
    end
end