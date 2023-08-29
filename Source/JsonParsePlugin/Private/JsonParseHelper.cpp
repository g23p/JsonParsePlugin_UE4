// Fill out your copyright notice in the Description page of Project Settings.


#include "JsonParseHelper.h"
#include "Serialization/JsonSerializer.h"
#include "Engine.h"

void print(FString input)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *input);
}

bool UJsonParseHelper::ParseJson(const FString& JsonStr, FJsonValueContent & JsonValue)
{
	TSharedPtr<FJsonValue> InnerJsonValue;
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonStr);

	if (FJsonSerializer::Deserialize(JsonReader, InnerJsonValue))
	{
		JsonValue.Value = InnerJsonValue;
		JsonValue.ValueType = ConfirmJsonType(InnerJsonValue);
		JsonValue.Key = FString();

		return true;
	}
	return false;
}

TArray<FJsonValueContent> UJsonParseHelper::GetAllChildrenNode(const FJsonValueContent& JsonValue)
{
	TArray<FJsonValueContent> FoundJsonArray;
	
	if (JsonValue.ValueType == FJsonValueType::JVT_ARRAY)
	{
		for (TSharedPtr<FJsonValue> Element : JsonValue.Value->AsArray())
		{
			FJsonValueContent NewJsonValue;
			NewJsonValue.Key = FString();
			NewJsonValue.Value = Element;
			NewJsonValue.ValueType = ConfirmJsonType(Element);

			FoundJsonArray.Add(NewJsonValue);
		}
	}
	else if (JsonValue.ValueType == FJsonValueType::JVT_OBJECT)
	{
		for (TPair<FString, TSharedPtr<FJsonValue>> Element : JsonValue.Value->AsObject()->Values)
		{
			FJsonValueContent NewJsonValue;
			NewJsonValue.Key = Element.Key;
			NewJsonValue.Value = Element.Value;
			NewJsonValue.ValueType = ConfirmJsonType(Element.Value);

			FoundJsonArray.Add(NewJsonValue);
		}
	}
	return FoundJsonArray;
}

bool UJsonParseHelper::NodeAsBool(const FJsonValueContent& JsonValue, bool& bSucceed, FString& Key)
{
	bSucceed = false;
	Key = FString();
	if (/*JsonValue.Value->Type == EJson::Boolean && */JsonValue.ValueType == FJsonValueType::JVT_BOOL)
	{
		bSucceed = true;
		Key = JsonValue.Key;
		return JsonValue.Value->AsBool();
	}
	return false;
}

float UJsonParseHelper::NodeAsNumber(const FJsonValueContent& JsonValue, bool & bSucceed, FString& Key)
{
	bSucceed = false;
	Key = FString();
	if (/*JsonValue.Value->Type == EJson::Number && */JsonValue.ValueType == FJsonValueType::JVT_NUMBER)
	{
		bSucceed = true;
		Key = JsonValue.Key;
		return JsonValue.Value->AsNumber();
	}
	return 0.0f;
}

FString UJsonParseHelper::NodeAsString(const FJsonValueContent& JsonValue, bool & bSucceed, FString& Key)
{
	bSucceed = false;
	Key = FString();
	if (/*JsonValue.Value->Type == EJson::String && */JsonValue.ValueType == FJsonValueType::JVT_STRING)
	{
		bSucceed = true;
		Key = JsonValue.Key;
		return JsonValue.Value->AsString();
	}
	else if (JsonValue.ValueType == FJsonValueType::JVT_NUMBER)
	{
		bSucceed = true;
		Key = JsonValue.Key;
		return FString::SanitizeFloat(JsonValue.Value->AsNumber());
	}
	else if (JsonValue.ValueType == FJsonValueType::JVT_BOOL)
	{
		bSucceed = true;
		Key = JsonValue.Key;
		return JsonValue.Value->AsBool() ? TEXT("true") : TEXT("false");
	}

	return FString();
}

