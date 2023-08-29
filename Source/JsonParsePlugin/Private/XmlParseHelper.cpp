// Fill out your copyright notice in the Description page of Project Settings.


#include "XmlParseHelper.h"

void UXmlParseHelper::SaveXmlToFile(const FString& FullPath, const FString& XmlStr)
{


}

bool UXmlParseHelper::ReadXMLFromFile(const FString& Path, FXmlFileContent& XmlFileContent)
{
	XmlFileContent.XmlFile = MakeShareable(new FXmlFile(Path));
	if (XmlFileContent.XmlFile)
		return true;

	return false;
}

bool UXmlParseHelper::GetRootNodeFromFileContent(FXmlFileContent XmlFileContent, FXmlNodeContent& RootNode)
{
	if (!XmlFileContent.XmlFile) return false;
	RootNode.XmlNode = XmlFileContent.XmlFile->GetRootNode();
	if (!RootNode.XmlNode)
	{
		return false; 
	}
	return true;
}

void UXmlParseHelper::GetAllchildrenNode(FXmlNodeContent ParentNode, TArray<FXmlNodeContent>& NodeContentArr)
{
	NodeContentArr.Empty();
	if (!ParentNode.XmlNode)
		return;
	for (FXmlNode* XmlNode : ParentNode.XmlNode->GetChildrenNodes())
	{
		FXmlNodeContent NodeContent;
		NodeContent.XmlNode = XmlNode;
		NodeContentArr.Add(NodeContent);
	}
}

void UXmlParseHelper::GetNode(FXmlNodeContent Node, FString& Tag, FString& Content)
{
	if (!Node.XmlNode) return;
	Tag = Node.XmlNode->GetTag();
	Content = Node.XmlNode->GetContent();
}

void UXmlParseHelper::SetNewContent(FXmlNodeContent Node, const FString& NewContent)
{
	if (!Node.XmlNode) return;
	Node.XmlNode->SetContent(NewContent);
}

bool UXmlParseHelper::AddNewNode(FXmlFileContent XmlFile, FString NewTag, FString NewContent)
{
	if (IsValid(XmlFile))
	{
		FXmlNodeContent RootNodeContent;
		if (GetRootNodeFromFileContent(XmlFile, RootNodeContent))return false;

		FXmlNodeContent ChildNode;
		if (FindChildNode(NewTag, RootNodeContent, ChildNode))
		{
			SetNewContent(ChildNode, NewTag);
		}
		else
		{
			RootNodeContent.XmlNode->AppendChildNode(NewTag, NewContent);
		}
		return true;
	}
	return false;
}

bool UXmlParseHelper::SaveChange(FXmlFileContent XmlFileContent, const FString& SavePath)
{
	if (!XmlFileContent.XmlFile)
		return false;
	return XmlFileContent.XmlFile->Save(SavePath);
}

bool UXmlParseHelper::FindChildNode(const FString& Tag, FXmlNodeContent Node, FXmlNodeContent& ChildNode)
{
	if (!Node.XmlNode) return false;
	ChildNode.XmlNode = Node.XmlNode->FindChildNode(Tag);
	if (!ChildNode.XmlNode)
		return false;

	return true;
}

bool UXmlParseHelper::IsValid(FXmlFileContent XmlFile)
{
	if (XmlFile.XmlFile && XmlFile.XmlFile->IsValid())
		return true;

	return false;
}

bool UXmlParseHelper::FindChildContent(FXmlNodeContent Node, const FString& Tag, FXmlNodeContent& OutNode, FString& Content)
{
	if (Node.XmlNode)
	{
		FXmlNode* XmlNode = Node.XmlNode->FindChildNode(Tag);
		if (XmlNode)
		{
			OutNode.XmlNode = XmlNode;
			Content = XmlNode->GetContent();
			return true;
		}
	}

	return false;
}
