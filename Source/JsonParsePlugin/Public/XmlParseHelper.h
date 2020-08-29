// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "XmlParser/Public/XmlParser.h"


#include "XmlParseHelper.generated.h"

/**
 * 
 */


USTRUCT(BlueprintType)
struct FXmlFileContent
{
	GENERATED_BODY()

	TSharedPtr<FXmlFile> XmlFile;

};

USTRUCT(BlueprintType)
struct FXmlNodeContent
{
	GENERATED_BODY()

	FXmlNode* XmlNode;
};

UCLASS()
class JSONPARSEPLUGIN_API UXmlParseHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()


public:

	UFUNCTION(BlueprintCallable, Category = "Xml|XmlParser")
		static void SaveXmlToFile(const FString& FullPath, const FString& XmlStr);

	UFUNCTION(BlueprintCallable, Category = "Xml|XmlParser")
		static bool ReadXMLFromFile(const FString& Path, FXmlFileContent& XmlFileContent);

	UFUNCTION(BlueprintPure, Category = "Xml|XmlParser")
		static bool GetRootNodeFromFileContent(FXmlFileContent XmlFileContent, FXmlNodeContent& RootNode);

	UFUNCTION(BlueprintPure, Category = "Xml|XmlParser")
		static void GetAllchildrenNode(FXmlNodeContent ParentNode, TArray<FXmlNodeContent>& NodeArr);

	UFUNCTION(BlueprintPure, Category = "Xml|XmlParser")
		static void GetNode(FXmlNodeContent Node, FString& Tag, FString& Content);

	UFUNCTION(BlueprintCallable, Category = "Xml|XmlWriter")
		static void SetNewContent(FXmlNodeContent Node, const FString& NewContent);

	UFUNCTION(BlueprintCallable, Category = "Xml|XmlWriter")
		static bool SaveChange(FXmlFileContent XmlFileContent, const FString& SavePath);

	UFUNCTION(BlueprintCallable, Category = "Xml|XmlWriter")
		static bool FindChildNode(const FString& Tag, FXmlNodeContent Node, FXmlNodeContent& ChildNode);

	UFUNCTION(BlueprintPure, Category = "Xml|XmlParser")
		static bool IsValid(FXmlFileContent XmlFile);
};
