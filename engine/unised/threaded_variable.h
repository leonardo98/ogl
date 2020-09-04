#pragma once

#ifndef THREADED_VARIABLE_H
#define THREADED_VARIABLE_H

#include <mutex>

template <class T>
class ThreadedVariable
{
public:
	// методы вызываются из любого потока
	const T& Get() const
	{
		std::lock_guard<std::mutex> lock(_mutex);
		return _value;
	}
	
	void Set(const T& value)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_value = value;
		_dirty = true;
	}
	
	bool IsDirty() const
	{
		if (_mutex.try_lock())
		{
			bool v = (_dirty != Dirty::None);
			_mutex.unlock();
			return v;
		}
		return true;
	}
	
	void ResetDirty()
	{
		if (_mutex.try_lock())
		{
			if (_dirt == Dirty::Value)
			{
				_rawValue = _value;
			}
			else if (_dirt == Dirty::RawValue)
			{
				_value = _rawValue;
			}
			_dirt = Dirty::None;
			_mutex.unlock();
		}
	}

	// методы вызываются только из главного потока
	ThreadedVariable(T value)
		: _value(value)
		, _rawValue(value)
		, _dirty(Dirty::None)
	{}

	const T& GetRaw() const
	{
		return _rawValue;
	}
	
	void SetRaw(const T& value)
	{
		_rawValue = value;
		if (_mutex.try_lock())
		{
			_dirty = Dirty::RawValue;
			_mutex.unlock();
		}
	}

private:
	T _value;
	T _rawValue;
	mutable std::mutex _mutex;
	enum class Dirty
	{
		None,
		Value,
		RawValue
	};
	Dirty _dirty;
};

#endif//THREADED_VARIABLE_H
