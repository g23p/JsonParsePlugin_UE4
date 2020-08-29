// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "K2Node_JsonParse.generated.h"

/**
 * 
 */


UCLASS()
class JSONPARSEEDITOR_API UK2Node_JsonParse : public UK2Node
{
	GENERATED_BODY()
	
public:

	virtual void AllocateDefaultPins() override;
	virtual FText GetTooltipText() const override { return FText::FromString(TEXT("Get Child Json Node Value")); }
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override { return FText::FromString(TEXT("Get Child Node Value")); }
	virtual FText GetMenuCategory() const { return FText::FromString(TEXT("Json|JsonParser")); }
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;
	virtual void PinDefaultValueChanged(UEdGraphPin* Pin) override;
	virtual void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) override;

	UEdGraphPin* GetThenPin() const;

protected:

	void OnEnumPinChanged(UEdGraphPin* Pin);

	UEdGraphPin* GetJsonValuePin() const;
	UEdGraphPin* GetJsonTypePin() const;
	UEdGraphPin* GetJsonKeyPin() const;
	UEdGraphPin* GetIsSucceedPin() const;
	UEdGraphPin* GetReturnValuePin() const;

private:

	UEdGraphPin* GetJsonTypePin(TArray<UEdGraphPin*>* OldPins);
};
