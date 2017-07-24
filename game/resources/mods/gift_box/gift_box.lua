
function GiftBox.init(monster, min)
	data = monster.data
	monster:setScale(math.random() * 0.2 + 0.8)

	monster.moveSpeed = 0
	monster.moveAngle =  math.random() * math.pi * 2.0
	
	monster:playAnimation("stand", math.random())
	
	monster:modifyDrawLevel(3)
	
	data.cannotBecomeBoss = true
	data.cannotShootBullets = true

	monster.knockbackResistance:addBuff(0.0)
end

