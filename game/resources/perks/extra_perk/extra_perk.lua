
function ExtraPerk.init(level)
    if missionData.perkPerLevel == nil then
        missionData.perkPerLevel = 4
    else
        missionData.perkPerLevel = missionData.perkPerLevel + 1
    end
end

