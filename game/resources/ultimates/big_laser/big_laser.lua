
function BigLaser.init(gun)
    gun.spreadAngle = 0.0
    gun.crosshairDistance = 550.0
    gun.data.maxRangeUniform = gun.laser:addUniformFloat("maxRange", 450.0)
    gun.data.timeUniform = gun.laser:addUniformFloat("time", 0.0)
    ShootTimer.initGun(gun, 0.05)
    gun.data.shooting = -1.0
end

function BigLaser.onTick(gun)
    gun.laser:setVisible(false)
    local data = gun.data
    if gun.isTriggered and gun:hasAmmo() and data.shooting < 0.0 then
        gun:consumeAmmo()
        data.shooting = 2.5
        
        local buff = player.maxRotateSpeed:addBuff(0.1)
        player.maxRotateSpeed:setBuffDuration(buff, data.shooting + 0.3)
        player.maxRotateSpeed:setBuffFadeInFadeOut(buff, 0.05, 0.3)
    end

    if data.shooting > 0.0 then
        data.shooting = data.shooting - dt
        gun.laser:setVisible(true)
        local range = 430.0
        local collision = getCollisionForRay(player.gunPos + player.aimDir * 25.0, player.aimDir * (range - 25.0), 15, CollisionFlags.NoBulletCollision)
        if collision >= 0.0 then
            range = collision + 20
        end
        gun.laser:setLength(range)
        gun.laser:addUniformFloat("time", time)
        if ShootTimer.checkGun(gun) then
            runForEachMonsterOnLine(player.gunPos, player.aimDir * (range), 15, {}, function(monster)
                local args = {doNotStun = true}
                monster:doDamageWithArgs(gun:getRandomDamage(), player.aimDir, args)
            end)
        end
    end
end