bool UJsonParseHelper::GetStringValue(const FJsonValueContent& JsonValue, const FString& Key, FString& FoundValue)
{
	if (JsonValue.ValueType == FJsonValueType::JVT_OBJECT)
	{
		for (TPair<FString, TSharedPtr<FJsonValue>> Element : JsonValue.Value->AsObject()->Values)
		{
			if (Element.Key == Key)
			{
				FString OutStr;
				bool OutBool;
				float OutNumber;
				if (Element.Value->TryGetString(OutStr))
				{
					FoundValue = OutStr;
					return true;
				}
				else if(Element.Value->TryGetNumber(OutNumber))
				{
					FoundValue = FString::SanitizeFloat(OutNumber);
					return true;
				}
				else if (Element.Value->TryGetBool(OutBool))
				{
					FoundValue = OutBool ? TEXT("true") : TEXT("false");
					return true;
				}
			}
		}
	}
	else if (JsonValue.ValueType == FJsonValueType::JVT_ARRAY)
	{
		if (JsonValue.Value->AsArray().Num() == 1 && JsonValue.Value->AsArray()[0]->Type == EJson::Object)
		{
			for (TPair<FString, TSharedPtr<FJsonValue>> Element : JsonValue.Value->AsArray()[0]->AsObject()->Values)
			{
				if (Element.Key == Key)
				{
					FString OutStr;
					bool OutBool;
					float OutNumber;
					if (Element.Value->TryGetString(OutStr))
					{
						FoundValue = OutStr;
						return true;
					}
					else if (Element.Value->TryGetNumber(OutNumber))
					{
						FoundValue = FString::SanitizeFloat(OutNumber);
						return true;
					}
					else if (Element.Value->TryGetBool(OutBool))
					{
						FoundValue = OutBool ? TEXT("true") : TEXT("false");
						return true;
					}
				}
			}
		}
	}
	
	return false;
}

