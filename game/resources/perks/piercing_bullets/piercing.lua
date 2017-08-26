
function PiercingBullets.init()
end

function PiercingBullets.onAddGunBullet(gun, bullet)
    if math.random() < 0.1 and bullet.penetrateCount < 5 then
        bullet.penetrateCount = 5
        local particle = bullet:addTrailParticle("RailGunParticle", Vec2.new(0.0, 0.0), 7.0, {})
    end
end
