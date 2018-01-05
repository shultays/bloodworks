
HomingOrb.homingOrbId = getGlobalUniqueId()
HomingOrb.homingOrbIdInitial = getGlobalUniqueId()

function HomingOrb.spawn(bonus, pos)
    local monster = getClosestMonsterWithIgnoreId(pos, {HomingOrb.homingOrbIdInitial})
    if monster ~= nil then
        monster:addIgnoreId(HomingOrb.homingOrbIdInitial)
        local bullet = addCustomBullet()
        bullet.damage = math.floor(math.random() * 30.0 + 30)
        bullet.position = Vec2.new(pos.x, pos.y)
        bullet.moveSpeed = 300.0
        bullet.moveAngle = 0
        bullet:setScript(HomingOrbBullet)
        bullet:addRenderableTextureWithSize(HomingOrb.basePath .. "bullet.png", Vec2.new(14.0, 14.0))
        bullet.data.lastHitIndex = -1
        bullet.radius = 15.0
        bullet.hasCollision = false
        bullet.lifeTime = 10.0
        
        bullet.moveAngle = (monster.position - pos):getAngle()
        bullet.penetrateCount = math.floor(6 * player.data.bonusDurationMultiplier)
        bullet.damage = math.floor(math.random() * 70 + 60)
        bullet.data.monster = monster
        local particle = bullet:addTrailParticle("PlasmaTrailParticle", Vec2.new(0.0, 0.0), 20.0, { doNotShare = true })
        particle.args.color = Vec3.new(0.8, 0.7, 0.1)
        particle.args.initialScale = 10.0
        particle.args.initialAlpha = 0.4
        particle.args.scaleSpeed = -20
        particle:setTexture(HomingOrb.basePath .. "particle.png")
        
        playSound({path = HomingOrb.basePath .. "plasma.ogg"})
        
        return bullet
    end
end

HomingOrbBullet = {}

function HomingOrbBullet.onTick(bullet)
    if bullet.data.monster ~= nil then
        local toMonster = (bullet.data.monster.position - bullet.position)
        local m = 1.0 - clamp(toMonster:length() / 60)
        bullet.moveAngle = approachAngle(bullet.moveAngle, toMonster:getAngle(), (10 + 90 * m) * dt)
        if bullet.data.monster.isDead or bullet.data.monster:hasIgnoreId(HomingOrb.homingOrbId) then
            bullet.data.monster = getClosestMonsterWithIgnoreId(bullet.position, {HomingOrb.homingOrbIdInitial, HomingOrb.homingOrbId, bullet.id})
        end
    end
    return false
end

function HomingOrbBullet.onHit(bullet, monster)
    playSound({path = HomingOrb.basePath .. "plasma_hit.ogg", position = bullet.position})
    if monster ~= nil then
        addExplosion(bullet.position, 60.0, 100.0, 20, 30, 0.1)
        monster:addIgnoreId(HomingOrb.homingOrbId)
        bullet.data.monster = getClosestMonsterWithIgnoreId(bullet.position, {HomingOrb.homingOrbIdInitial, HomingOrb.homingOrbId, bullet.id, monster.id})
    else
        bullet.data.monster = getClosestMonsterWithIgnoreId(bullet.position, {HomingOrb.homingOrbIdInitial, HomingOrb.homingOrbId, bullet.id})
    end
    bullet.damage = math.floor(math.random() * 70 + 60)
end

