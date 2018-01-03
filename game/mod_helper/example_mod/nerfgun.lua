
function NerfGun.init(gun)
    gun.spreadAngle = 0.0
    gun.crosshairDistance = 350.0
    ShootTimer.initGun(gun, 0.7)
end


function NerfGun.onTick(gun)
    if gun.isTriggered and gun:hasAmmo() then
        if ShootTimer.checkGun(gun) then
            gun:consumeAmmo()
            local bullet = gun:addBullet()
            bullet.data.randAngle = 0.0
            bullet.data.randShift = 0.0
            bullet.data.totalShift = 0.0
        end
    end
end

function NerfGun.onBulletTick(gun, bullet)
    local data = bullet.data
    if data.randShift > 1.0 then
        data.randShift = data.randShift - dt
        data.randAngle = data.randAngle + dt * 0.5
    elseif data.randShift < -1.0 then
        data.randShift = data.randShift + dt
        data.randAngle = data.randAngle - dt * 0.5
    else
        data.randShift = math.random() * 0.1 + 1.0
        
        if data.totalShift > 0.002 or (data.totalShift > -0.002 and math.random() < 0.5) then
            data.randShift = -data.randShift
        end
    end
    data.totalShift = data.totalShift + data.randAngle * dt
    local v = Vec2.fromAngle(bullet.moveAngle + math.pi * 0.5) * (1000.0 * data.randAngle * dt)
    bullet.position = bullet.position + v
end
