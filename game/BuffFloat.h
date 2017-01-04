#pragma once

#include "cGlobals.h"
#include <unordered_map>

class BuffFloat
{
	float baseValue;
	float finalValue;

	float totalAddAfterMultiplyBuff;
	float totalMultiplyBuff;
	float totalAddBeforeMultiplyBuff;

	enum BuffType
	{
		multiply_buff,
		add_after_multiply_buff,
		add_before_multiply_buff,
	};

	struct BuffInfo 
	{
		int buffType;
		float buffAmount;
		float startTime;

		float duration;
		float fadeInDuration;
		float fadeOutDuration;
	};
	std::unordered_map<int, BuffInfo> buffs;
	bool isDirty;
	bool isDirtyForTick; // this should be a timestamp
public:
	BuffFloat()
	{
		baseValue = finalValue = 1.0f;
		reset();
	}

	BuffFloat(float baseValue)
	{
		this->baseValue = finalValue = baseValue;
		reset();
	}

	void setBaseValue(float baseValue)
	{
		this->baseValue = baseValue;
	}

	void addBuff(int id, float amount, int buffType)
	{
		BuffInfo buff;
		buff.buffAmount = amount;
		buff.buffType = buffType;
		buff.startTime = timer.getTime();
		buff.duration = -1.0f;
		buff.fadeInDuration = -1.0f;
		buff.fadeOutDuration = -1.0f;
		buffs[id] = buff;
		isDirty = true;
	}


	void setBuffAmount(int id, float amount)
	{
		buffs.at(id).buffAmount = amount;
		isDirty = true;
	}
	void setBuffDuration(int id, float duration)
	{
		buffs.at(id).duration = duration;
		isDirty = true;
		isDirtyForTick = duration >= 0.0f;
	}
	void setBuffFadeInFadeOut(int id, float fadeIn, float fadeOut)
	{
		auto& buff = buffs.at(id);
		buff.fadeInDuration = fadeIn;
		buff.fadeOutDuration = fadeOut;
		isDirty = true;
		isDirtyForTick = (fadeIn >= 0.0f || fadeOut >= 0.0f);
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

private:

	void reset()
	{
		totalAddAfterMultiplyBuff = 0.0f;
		totalMultiplyBuff = 1.0f;
		totalAddBeforeMultiplyBuff = 0.0f;
		isDirty = false;
	}

	void build()
	{
		reset();

		for (auto& b : buffs)
		{
			auto& buff = b.second;
			if (buff.buffType == multiply_buff)
			{
				totalMultiplyBuff *= buff.buffAmount;
			}
			else if (buff.buffType == add_after_multiply_buff)
			{
				totalAddAfterMultiplyBuff += buff.buffAmount;
			}
			else if (buff.buffType == add_before_multiply_buff)
			{
				totalAddBeforeMultiplyBuff += buff.buffAmount;
			}
		}

		finalValue = (baseValue + add_before_multiply_buff) * multiply_buff + add_after_multiply_buff;
		isDirty = false;
	}


};