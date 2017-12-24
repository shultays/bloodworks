PhoenixDive.buffId = getGlobalUniqueId()

function PhoenixDive.init(gun)
    gun.data.shooting = -1.0
    gun.data.gameObject = addGameObject("FlamethrowerObject")
    gun.data.gameObject:setLevel(RenderableLevel.player + 2)
    gun.data.particle = gun.data.gameObject:addParticle("FlameParticle", {})
    gun.data.started = false
end

function PhoenixDive.onTick(gun)
    local data = gun.data
    if gun.isTriggered and gun:hasAmmo() and data.shooting < 0.0 then
        gun:consumeAmmo()
        data.shooting = 0.6
        
        local buff = player.maxSpeed:addBuff(0.0)
        player.maxSpeed:setBuffDuration(buff, data.shooting)
        buff = player.maxRotateSpeed:addBuff(0.0)
        player.maxRotateSpeed:setBuffDuration(buff, data.shooting)
        buff = player.colorMultiplier:addBuff(Vec4.fromColor(0xFFFF8888))
        player.colorMultiplier:setBuffDuration(buff, data.shooting)
        player.colorMultiplier:setBuffFadeInFadeOut(buff, 0.1, 0.1)
        data.moveAngle = player.aimAngle
        
        data.particleTime = 0.0
        data.hitTime = 0.0
        data.started = true
        playSound({path = PhoenixDive.basePath .."dive.ogg", volume = 0.9})
        player:addKnockback(Vec2.fromAngle(data.moveAngle) * 500.0, data.shooting)
    end
    
    if data.shooting >= 0.0 then
        data.shooting = data.shooting - dt
        
    end
    
    if data.started then
        player.canFireNextFrame = false
        if data.shooting < 0.0 then
            data.started = false
            addExplosion(player.position, 160.0, 230.0, 250, 350, 0.1)
            playSound({path = "~/resources/sounds/explode.ogg"})
        end
        
        data.hitTime = data.hitTime - dt
        if data.hitTime < 0.0 then
            data.hitTime = data.hitTime + 0.02
            
            runForEachMonsterInRadius(player.position, 40.0, {}, function(monster)
                local args = {doNotStun = true}
                local toMonster = monster.position - player.position
                toMonster:normalize()
                local side = player.aimDir:sideVec()
                monster:doDamageWithArgs(8 + math.random(8), toMonster, args)
                if toMonster:dot(side) > 0.0 then
                    monster:addKnockback(side * 30.0, 0.35)
                else
                    monster:addKnockback(-side * 30.0, 0.35)
                end
                
                if monster.data.flamethrowerObject == nil then
                    monster.data.flamethrowerObject = addGameObject("BurnMonsterObject")
                    monster.data.flamethrowerObject.data.monster = monster
                    monster.data.flamethrowerObject.data.damageMin = 4
                    monster.data.flamethrowerObject.data.damageVar = 6
                end
                
                monster.data.flamethrowerObject.data.count = 4
            end)
        end
        
        data.particleTime = data.particleTime - dt
        while data.particleTime < 0.0 do
            data.particleTime = data.particleTime + 0.02
            for i = 1,16 do
                data.particle:addParticle(player.position + Vec2.randDir() * 20.0 * (math.random() - 0.5), {moveSpeed = Vec2.fromAngle(data.moveAngle + (math.random() - 0.5) * math.pi  ) * 100.0 })
            end
        end
        
    end
end

function PhoenixDive.onPlayerDamaged(gun, damage, dir, params)
    if gun.data.started then
        return -1
    end
    return damage
end

function PhoenixDive.spawnChance(gun)
    return ultimateSpawnChance()
end
