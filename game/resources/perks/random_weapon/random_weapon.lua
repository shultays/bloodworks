
function RandomWeapon.init(index)
    while true do
        local gun = getGunAtIndex(math.floor(math.random() * getGunCount()))
        if player.gun == nil or player.gun.name ~= gun.name then
            if gun.ultimate == false then
                player:setGun(gun)
                break
            end
        end
    end
end

