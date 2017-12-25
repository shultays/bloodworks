
function PulseGun.init(gun)
    gun.spreadAngle = 0.0
    gun.crosshairDistance = 250.0
    ShootTimer.initGun(gun, 0.15)
end


function PulseGun.onTick(gun)
    if gun.isTriggered and gun:hasAmmo() then
        if ShootTimer.checkGun(gun) then
            gun:consumeAmmo()
            local bullet = gun:addBullet()
            bullet.data.t = 0.3
            bullet:setScale(bullet.data.t)
            bullet.penetrateCount = -1
            bullet.data.toKillTime = 1000.0
            bullet:setColor(Vec4.new(1.0, 1.0, 1.0, 0.4))
        end
    end
end


function PulseGun.onBulletTick(gun, bullet)
    local data = bullet.data
    data.t = data.t + dt * 10.5
    data.t = math.min(data.t, 1.5)
    bullet:setScale(data.t)
    
    data.toKillTime = data.toKillTime - dt
    if data.toKillTime < 0.0 then
        bullet:removeSelf()
    end
end

function PulseGun.onBulletHit(gun, bullet, monster)
    if monster ~= nil then
        monster:addKnockback(bullet.moveDir * 200.0, 0.06)
        if bullet.data.toKillTime > 0.05 then
            bullet.data.toKillTime = 0.05
        end
    end
end