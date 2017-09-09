BlackHole.buffId = getGlobalUniqueId()

function BlackHole.init(gun)
    gun.data.shooting = -1.0
    gun.data.started = false
end

function BlackHole.onTick(gun)
    local data = gun.data
    local duration = 6.0 * player.data.bonusDurationMultiplier
    if gun.isTriggered and gun:hasAmmo() and data.started == false then
        gun:consumeAmmo()
        if gun.data.postprocess == nil then
            gun.data.postprocess = addPostProcess("resources/post_process/blackhole.ps")
        end
        data.shooting = 0.0
        data.started = true
        data.postprocess:setEnabled(true)
        data.ppos = player.position + player.aimDir * (player.crosshairDistance + 50.0)    
        data.bodyIndex = addCircleCollider(data.ppos, 1.0)
        setColliderFlags(data.bodyIndex, CollisionFlags.NoMonsterCollision + CollisionFlags.NoBulletCollision)
        
        data.bulletBodyIndex = addCircleCollider(data.ppos, 1.0)
        setColliderFlags(data.bulletBodyIndex, CollisionFlags.NoMonsterCollision + CollisionFlags.NoPlayerCollision)
        playSound({path = BlackHole.basePath .. "black_hole.ogg", position = data.ppos, volume = 1.2})
    end
    
    if data.started then
        data.shooting = data.shooting + dt
        if data.shooting > duration then
            data.started = false
            data.postprocess:setEnabled(false)
            removeCollider(data.bodyIndex)
            removeCollider(data.bulletBodyIndex)
        else
            local t = 1.0
            if data.shooting < 1.0  then
                t = data.shooting
            elseif data.shooting > duration - 1.0 then
                t = duration - data.shooting
            end
        
            data.postprocess:setShaderWeight(t)
            local v = convertToScreenPosition(data.ppos)
            gun.data.postprocess:addUniformVec2("uRelBlackHolePos", v)
            
            local killRange = 30 * t
            local moveRange = killRange + 130 * (0.1 + t * 0.9)
            
            local playerRadius = 40.0
            if moveRange > playerRadius then
                relocateCircleCollider(data.bodyIndex, data.ppos, moveRange - playerRadius)
            end
            local bulletRadius = 70.0
            if moveRange > bulletRadius then
                relocateCircleCollider(data.bulletBodyIndex, data.ppos, moveRange - bulletRadius)
            end
            
            runForEachMonsterInRadius( data.ppos, moveRange + 30.0, {}, function(monster)
                local d = monster.position:distance(data.ppos) - monster.bulletRadius
                if d < killRange and monster.canGetOneShooted then
                    monster:killSelf()
                elseif d < moveRange then
                    local m = (moveRange - d) * t / (moveRange - killRange)
                    local a = (monster.position - data.ppos):getAngle()
                    local toHole = -Vec2.fromAngle(a)
                    local toSide = -toHole:sideVec()
                    monster:addKnockback(toSide * m * 800.0 + toHole * m * 300.0, -1.0)
                end
            end)
        
            moveRange = moveRange + 30
            local d = player.position:distance(data.ppos)
            if d < moveRange then
                local m = (moveRange - d) * t / (moveRange - killRange)
                local a = (player.position - data.ppos):getAngle()
                local toHole = -Vec2.fromAngle(a)
                local toSide = -toHole:sideVec()
                player:addKnockback(toSide * m * 800.0 + toHole * m * 200.0, -1.0)
            end
        end
    end
end

