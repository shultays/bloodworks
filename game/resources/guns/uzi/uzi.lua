
function Uzi.init(gun)
    gun.spreadAngle = 0.15
    gun.crosshairDistance = 550.0
    ShootTimer.initGun(gun, 0.04)
end


function Uzi.onTick(gun)
    if gun.isTriggered and gun:hasAmmo() then
        if ShootTimer.checkGun(gun) then
            gun:consumeAmmo()
            local bullet = gun:addBullet()
            bullet.onDamageArgs.customSlowdownAmount = 4.0
            bullet.onDamageArgs.slowdownOnly = true
            bullet.lifeTime = 0.5 + math.random() * 0.1
            
            bullet:addTrailParticle("BulletTrailParticle", Vec2.new(0.0, 10.0), 15.0, { color =  Vec3.new(0.9, 0.8, 0.3), initialScale = 1.5, fadeOutSpeed = 1.8})
        end
    end
end
