
function DoubleShoot.init()
end

function DoubleShoot.onAddGunBullet(gun, bullet)
    if gun.data.timeToNextShoot ~= nil and player.data.lastDoubleShoot + 0.5 < time and math.random() < 0.25 and bullet.data.cannotBeBuffed ~= true and time > player.data.lastDoubleShootCheck then
        gun.data.timeToNextShoot = math.min(time + 0.15, gun.data.timeToNextShoot)
        player.data.lastDoubleShoot = time
    end
    player.data.lastDoubleShootCheck = time
end

function DoubleShoot.reset()
    player.data.lastDoubleShoot = time
    player.data.lastDoubleShootCheck = time
end
