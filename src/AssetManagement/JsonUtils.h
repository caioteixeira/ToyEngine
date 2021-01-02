#pragma once
#include "document.h"
#include <string>
#include "Math.h"

inline bool GetStringFromJSON(const rapidjson::Value& inObject, const char* inProperty, std::string& outStr)
{
    const auto itr = inObject.FindMember(inProperty);
	if (itr == inObject.MemberEnd())
	{
		return false;
	}

	auto& property = itr->value;
	if (!property.IsString())
	{
		return false;
	}

	outStr = property.GetString();
	return true;
}

inline bool GetVectorFromJSON(const rapidjson::Value& inObject, const char* inProperty, Vector3& outVector)
{
    const auto itr = inObject.FindMember(inProperty);
	if (itr == inObject.MemberEnd())
	{
		return false;
	}

	auto& property = itr->value;
	if (!property.IsArray() || property.Size() != 3)
	{
		return false;
	}

	for (rapidjson::SizeType i = 0; i < 3; i++)
	{
		if (!property[i].IsDouble())
		{
			return false;
		}
	}

	outVector.x = property[0].GetDouble();
	outVector.y = property[1].GetDouble();
	outVector.z = property[2].GetDouble();

	return true;
}

inline bool GetFloatFromJSON(const rapidjson::Value& inObject, const char* inProperty, float& outFloat)
{
    auto itr = inObject.FindMember(inProperty);
    if (itr == inObject.MemberEnd())
    {
        return false;
    }

    auto& property = itr->value;
    if (!property.IsDouble())
    {
        return false;
    }

    outFloat = property.GetDouble();
    return true;
}

inline bool GetIntFromJSON(const rapidjson::Value& inObject, const char* inProperty, int& outInt)
{
    auto itr = inObject.FindMember(inProperty);
    if (itr == inObject.MemberEnd())
    {
        return false;
    }

    auto& property = itr->value;
    if (!property.IsInt())
    {
        return false;
    }

    outInt = property.GetInt();
    return true;
}