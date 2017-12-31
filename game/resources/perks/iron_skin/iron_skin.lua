
function IronSkin.init(level)
    IronSkin.level = 6 + (level - 1) * 3
end

function IronSkin.onPlayerDamaged(damage, dir, params)
    if damage <= 0 then
        return damage
    end
    if params.doNotReduce ~= true and damage > 0 then
        return damage - IronSkin.level
    end
    return damage
end