bool UJsonParseHelper::GetBoolValue(const FJsonValueContent& JsonValue, const FString& Key, bool& FoundValue)
{
	if (JsonValue.ValueType == FJsonValueType::JVT_OBJECT)
	{
		for (TPair<FString, TSharedPtr<FJsonValue>> Element : JsonValue.Value->AsObject()->Values)
		{
			if (Element.Key == Key)
			{
				bool OutBool;
				if (Element.Value->TryGetBool(OutBool))
				{
					FoundValue = OutBool;
					return true;
				}
			}
		}
	}
	else if (JsonValue.ValueType == FJsonValueType::JVT_ARRAY)
	{
		if (JsonValue.Value->AsArray().Num() == 1 && JsonValue.Value->AsArray()[0]->Type == EJson::Object)
		{
			for (TPair<FString, TSharedPtr<FJsonValue>> Element : JsonValue.Value->AsArray()[0]->AsObject()->Values)
			{
				if (Element.Key == Key)
				{
					bool OutBool;
					if (Element.Value->TryGetBool(OutBool))
					{
						FoundValue = OutBool;
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool UJsonParseHelper::GetChildJsonNode(const FJsonValueContent& JsonValue, const FString& Key, FJsonValueContent& FoundValue)
{
	FJsonValueContent JsonValueContent;
	if (JsonValue.ValueType == FJsonValueType::JVT_OBJECT)
	{
		for (TPair<FString, TSharedPtr<FJsonValue>> Element : JsonValue.Value->AsObject()->Values)
		{
			if (Element.Key == Key)
			{
				JsonValueContent.Value = Element.Value;
				JsonValueContent.Key = Element.Key;
				JsonValueContent.ValueType = ConfirmJsonType(Element.Value);
				FoundValue = JsonValueContent;
				return true;
			}
		}
	}
	else if (JsonValue.ValueType == FJsonValueType::JVT_ARRAY)
	{
		if (JsonValue.Value->AsArray().Num() == 1 && JsonValue.Value->AsArray()[0]->Type == EJson::Object)
		{
			for (TPair<FString, TSharedPtr<FJsonValue>> Element : JsonValue.Value->AsArray()[0]->AsObject()->Values)
			{
				if (Element.Key == Key)
				{
					JsonValueContent.Value = Element.Value;
					JsonValueContent.Key = Element.Key;
					JsonValueContent.ValueType = ConfirmJsonType(Element.Value);
					FoundValue = JsonValueContent;
					return true;
				}
			}
		}
	}

	return false;
}

bool UJsonParseHelper::GetNumberValue(const FJsonValueContent& JsonValue, const FString& Key, int32& FoundValue)
{
	return false;
}

bool UJsonParseHelper::Generic_GetNumberValue(const FJsonValueContent& JsonValue, const FString& Key, void* ValueAddr, FProperty* ValueType)
{
#if ENGINE_MAJOR_VERSION == 4
	FFloatProperty* FloatProperty = CastField<FFloatProperty>(ValueType);
	float* OutPtr = FloatProperty->GetPropertyValuePtr(ValueAddr);
#elif ENGINE_MAJOR_VERSION == 5
	FDoubleProperty* DoubleProperty = CastField<FDoubleProperty>(ValueType);
	double* OutPtr = DoubleProperty->GetPropertyValuePtr(ValueAddr);
#endif

	if (JsonValue.ValueType == FJsonValueType::JVT_OBJECT)
	{
		double OutNumber;
		if (JsonValue.Value->AsObject()->TryGetNumberField(Key, OutNumber))
		{
			*OutPtr = OutNumber;
			return true;
		}
	}
	else if (JsonValue.ValueType == FJsonValueType::JVT_ARRAY)
	{
		if (JsonValue.Value->AsArray().Num() == 1 && JsonValue.Value->AsArray()[0]->Type == EJson::Object)
		{
			for (TPair<FString, TSharedPtr<FJsonValue>> Element : JsonValue.Value->AsArray()[0]->AsObject()->Values)
			{
				if (Element.Key == Key)
				{
					float OutNumber;
					if (Element.Value->TryGetNumber(OutNumber))
					{
						*OutPtr = OutNumber;
						return true;
					}
				}
			}
		}
	}

	return false;
}

FString UJsonParseHelper::JsonNodeToString(const FJsonValueContent& JsonValue)
{
	FString OutStr, WriterBuf;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&WriterBuf);
	switch (JsonValue.ValueType)
	{
	case FJsonValueType::JVT_BOOL:
	case FJsonValueType::JVT_NUMBER:
	case FJsonValueType::JVT_STRING:
		OutStr = TEXT("\"") + JsonValue.Key + TEXT("\":") + NodeToString(JsonValue);
		break;
	case FJsonValueType::JVT_ARRAY:
		FJsonSerializer::Serialize(JsonValue.Value->AsArray(), Writer);
		OutStr = WriterBuf;
		break;
	case FJsonValueType::JVT_OBJECT:
		FJsonSerializer::Serialize(JsonValue.Value->AsObject().ToSharedRef(), Writer);
		OutStr = WriterBuf;
		break;
	default:
		break;
	}
	Writer->Close();
	return OutStr;
}

FJsonValueType UJsonParseHelper::ConfirmJsonType(TSharedPtr<FJsonValue> JsonValue)
{
	FJsonValueType Type = FJsonValueType::JVT_NONE;
	switch (JsonValue->Type)
	{
	case EJson::Null:
		Type = FJsonValueType::JVT_NULL;
		break;
	case EJson::Array:
		Type = FJsonValueType::JVT_ARRAY;
		break;
	case EJson::Object:
		Type = FJsonValueType::JVT_OBJECT;
		break;
	case EJson::Boolean:
		Type = FJsonValueType::JVT_BOOL;
		break;
	case EJson::Number:
		Type = FJsonValueType::JVT_NUMBER;
		break;
	case EJson::String:
		Type = FJsonValueType::JVT_STRING;
		break;
	case EJson::None:
		Type = FJsonValueType::JVT_NONE;
		break;
	}
	return Type;
}

FString UJsonParseHelper::NodeToString(FJsonValueContent JsonValue)
{
	switch (JsonValue.ValueType)
	{
	case FJsonValueType::JVT_BOOL:
		return JsonValue.Value->AsBool() ? TEXT("true") : TEXT("false");
	case FJsonValueType::JVT_NUMBER:
		//return FString::FromInt(JsonValue.Value->AsNumber());
		return FString::SanitizeFloat(JsonValue.Value->AsNumber());
	case FJsonValueType::JVT_STRING:
		return TEXT("\"") + JsonValue.Value->AsString() + TEXT("\"");
	default:
		return FString();
	}
}