
function Laser.init(gun)
    gun.spreadAngle = 0.0
    gun.crosshairDistance = 550.0
    gun.data.maxRangeUniform = gun.laser:addUniformFloat("maxRange", 500.0)
    ShootTimer.initGun(gun, 0.1)
end


function Laser.onTick(gun)
    gun.laser:setVisible(false)
    if gun.isTriggered and gun:hasAmmo() then
        gun.laser:setVisible(true)
        local range = 500.0
        local result = getClosestMonsterOnLine(player.gunPos, player.aimDir * range, 0.0, {gun = gun, ignoreFlags = CollisionFlags.NoBulletCollision})
        range = result.distance
        
        gun.laser:setLength(range)
        
        if ShootTimer.checkGun(gun) then
            gun:consumeAmmo()
            if result.monster ~= nil then
                local args = {doNotStun = true}
                result.monster:doDamageWithArgs(gun:getRandomDamage(), player.aimDir, args)
            end
        end
    end
end
