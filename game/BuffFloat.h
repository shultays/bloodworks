#pragma once

#include "cGlobals.h"
#include <unordered_map>

template <typename T>
class BuffTemplate
{
	friend class LuaBuffController;
	friend class BuffInfo;
	T baseValue;
	T finalValue;

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
		friend class BuffTemplate;
		int buffType;
		T buffAmount;
		float startTime;

		float duration;
		float fadeInDuration;
		float fadeOutDuration;

		float buffEndTime;
		float fadeInEndTime;
		float fadeOutStartTime;

		BuffTemplate *parent;
		bool removeAfterEnds;
	public:

		void restart()
		{
			startTime = timer.getTime();
			setBuffDuration(duration);
			setBuffFadeInFadeOut(fadeInDuration, fadeOutDuration);
		}

		void setBuffAmount(T amount)
		{
			buffAmount = amount;
			parent->isDirty = parent->isChangedBeforeTick = true;
		}

		void setBuffDuration(float duration)
		{
			this->duration = duration;
			buffEndTime = startTime + duration;
			fadeOutStartTime = buffEndTime - fadeOutDuration;
			parent->isDirty = parent->isChangedBeforeTick = true;
		}

		void setBuffFadeInFadeOut(float fadeIn, float fadeOut)
		{
			this->fadeInDuration = fadeIn;
			this->fadeOutDuration = fadeOut;

			fadeInEndTime = startTime + fadeInDuration;
			fadeOutStartTime = buffEndTime - fadeOutDuration;

			parent->isDirty = parent->isChangedBeforeTick = true;
		}

		T getCurrentBuffAmount()
		{
			T buffValue = buffAmount;
			float buffMultiplier = 1.0f;
			float time = timer.getTime();
			if (time < fadeInEndTime)
			{
				buffMultiplier = (time - startTime) / fadeInDuration;
			}
			else if (time > fadeOutStartTime)
			{
				buffMultiplier = (buffEndTime - time) / fadeOutDuration;
			}

			if (buffType == multiply_buff)
			{
				return lerp(T(1.0f), buffAmount, buffMultiplier);
			}
			else if (buffType == add_after_multiply_buff)
			{
				return buffAmount * buffMultiplier;
			}
			else if (buffType == add_before_multiply_buff)
			{
				return buffAmount * buffMultiplier;
			}
			return T(1.0);
		}

	};

private:
	std::unordered_map<int, BuffInfo> buffs;
	bool isDirty;
	bool isDirtyNextTick;
	bool isChangedBeforeTick;

	float dirtyTickTimeStart;
	float dirtyTickTimeEnd;
	int buffControllerId;
public:
	BuffTemplate()
	{
		buffControllerId = -1;
		baseValue = finalValue = T(1.0f);
		isChangedBeforeTick = true;
		reset();
	}

	BuffTemplate(T baseValue)
	{
		buffControllerId = -1;
		this->baseValue = finalValue = baseValue;
		isChangedBeforeTick = true;
		reset();
	}

	void setBaseValue(T baseValue)
	{
		this->baseValue = baseValue;
		isChangedBeforeTick = isDirty = true;
	}

	void addBuff(int id, T amount, BuffType buffType)
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
		buff.removeAfterEnds = true;
		isChangedBeforeTick = isDirty = true;
	}

	void addBuffWithType(int id, T amount)
	{
		addBuff(id, amount, multiply_buff);
	}

	void setBuffAmount(int id, T amount)
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

	T getBuffedValue()
	{
		if (isDirty)
		{
			build();
		}
		return finalValue;
	}

	T getBaseValue()
	{
		return baseValue;
	}

	bool tick()
	{
		float time = timer.getTime();

		if (isDirty || isDirtyNextTick || time < dirtyTickTimeEnd || time > dirtyTickTimeStart)
		{
			return build();
		}
		bool r = isChangedBeforeTick;
		isChangedBeforeTick = false;
		return r;
	}

	bool hasBuffInfo(int id)
	{
		return buffs.count(id) > 0;
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

	bool build()
	{
		reset();
		if (buffs.size() == 0)
		{
			return false;
		}

		T totalAddAfterMultiplyBuff = T(0.0f);
		T totalMultiplyBuff = T(1.0f);
		T totalAddBeforeMultiplyBuff = T(0.0f);

		std::vector<int> toRemove;
		toRemove.reserve(10);
		float time = timer.getTime();
		for (auto& b : buffs)
		{
			auto& buff = b.second;
			if (time > buff.buffEndTime)
			{
				if (buff.removeAfterEnds)
				{
					toRemove.push_back(b.first);
				}
				continue;
			}

			dirtyTickTimeStart = min(dirtyTickTimeStart, buff.buffEndTime);
			dirtyTickTimeStart = min(dirtyTickTimeStart, buff.fadeOutStartTime);
			dirtyTickTimeEnd = max(dirtyTickTimeEnd, buff.fadeInEndTime);

			T buffAmount = buff.getCurrentBuffAmount();

			if (time < buff.fadeInEndTime || time > buff.fadeOutStartTime)
			{
				isDirtyNextTick = true;
			}

			if (buff.buffType == multiply_buff)
			{
				totalMultiplyBuff *= buffAmount;
			}
			else if (buff.buffType == add_after_multiply_buff)
			{
				totalAddAfterMultiplyBuff += buffAmount;
			}
			else if (buff.buffType == add_before_multiply_buff)
			{
				totalAddBeforeMultiplyBuff += buffAmount;
			}
		}

		for (auto i : toRemove)
		{
			buffs.erase(i);
		}

		finalValue = (baseValue + totalAddBeforeMultiplyBuff) * totalMultiplyBuff + totalAddAfterMultiplyBuff;
		isDirty = false;
		return true;
	}
};


using BuffFloat = BuffTemplate<float>;
using BuffVec2 = BuffTemplate<Vec2>;
using BuffVec3 = BuffTemplate<Vec3>;
using BuffVec4 = BuffTemplate<Vec4>;
