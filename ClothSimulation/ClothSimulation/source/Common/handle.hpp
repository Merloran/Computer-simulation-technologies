#pragma once

template<typename Type>
struct Handle
{
	Int32 id;

	static const Handle<Type> sNone;
	inline bool operator==(const Handle<Type>& other) const
	{
		return id == other.id;
	}

	inline bool operator!=(const Handle<Type>& other) const
	{
		return id != other.id;
	}
};
