
function Monster.init()
	local monster = monsters[monsterId]

	monster.x = math.random() * 800 - 400;
	monster.y = math.random() * 600 - 300;
	
	monster.scale = math.random() * 0.4 + 0.5

end

function Monster.onTick()
	local monster = monsters[monsterId]

	local vPlayer = vector(player.x, player.y)
	local vMonster = vector(monster.x, monster.y)
	local diff = vPlayer - vMonster;

	monster.moveAngle = diff:angle();
	monster.moveSpeed = 20;
end

