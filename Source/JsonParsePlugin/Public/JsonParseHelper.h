// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Runtime/Json/Public/Json.h"
#include "JsonParseHelper.generated.h"

class FJsonObject;

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
struct FJsonValueContent
{
	GENERATED_BODY()

	TSharedPtr<FJsonValue> Value;

	UPROPERTY(BlueprintReadWrite)
		FString Key;

	UPROPERTY(BlueprintReadWrite)
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

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
		static FString GetStringValue(const FJsonValueContent& JsonValue, bool& bSucceed, const FString& Key);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
		static float GetNumberValue(const FJsonValueContent& JsonValue, bool& bSucceed, const FString& Key);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
		static bool GetBoolValue(const FJsonValueContent& JsonValue, bool& bSucceed, const FString& Key);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
		static FJsonValueContent GetChildJsonNode(const FJsonValueContent& JsonValue, bool& bSucceed, const FString& Key);

	UFUNCTION(BlueprintCallable, Category = "Json|JsonWriter")
		static FString JsonNodeToString(const FJsonValueContent& JsonValue);

	static FJsonValueType ConfirmJsonType(TSharedPtr<FJsonValue> JsonValue);
	static FString NodeToString(FJsonValueContent JsonValue);
};

