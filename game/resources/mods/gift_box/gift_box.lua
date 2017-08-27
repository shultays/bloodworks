
function GiftBox.init(monster, min)
    data = monster.data
    monster:setScale(math.random() * 0.2 + 0.8)

    monster.moveSpeed = 0
    monster.moveAngle =  math.random() * math.pi * 2.0
    monster:playAnimation("stand", math.random())
    
    monster:modifyDrawLevel(3)
    
    data.cannotBecomeBoss = true
    data.cannotShootBullets = true
    
    data.angle = monster.moveAngle
    data.rotate = 0.2 + math.random() * 0.1
    data.shift = math.random() * math.pi

    data.particle = monster:addParticleSpawner("CriticalParticle", {});
    
    monster.knockbackResistance:addBuff(0.0)
    
    monster.dropChance = 0.0
    missionData.giftBoxTime = time
    data.particleTime = 0.0
end

function GiftBox.onMissionLoad(missionData)
    local m = addRandomMonster("GiftBox")
    
    local spawn
    local t = 10
    
    while t > 0 do
        t = t - 1
        local shift = Vec2.fromAngle(math.random() * math.pi) * 200.0
        spawn = player.position + shift
        if hasCircleCollision(spawn, 30.0) == false then
            break
        end
    end
    
    m.position = spawn
    m.data.spawnType = "gun"
    
    missionData.spawnWeaponOnFirstKill = false
end


function GiftBox.spawnChanceInMission(missionData, min)
    if time - missionData.giftBoxTime < 40.0 then
        return 0.00
    end
    return 0.05
end

function GiftBox.onTick(monster)
    local data = monster.data
    data.particleTime = data.particleTime - dt
    if data.particleTime < 0.0 then
        data.particleTime = data.particleTime + 0.01

        local s = Vec2:new()
        s:setAngle(math.random() * math.pi * 2.0)
        s = s * 20

        monster:spawnParticleShifted(s, data.particle, {initialScale = 15.0, moveSpeed = 150.0})
    end
    monster.moveAngle = data.angle + math.sin(time * 6.0 + data.shift) * data.rotate
end

function GiftBox.onKilled(monster)
    if monster.data.spawnType == "gun" then
        spawnRandomGun(monster.position)
    else
        spawnRandomBonus(monster.position)
    end
end