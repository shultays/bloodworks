

function BulletTrailParticle.initSystem()
    addAttribute("color", "vec3")
    addAttribute("rotation", "float")
    addAttribute("initialScale", "float")
    addAttribute("scaleSpeed", "float")
    addAttribute("initialAlpha", "float")
    addAttribute("fadeOutSpeed", "float")
    addAttribute("startFadeinSpeed", "float")
    
    addUniform("lastBulletTime", "float")
end


function BulletTrailParticle.setDefaultArgs(args)
    if args.color == nil then
        args.color = Vec3.new(0.8, 0.6, 0.3)
    end
    if args.scaleSpeed == nil then
        args.scaleSpeed = 5.0
    end
    if args.initialAlpha == nil then
        args.initialAlpha = 0.4
    end
    if args.fadeOutSpeed == nil then
        args.fadeOutSpeed = 2.0
    end
    if args.startFadeinSpeed == nil then
        args.startFadeinSpeed = 30.0
    end
end

function BulletTrailParticle.addParticle(params, pos, args)
    params.pos = Vec2.new(pos.x, pos.y)
    
    params.color = args.color
    
    params.initialScale = args.initialScale
    
    params.scaleSpeed = args.scaleSpeed
    
    params.initialAlpha = args.initialAlpha
    
    params.fadeOutSpeed = args.fadeOutSpeed
    
    params.startFadeinSpeed = args.startFadeinSpeed
    
    params.rotation = params.bullet.moveAngle
    
    params.lastBulletTime = time - params.particleBeginTime
end