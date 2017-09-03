
function NarSie.init(monster, min)
    data = monster.data

    monster.moveSpeed = 0
    monster.moveAngle = 0.0
    monster.spriteAngle = math.pi * -0.5
    
    monster:playAnimation("stand")
    
    monster:setDrawLevel(RenderableLevel.player + 20)

    monster.knockbackResistance:addBuff(0.01)
    
    monster.canGetOneShooted = false
    monster.hasCollision = false
    
    data.inhaleTimer = 0.0
    data.currentAttack = nil
    data.currentAttackTimer = 0.0
    
    data.diff = 0.0
    
    NarSie.attacks = { NarSie.ringBullets }
end

function NarSie.spawnChanceInMission(missionData, min)
    return 0.0
end

function NarSie.onTick(monster)
    data = monster.data
    
    data.inhaleTimer = data.inhaleTimer - dt
    
    if data.inhaleTimer < 0.0 then
        data.inhaleTimer = 1.0 + math.random() * 1.0
        monster:playAnimation("inhale")
    end
    
    if data.currentAttack == nil then
        data.currentAttackTimer = data.currentAttackTimer - dt
        if data.currentAttackTimer < 0.0 then
            data.currentAttack = NarSie.attacks[ math.random( #NarSie.attacks ) ]
            data.currentAttack(monster, true)
            data.currentAttackTimer = 5.0
        end
    else 
        data.currentAttack(monster, false)
    end
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
