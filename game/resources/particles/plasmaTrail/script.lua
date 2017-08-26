

function PlasmaTrailParticle.initSystem()
    addAttribute("color", "vec3")
    addAttribute("initialScale", "float")
    addAttribute("scaleSpeed", "float")
    addAttribute("initialAlpha", "float")
    addAttribute("fadeOutSpeed", "float")
end


function PlasmaTrailParticle.setDefaultArgs(args)
    if args.color == nil then
        args.color = Vec3.new(1.0, 1.0, 1.0)
    end
    if args.initialScale == nil then
        args.initialScale = 6.0
    end
    if args.scaleSpeed == nil then
        args.scaleSpeed = -10.0 
    end
    if args.initialAlpha == nil then
        args.initialAlpha = 0.6
    end
    if args.fadeOutSpeed == nil then
        args.fadeOutSpeed = 2.0
    end
end

function PlasmaTrailParticle.addParticle(params, pos, args)
    params.pos = Vec2.new(pos.x, pos.y)
    
    params.color = args.color
    params.initialScale = args.initialScale
    params.scaleSpeed = args.scaleSpeed
    params.initialAlpha = args.initialAlpha
    params.fadeOutSpeed = args.fadeOutSpeed
end