// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Runtime/Json/Public/Json.h"
#include "JsonParseHelper.generated.h"


UENUM(BlueprintType)
enum class FJsonValueType : uint8
{
	JVT_NONE UMETA(DisplayName = "None"),
	JVT_NULL UMETA(DisplayName = "Null"),
	JVT_BOOL UMETA(DisplayName = "Bool"),
	JVT_NUMBER UMETA(DisplayName = "Number"),
	JVT_STRING UMETA(DisplayName = "String"),
	JVT_ARRAY UMETA(DisplayName = "Array"),
	JVT_OBJECT UMETA(DisplayName = "Object"),
};

UENUM(BlueprintType)
enum class EJsonValueTypeBP : uint8
{
	NONE UMETA(DisplayName = "*NONE*"),
	NODE UMETA(DisplayName = "Node"),
	STRING UMETA(DisplayName = "String"),
	BOOL UMETA(DisplayName = "Bool"),
	NUMBER UMETA(DisplayName = "Number"),
};

USTRUCT(BlueprintType)
struct JSONPARSEPLUGIN_API FJsonValueContent
{
	GENERATED_BODY()

	TSharedPtr<FJsonValue> Value;

	UPROPERTY(BlueprintReadWrite, Category = "JsonParse|JsonValue")
		FString Key;

	UPROPERTY(BlueprintReadWrite, Category = "JsonParse|JsonValue")
		FJsonValueType ValueType;
};


/**
 * 
 */
UCLASS()
class JSONPARSEPLUGIN_API UJsonParseHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()


public:

	UFUNCTION(BlueprintCallable, Category = "Json|JsonParser")
		static bool ParseJson(const FString& JsonStr, FJsonValueContent& JsonValue);

	UFUNCTION(BlueprintCallable, Category = "Json|JsonParser")
		static TArray<FJsonValueContent> GetAllChildrenNode(const FJsonValueContent& JsonValue);

	UFUNCTION(BlueprintCallable, Category = "Json|JsonParser")
		static bool NodeAsBool(const FJsonValueContent& JsonValue, bool& bSucceed, FString& Key);

	UFUNCTION(BlueprintCallable, Category = "Json|JsonParser")
		static float NodeAsNumber(const FJsonValueContent& JsonValue, bool& bSucceed, FString& Key);

	UFUNCTION(BlueprintCallable, Category = "Json|JsonParser")
		static FString NodeAsString(const FJsonValueContent& JsonValue, bool& bSucceed, FString& Key);


	UFUNCTION(BlueprintCallable, Category = "Json|JsonParser", meta = (BlueprintInternalUseOnly = "true"))
		static bool GetStringValue(const FJsonValueContent& JsonValue, const FString& Key, FString& FoundValue);

	UFUNCTION(BlueprintCallable, Category = "Json|JsonParser", meta = (BlueprintInternalUseOnly = "true"))
		static bool GetBoolValue(const FJsonValueContent& JsonValue, const FString& Key, bool& FoundValue);

	UFUNCTION(BlueprintCallable, Category = "Json|JsonParser", meta = (BlueprintInternalUseOnly = "true"))
		static bool GetChildJsonNode(const FJsonValueContent& JsonValue, const FString& Key, FJsonValueContent& FoundValue);

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "Json|JsonParser", meta = (BlueprintInternalUseOnly = "true", CustomStructureParam = "FoundValue"))
		static bool GetNumberValue(const FJsonValueContent& JsonValue, const FString& Key, int32& FoundValue);

	//DECLARE_FUNCTION(execGetNumberValue)
	static void execGetNumberValue(UObject* Context, FFrame& Stack, RESULT_DECL)
	{
		P_GET_STRUCT_REF(FJsonValueContent, JsonValue);
		P_GET_PROPERTY(FStrProperty, Key);

		Stack.StepCompiledIn<FProperty>(NULL);
		FProperty* ValueProperty = CastField<FProperty>(Stack.MostRecentProperty);
		void* ValueAddrIn = Stack.MostRecentPropertyAddress;

		P_FINISH;
		P_NATIVE_BEGIN;
		*(bool*)RESULT_PARAM = UJsonParseHelper::Generic_GetNumberValue(JsonValue, *Key, ValueAddrIn, ValueProperty);
		P_NATIVE_END;
	}

	static bool Generic_GetNumberValue(const FJsonValueContent& JsonValue, const FString& Key, void* ValueAddr, FProperty* ValueType);

	template<typename T>
	static bool GetNumberValue_Template(const FJsonValueContent& JsonValue, const FString& Key, T& FoundValue);

	UFUNCTION(BlueprintCallable, Category = "Json|JsonWriter")
		static FString JsonNodeToString(const FJsonValueContent& JsonValue);

	static FJsonValueType ConfirmJsonType(TSharedPtr<FJsonValue> JsonValue);
	static FString NodeToString(FJsonValueContent JsonValue);
};

template<typename T>
inline bool UJsonParseHelper::GetNumberValue_Template(const FJsonValueContent& JsonValue, const FString& Key, T& FoundValue)
{
	if (JsonValue.ValueType == FJsonValueType::JVT_OBJECT)
	{
		double OutNumber;
		if (JsonValue.Value->AsObject()->TryGetNumberField(Key, OutNumber))
		{
			FoundValue = OutNumber;
			return true;
		}
	}
	else if (JsonValue.ValueType == FJsonValueType::JVT_ARRAY)
	{
		if (JsonValue.Value->AsArray().Num() == 1 && JsonValue.Value->AsArray()[0]->Type == EJson::Object)
		{
			double OutNumber;
			if (JsonValue.Value->AsArray()[0]->AsObject()->TryGetNumberField(Key, OutNumber))
			{
				FoundValue = OutNumber;
				return true;
			}
		}
	}

	return false;
}