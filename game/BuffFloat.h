#pragma once

#include "cGlobals.h"
#include <unordered_map>

class BuffFloat
{
	friend class LuaBuffController;
	friend class BuffInfo;
	float baseValue;
	float finalValue;

public:
	enum BuffType
	{
		multiply_buff,
		add_after_multiply_buff,
		add_before_multiply_buff,
	};
	class BuffInfo 
	{
		friend class BloodworksLuaWorld;
		friend class BuffFloat;
		int buffType;
		float buffAmount;
		float startTime;

		float duration;
		float fadeInDuration;
		float fadeOutDuration;

		float buffEndTime;
		float fadeInEndTime;
		float fadeOutStartTime;

		BuffFloat *parent;
	public:


		void setBuffAmount(float amount)
		{
			buffAmount = amount;
			parent->isDirty = true;
		}

		void setBuffDuration(float duration)
		{
			duration = duration;
			buffEndTime = startTime + duration;
			fadeOutStartTime = buffEndTime - fadeOutDuration;
			parent->isDirty = true;
		}

		void setBuffFadeInFadeOut(float fadeIn, float fadeOut)
		{
			fadeInDuration = fadeIn;
			fadeOutDuration = fadeOut;

			fadeInEndTime = startTime + fadeInDuration;
			fadeOutStartTime = buffEndTime - fadeOutDuration;

			parent->isDirty = true;
		}
	};

private:
	std::unordered_map<int, BuffInfo> buffs;
	bool isDirty;
	bool isDirtyNextTick;
	float dirtyTickTimeStart;
	float dirtyTickTimeEnd;
	int buffControllerId;
public:
	BuffFloat()
	{
		buffControllerId = -1;
		baseValue = finalValue = 1.0f;
		reset();
	}

	BuffFloat(float baseValue)
	{
		buffControllerId = -1;
		this->baseValue = finalValue = baseValue;
		reset();
	}

	void setBaseValue(float baseValue)
	{
		this->baseValue = baseValue;
		isDirty = true;
	}

	void addBuff(int id, float amount, BuffType buffType)
	{
		auto& buff = buffs[id];
		buff.buffAmount = amount;
		buff.buffType = buffType;
		buff.startTime = timer.getTime();
		buff.duration = 1000000.0f;
		buff.fadeInDuration = -1.0f;
		buff.fadeOutDuration = -1.0f;
		buff.fadeInEndTime = buff.startTime + buff.fadeInDuration;
		buff.buffEndTime = buff.startTime + buff.duration;
		buff.fadeOutStartTime = buff.buffEndTime - buff.fadeOutDuration;
		buff.parent = this;
		isDirty = true;
	}

	void addBuffWithType(int id, float amount)
	{
		addBuff(id, amount, multiply_buff);
	}

	void setBuffAmount(int id, float amount)
	{
		buffs.at(id).setBuffAmount(amount);
	}

	void setBuffDuration(int id, float duration)
	{
		buffs.at(id).setBuffDuration(duration);
	}

	void setBuffFadeInFadeOut(int id, float fadeIn, float fadeOut)
	{
		buffs.at(id).setBuffFadeInFadeOut(fadeIn, fadeOut);
	}

	float getBuffedValue()
	{
		if (isDirty)
		{
			build();
		}
		return finalValue;
	}

	float getBaseValue()
	{
		return baseValue;
	}

	void tick()
	{
		float time = timer.getTime();

		if (isDirty || isDirtyNextTick || time < dirtyTickTimeEnd || time > dirtyTickTimeStart)
		{
			build();
		}
	}

	BuffInfo& getBuffInfo(int id)
	{
		return buffs.at(id);
	}

	void clear()
	{
		reset();
		buffs.clear();
	}
private:

	void reset()
	{
		dirtyTickTimeEnd = 0.0f;
		dirtyTickTimeStart = FLT_MAX;
		finalValue = baseValue;
		isDirty = false;
		isDirtyNextTick = false;
	}

	void build()
	{
		reset();
		if (buffs.size() == 0)
		{
			return;
		}

		float totalAddAfterMultiplyBuff = 0.0f;
		float totalMultiplyBuff = 1.0f;
		float totalAddBeforeMultiplyBuff = 0.0f;

		std::vector<int> toRemove;
		toRemove.reserve(10);
		float time = timer.getTime();
		for (auto& b : buffs)
		{
			auto& buff = b.second;
			if (time > buff.buffEndTime)
			{
				toRemove.push_back(b.first);
				continue;
			}

			dirtyTickTimeStart = min(dirtyTickTimeStart, buff.buffEndTime);
			dirtyTickTimeStart = min(dirtyTickTimeStart, buff.fadeOutStartTime);
			dirtyTickTimeEnd = max(dirtyTickTimeEnd, buff.fadeInEndTime);

			float buffValue = buff.buffAmount;
			float buffMultiplier = 1.0f;
			if (time < buff.fadeInEndTime)
			{
				buffMultiplier = (time - buff.startTime) / buff.fadeInDuration;
				isDirtyNextTick = true;
			}
			else if (time > buff.fadeOutStartTime)
			{
				buffMultiplier = (buff.buffEndTime - time) / buff.fadeOutDuration;
				isDirtyNextTick = true;
			}

			if (buff.buffType == multiply_buff)
			{
				totalMultiplyBuff *= lerp(1.0f, buff.buffAmount, buffMultiplier);
			}
			else if (buff.buffType == add_after_multiply_buff)
			{
				totalAddAfterMultiplyBuff += buff.buffAmount * buffMultiplier;
			}
			else if (buff.buffType == add_before_multiply_buff)
			{
				totalAddBeforeMultiplyBuff += buff.buffAmount * buffMultiplier;
			}
		}


		for (auto i : toRemove)
		{
			buffs.erase(i);
		}

		finalValue = (baseValue + totalAddBeforeMultiplyBuff) * totalMultiplyBuff + totalAddAfterMultiplyBuff;
		isDirty = false;
	}
};