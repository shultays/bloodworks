
function RainbowGun.init(gun)
    ShootTimer.initGun(gun, 0.55)
    
    gun.data.shootTime = 0.0
    gun.data.shootSpeed =  Vec2.new(0.0, 0.0)
    
end


function RainbowGun.onTick(gun)
    gun.data.shootTime = gun.data.shootTime - dt
    gun.stripLaser:setShooting(gun.data.shootTime > 0.0)
    gun.stripLaser:setSpeed( gun.data.shootSpeed )
    if gun.isTriggered and gun:hasAmmo() then
        if ShootTimer.checkGun(gun) then
            gun.data.shootTime = 0.11
            gun.data.shootSpeed = player.aimDir * gun.bulletSpeed * player.bulletSpeedMultiplier:getBuffedValue()
            gun:consumeAmmo()
            local bullet = gun:addBullet()
            bullet.penetrateCount = -1
            bullet.hasCollision = false
            bullet.hitsMultipleTimes = true
        end
    end
end

 function RainbowGun.onBulletHit(gun, bullet, monster)
    if monster ~= nil then
        local toMonster = monster.position - bullet.position
        local d = 40 - toMonster:normalize() 
        if d < 2.0 then
            d = 2.0
        end
        monster:addKnockback(toMonster * d * 2, 0.1)
    end
 end
