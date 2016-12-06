
function MachineGun.init(gun)
	gun.spreadAngle = 0.10
	gun.crosshairDistance = 550.0
	ShootTimer.InitGun(gun, 0.1)
end


function MachineGun.onTick(gun)
	if gun.leftMouseDown then
		if ShootTimer.CheckGun(gun) then
			local bullet = gun:addBullet()
			bullet:addTrailParticle("BulletTrailParticle", Vec2.new(0.0, 0.0), 0.015, {})
		end
	end
end
