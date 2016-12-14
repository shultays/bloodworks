
function MachineGun.init(gun)
	gun.spreadAngle = 0.10
	gun.crosshairDistance = 550.0
	ShootTimer.InitGun(gun, 0.1)
end


function MachineGun.onTick(gun)
	if gun.isTriggered then
		if ShootTimer.CheckGun(gun) then
			local bullet = gun:addBullet()
			local particle = bullet:addTrailParticle("BulletTrailParticle", Vec2.new(0.0, 14.0), 15.0, {})
			particle.args.initialScale = 2.0
			particle.args.fadeOutSpeed = 1.2
		end
	end
end
