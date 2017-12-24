
function ShrinkGun.init(gun)
    gun.spreadAngle = 0.0
    gun.crosshairDistance = 700.0
    ShootTimer.initGun(gun, 0.45)
    
    SpreadHelper.initGun(gun)
    gun.data.minSpread = 0.0
    gun.data.maxSpread = 0.1
    gun.data.spreadDecreaseSpeed = 0.25
    gun.data.spreadIncreasePerShoot = 0.025
end


function ShrinkGun.onTick(gun)
    SpreadHelper.onTick(gun)
    if gun.isTriggered and gun:hasAmmo() then
        if ShootTimer.checkGun(gun) then
            gun:consumeAmmo()
            SpreadHelper.onShoot(gun)
            local bullet = gun:addBullet()
            local particle = bullet:addTrailParticle("CriticalParticle", Vec2.new(0.0, 0.0), 15.0, {})
            local r = math.random()
            local g = math.random()
            local b = math.random()
            local ra = math.random()
            if ra < 0.333 then
                r = 1.0
            elseif ra < 0.666 then
                g = 1.0
            else
                b = 1.0
            end

            bullet:setColor(Vec4:new(r, g, b, 1.0))
        end
    end
end



function ShrinkGun.onBulletHit(gun, bullet, monster)
    if monster ~= nil and monster.canGetOneShooted == true then
        monster:setScale(monster.scale * 0.7)
        
        local particleCount = 10
        if monster.scale < 0.4 then
            particleCount = 20
            monster:killSelf()
            playSound({path = ShrinkGun.basePath .. "shrink_kill.ogg", volume = 0.6})
        else
        
            playSound({path = ShrinkGun.basePath .. "shrink_hit.ogg", volume = 0.3})
        end
        
        if monster.data.shrinkParticle == nil then
            monster.data.shrinkParticle = monster:addParticleSpawner("CriticalParticle", {});
        end
        for i=1,particleCount do
            monster:spawnParticle(monster.data.shrinkParticle, {initialScale = 15.0, moveSpeed = 150.0})
        end
    end
end