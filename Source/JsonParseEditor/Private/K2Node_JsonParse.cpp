// Fill out your copyright notice in the Description page of Project Settings.


#include "K2Node_JsonParse.h"
#include "BlueprintNodeSpawner.h"
#include "BlueprintActionDatabaseRegistrar.h"

#include "EdGraphSchema_K2.h"
#include "K2Node_CallFunction.h"

#include "KismetCompiler.h"
#include "EdGraph/EdGraphNode.h"

#include "JsonParseHelper.h"

#include "Kismet2/BlueprintEditorUtils.h"

struct FK2Node_JsonParsePinName
{
	static const FName JsonValuePinName;
	static const FName JsonTypePinName;
	static const FName JsonKeyPinName;
	static const FName bIsSucceedPinName;
	static const FName ReturnValuePinName;
};

const FName FK2Node_JsonParsePinName::JsonValuePinName(TEXT("JsonValue"));
const FName FK2Node_JsonParsePinName::JsonTypePinName(TEXT("ChildJsonType"));
const FName FK2Node_JsonParsePinName::JsonKeyPinName(TEXT("Key"));
const FName FK2Node_JsonParsePinName::bIsSucceedPinName(TEXT("bIsSucceed"));
const FName FK2Node_JsonParsePinName::ReturnValuePinName(TEXT("Value"));

void UK2Node_JsonParse::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);

	UScriptStruct* JsonValueStruct = TBaseStructure<FJsonValueContent>::Get();
	UEdGraphPin* JsonValuePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, JsonValueStruct, FK2Node_JsonParsePinName::JsonValuePinName);

	UEnum* const JsonTypeEnum = FindObjectChecked<UEnum>(nullptr, TEXT("/Script/JsonParsePlugin.EJsonValueTypeBP"), true);
	UEdGraphPin* const JsonTypePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Byte, JsonTypeEnum, FK2Node_JsonParsePinName::JsonTypePinName);
	JsonTypePin->DefaultValue = JsonTypeEnum->GetNameStringByValue(static_cast<int>(EJsonValueTypeBP::NONE));

	UEdGraphPin* JsonKeyPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_String, FK2Node_JsonParsePinName::JsonKeyPinName);

	UEdGraphPin* IsSucceedPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Boolean, FK2Node_JsonParsePinName::bIsSucceedPinName);

	UEdGraphPin* ReturnValuePin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Wildcard, FK2Node_JsonParsePinName::ReturnValuePinName);
}

void UK2Node_JsonParse::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* ActionKey = GetClass();

	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

void UK2Node_JsonParse::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	static const FName JsonValueParamName(TEXT("JsonValue"));
	static const FName JsonKeyParamName(TEXT("Key"));
	//static const FName IsSucceedParamName(TEXT("bSucceed"));
	static const FName JsonReturnValueName(TEXT("FoundValue"));

	UEdGraphPin* ExecPin = GetExecPin();
	UEdGraphPin* ThenPin = GetThenPin();

	UEdGraphPin* JsonValuePin = GetJsonValuePin();
	UEdGraphPin* JsonTypePin = GetJsonTypePin();
	UEdGraphPin* JsonKeyPin = GetJsonKeyPin();
	UEdGraphPin* IsSucceedPin = GetIsSucceedPin();
	UEdGraphPin* ReturnValuePin = GetReturnValuePin();

	if (ExecPin && ThenPin)
	{
		FString TypeStr = JsonTypePin->GetDefaultAsString();
		FName MyFunctionName = FName();
		UK2Node_CallFunction* CallFuncNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);

		if (TypeStr == TEXT("NONE"))
		{

		}
		else if (TypeStr == TEXT("NODE"))
		{
			MyFunctionName = GET_FUNCTION_NAME_CHECKED(UJsonParseHelper, GetChildJsonNode);
		}
		else if (TypeStr == TEXT("STRING"))
		{
			MyFunctionName = GET_FUNCTION_NAME_CHECKED(UJsonParseHelper, GetStringValue);
		}
		else if (TypeStr == TEXT("BOOL"))
		{
			MyFunctionName = GET_FUNCTION_NAME_CHECKED(UJsonParseHelper, GetBoolValue);
		}
		else if (TypeStr == TEXT("NUMBER"))
		{
			MyFunctionName = GET_FUNCTION_NAME_CHECKED(UJsonParseHelper, GetNumberValue);
		}
		
		if (!MyFunctionName.IsNone())
		{
			CallFuncNode->FunctionReference.SetExternalMember(MyFunctionName, UJsonParseHelper::StaticClass());
			CallFuncNode->AllocateDefaultPins();

			UEdGraphPin* CallJsonValue = CallFuncNode->FindPinChecked(JsonValueParamName);
			UEdGraphPin* CallJsonKey = CallFuncNode->FindPinChecked(JsonKeyParamName);
			UEdGraphPin* CallIsSucceed = CallFuncNode->GetReturnValuePin();
			UEdGraphPin* CallReturnValue = CallFuncNode->FindPinChecked(JsonReturnValueName);
#if ENGINE_MAJOR_VERSION == 4
			CallReturnValue->PinType.PinCategory = UEdGraphSchema_K2::PC_Float;
#elif ENGINE_MAJOR_VERSION == 5
			CallReturnValue->PinType.PinCategory = UEdGraphSchema_K2::PC_Real;
			CallReturnValue->PinType.PinSubCategory = UEdGraphSchema_K2::PC_Double;
#endif

			CompilerContext.MovePinLinksToIntermediate(*ExecPin, *CallFuncNode->GetExecPin());

			CompilerContext.MovePinLinksToIntermediate(*JsonValuePin, *CallJsonValue);
			CompilerContext.MovePinLinksToIntermediate(*JsonKeyPin, *CallJsonKey);
			CompilerContext.MovePinLinksToIntermediate(*IsSucceedPin, *CallIsSucceed);
			CompilerContext.MovePinLinksToIntermediate(*ReturnValuePin, *CallReturnValue);

			CompilerContext.MovePinLinksToIntermediate(*ThenPin, *CallFuncNode->GetThenPin());
		}
	}

	BreakAllNodeLinks();
}

