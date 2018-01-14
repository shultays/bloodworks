
function BumerangGun.init(gun)
    gun.spreadAngle = 0.0
    gun.crosshairDistance = 750.0
    ShootTimer.initGun(gun, 0.9)
    gun.data.maxSpread = 0.25
    gun.data.spreadDecreaseStartTime = 0.35
    gun.data.spreadDecreaseSpeed = 0.80
    gun.data.spreadIncreasePerShoot = 0.03
    
    gun.data.lastShootTimeForDouble = time
end


function BumerangGun.onTick(gun)
    if gun.isTriggered and gun:hasAmmo() then
        local gunReady = ShootTimer.checkGun(gun)
        if time - gun.data.lastShootTimeForDouble > 0.15 or gunReady then
            if gunReady then
                gun.data.lastShootTimeForDouble = time
            else
                gun.data.lastShootTimeForDouble = time + 100
            end
            gun:consumeAmmo()
            local bullet = gun:addBullet()
            
            local distance = math.max(250, player.crosshairDistance)
            distance = math.min(550, distance)
            bullet.data.movePos = player.position + player.aimDir * distance
            bullet.data.startTime = time
            bullet.data.startPos = bullet.position:copy()
            bullet.data.startAngle = player.aimAngle
            bullet.data.sideDir = player.aimDir * 100.0
            bullet.data.sideDir:rotateBy(math.pi * 0.5)
            bullet.meshRotation = 0.0
            bullet.data.speedMult = -clamp(distance / 250) * 0.3 + 1.2
            bullet.penetrateCount = 2
        end
    end
end

function BumerangGun.onBulletTick(gun, bullet)
    local movePos = bullet.data.movePos
    local dur = 1.0 * bullet.data.speedMult
    local timeDiff = (time - bullet.data.startTime) * dur
    
    if bullet.data.returns then
        movePos = lerp(movePos, player.position, timeDiff)
        bullet.data.startAngle = (player.position - bullet.data.startPos) : getAngle()
    end

    
    local center = (bullet.data.startPos + movePos) * 0.5
    local d = bullet.data.startPos:distance(movePos) * 0.5
    local angle = (-0.5 - timeDiff) * math.pi
    local shift = Vec2:new(math.sin(angle) * d, math.cos(angle) * d * 0.3)
    shift:rotateBy(-bullet.data.startAngle)
    bullet:setPosition(center + shift)

    bullet.meshRotation = bullet.meshRotation + dt * 10
    if timeDiff > 1.0 then
        if  bullet.data.returns == nil then
            bullet.data.startPos = bullet.data.movePos
            bullet.data.movePos = player.position
            bullet.data.startTime = time
            bullet.data.startAngle = (player.position - bullet.data.startPos) : getAngle()
            bullet.data.returns = true
        else
            bullet:removeSelf()
        end
    end
end
