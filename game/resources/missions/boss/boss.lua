
function NarSieFight.init()
    local theSeed = os.time()
    math.randomseed( theSeed )
    missionData = {}
    debugInit(missionData)
    missionInit(missionData)
    missionData.perkPerLevel = 4
    missionData.startTime = time
    missionData.firstTick = true
    missionData.levelUpBonusChance = 1.0
    
    addRandomColliders(35, 400.0)
end

function NarSieFight.onTick()
    min = missionTime / 60.0 + missionData.extraMin

    if player.isDead then
        if isKeyPressed(keys.Space) then
            loadMission("NarSieFight")
        end
        
        return
    end

    if missionData.firstTick == true then
        local m = addRandomMonster("NarSie")
        missionData.narsie = m
        
        m.position = player.position + Vec2.new(0.0, 200.0)
        
        missionData.health = addGameObject("HealthBar")
        
        missionData.maxHitPoint = m.hitPoint
        missionData.spawnTimer = 0.0
        calcRandomSpawns()
        for i = 1, 50 do
            NarSieFight.addMonster()
        end
        
        local guns = {}
        for i= -2,2 do
            while true do
                local g = getRandomGun()
                if guns[g.name] ~= true then
                    guns[g.name]  = true
                    spawnGun(player.position + Vec2.fromAngle(-math.pi * 0.5 + 0.7 * i) * 170.0, g.name)
                    break
                end
            end
        end
    end
    
    if missionData.narsie ~= nil then
        missionData.spawnTimer = missionData.spawnTimer - dt
        
        if missionData.spawnTimer < 0.0 then
            missionData.spawnTimer = 2.0
            NarSieFight.addMonster()
        end
        
        HealthBar.updateBar(missionData.health, missionData.narsie.hitPoint / missionData.maxHitPoint)
        missionData.firstTick = false
    end
    gameRestTick()
end

function NarSieFight.onNarSieKilled(monster)
    missionData.health.toBeRemoved = true
    missionData.health = nil
    missionData.narsie = nil
    
    local count = getMonsterCount()
    for i = 0, count - 1 do
        local monster = getMonsterAtIndex(i)
        monster.experienceMultiplier = 0.0
        monster.scoreMultiplier = 0.0
        monster:doDamage(10000, Vec2.randDir())
    end
    
    showGameReset("You're\nWinner !")
end


function NarSieFight.addMonster()
    local pos = getRandomPosition( {canBeEdge=true, notNearPlayer=true, notNearMonsters=true, playerRange=800.0})
    local monster = addRandomMonster(nil, true, true)
    
    monster.data.shootsBullets = false
    monster.position = pos
    monster.moveAngle =  math.random() * math.pi * 2.0
    monster.data.randomMove = true
    monster.data.maxMoveSpeed = monster.data.maxMoveSpeed * 0.5
    monster:setScale(0.7)
    monster.data.playerSeeRange = monster.data.playerSeeRange * 0.3
    monster.colorMultiplier:addBuff(Vec4.new(0.5, 0.5, 0.5, 1.0))
end
function NarSieFight.onPlayerDied()
    showGameReset()
end

function NarSieFight.onMonsterDied(monster)
end

function NarSieFight.onDebugTick()
    debugTick(missionData)
end


HealthBar = {}

function HealthBar.init(gameObject)
    gameObject:setLevel(RenderableLevel.GUI + 5)
    
    gameObject.data.barBG = gameObject:addTexture(missionPath .. "bar_bg.png", "~/resources/default")
    gameObject.data.barBG:setAlignment(RenderableAlignment.bottom)
    gameObject.data.barBG:setWorldMatrix(Mat3.fromScale(286.0, 24.0))
    
    
    gameObject.data.bar = gameObject:addTexture(missionPath .. "bar.png", missionPath .. "bar")
    gameObject.data.bar:setAlignment(RenderableAlignment.bottom)
    gameObject.data.bar:setWorldMatrix(Mat3.fromScale(286.0, 24.0))
    gameObject.data.bar:setColor(Vec4.fromColor(0x44FFFFFF))
    
    gameObject.data.barFG = gameObject:addTexture(missionPath .. "bar_fg.png", "~/resources/default")
    gameObject.data.barFG:setAlignment(RenderableAlignment.bottom)
    gameObject.data.barFG:setWorldMatrix(Mat3.fromScale(300.0, 40.0))
    
    gameObject:setPosition(Vec2.new(0, 40))
    
    HealthBar.updateBar(gameObject, 1.0)
    HealthBar.repositionGUI(gameObject)
end

function HealthBar.repositionGUI(gameObject)
    gameObject:setPosition(Vec2.new(0, 60 / getCameraZoom()))
    gameObject:setScale(Vec2.new(0.5 / getCameraZoom(), 0.5 / getCameraZoom()))
end


function HealthBar.updateBar(gameObject, scale)
    gameObject.data.bar:setUniformFloat("uScale", scale)
end