void UK2Node_JsonParse::PinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::PinConnectionListChanged(Pin);

	if (Pin && (Pin->PinName == FK2Node_JsonParsePinName::JsonTypePinName))
	{
		OnEnumPinChanged(Pin);
	}
}

void UK2Node_JsonParse::PinDefaultValueChanged(UEdGraphPin* Pin)
{
	Super::PinDefaultValueChanged(Pin);

	if (Pin && (Pin->PinName == FK2Node_JsonParsePinName::JsonTypePinName))
	{
		OnEnumPinChanged(Pin);
	}
}

void UK2Node_JsonParse::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins)
{
	AllocateDefaultPins();

	UEdGraphPin* JsonTypePin = GetEnumPinFromAllPins(&OldPins);
	OnEnumPinChanged(JsonTypePin);

	RestoreSplitPins(OldPins);
}

void UK2Node_JsonParse::OnEnumPinChanged(UEdGraphPin* Pin)
{
	TArray<UEdGraphPin*> ConnectionList = Pin->LinkedTo;

	UEdGraphPin* ReturnValuePin = GetReturnValuePin();
	TArray<UEdGraphPin*> ReturnValuePinConnetionList;
	if (ReturnValuePin)
	{
		ReturnValuePinConnetionList = ReturnValuePin->LinkedTo;
		ReturnValuePin->BreakAllPinLinks(true);
		RemovePin(ReturnValuePin);
	}

	FString TypeStr = Pin->GetDefaultAsString();

	if (ConnectionList.Num() > 0 || TypeStr == TEXT("NONE"))
	{
		CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Wildcard, FK2Node_JsonParsePinName::ReturnValuePinName);
	}
	else if (TypeStr == TEXT("NODE"))
	{
		UScriptStruct* JsonValueStruct = TBaseStructure<FJsonValueContent>::Get();
		CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Struct, JsonValueStruct, FK2Node_JsonParsePinName::ReturnValuePinName);
	}
	else if (TypeStr == TEXT("STRING"))
	{
		CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_String, FK2Node_JsonParsePinName::ReturnValuePinName);
	}
	else if (TypeStr == TEXT("BOOL"))
	{
		CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Boolean, FK2Node_JsonParsePinName::ReturnValuePinName);
	}
	else if (TypeStr == TEXT("NUMBER"))
	{
#if ENGINE_MAJOR_VERSION == 4
		CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Float, FK2Node_JsonParsePinName::ReturnValuePinName);
#elif ENGINE_MAJOR_VERSION == 5
		CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Real, UEdGraphSchema_K2::PC_Double, FK2Node_JsonParsePinName::ReturnValuePinName);
#endif
	}

	ReturnValuePin = GetReturnValuePin();
	if (ReturnValuePin && ReturnValuePinConnetionList.Num() > 0)
	{
		const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

		for (UEdGraphPin* LinkedPin : ReturnValuePinConnetionList)
		{
			K2Schema->TryCreateConnection(ReturnValuePin, LinkedPin);
		}
	}

	GetGraph()->NotifyGraphChanged();
	FBlueprintEditorUtils::MarkBlueprintAsModified(GetBlueprint());
}

UEdGraphPin* UK2Node_JsonParse::GetThenPin() const
{
	UEdGraphPin* Pin = FindPin(UEdGraphSchema_K2::PN_Then);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

UEdGraphPin* UK2Node_JsonParse::GetJsonValuePin() const
{
	UEdGraphPin* Pin = FindPin(FK2Node_JsonParsePinName::JsonValuePinName);
	check(Pin == nullptr || Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2Node_JsonParse::GetJsonTypePin() const
{
	UEdGraphPin* Pin = FindPin(FK2Node_JsonParsePinName::JsonTypePinName);
	check(Pin == nullptr || Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2Node_JsonParse::GetJsonKeyPin() const
{
	UEdGraphPin* Pin = FindPin(FK2Node_JsonParsePinName::JsonKeyPinName);
	check(Pin == nullptr || Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2Node_JsonParse::GetIsSucceedPin() const
{
	UEdGraphPin* Pin = FindPin(FK2Node_JsonParsePinName::bIsSucceedPinName);
	check(Pin == nullptr || Pin->Direction == EGPD_Output);
	return Pin;
}

UEdGraphPin* UK2Node_JsonParse::GetReturnValuePin() const
{
	UEdGraphPin* Pin = FindPin(FK2Node_JsonParsePinName::ReturnValuePinName);
	check(Pin == nullptr || Pin->Direction == EGPD_Output);
	return Pin;
}

UEdGraphPin* UK2Node_JsonParse::GetEnumPinFromAllPins(const TArray<UEdGraphPin*>* InPins)
{
	const TArray<UEdGraphPin*>* PinsToSearch = InPins ? InPins : &Pins;

	UEdGraphPin* JsonTypePin = nullptr;
	for (UEdGraphPin* Pin : *PinsToSearch)
	{
		if (Pin && Pin->PinName == FK2Node_JsonParsePinName::JsonTypePinName)
		{
			JsonTypePin = Pin;
			break;
		}
	}
	return JsonTypePin;
}

