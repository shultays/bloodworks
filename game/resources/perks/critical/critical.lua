
function CriticalBullets.init()
end

function CriticalBullets.onAddGunBullet(gun, bullet)
    if math.random() < 0.125 then
        bullet.damage = bullet.damage * 4
        bullet.radius = bullet.radius * 2
        bullet:addTrailParticle("CriticalParticle", Vec2.new(0.0, 0.0), 30.0, {}):setLevel(RenderableLevel.bullets - 1)
        bullet.meshScale = bullet.meshScale * 1.5
        bullet:updateDrawable()
    end
end
