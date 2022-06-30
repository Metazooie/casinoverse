// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Json/Public/Serialization/JsonReader.h"
#include "Json/Public/Serialization/JsonSerializer.h"
#include "Json/Public/Dom/JsonObject.h"
#include "JsonUtilities/Public/JsonObjectConverter.h"
#include "Core/Public/Containers/UnrealString.h"
#include "Policies/CondensedJsonPrintPolicy.h"
#include "Engine.h"
#include "AgentPlatformBlueprintLibrary.generated.h"

const FString UIInteraction = "50";
// TODO: PWPLAT-655. More work is required around messages, types, etc.
const int OnWindowResize = 1;


USTRUCT(BlueprintType)
struct FAgentPlatformJsonObject
{
	GENERATED_USTRUCT_BODY()
	FAgentPlatformJsonObject() : Object(NULL) {};
	TSharedPtr<class FJsonObject> Object;
};
USTRUCT(BlueprintType)
struct FAgentPlatformJsonValue
{
	GENERATED_USTRUCT_BODY()
	TSharedPtr<class FJsonValue> Value;
};


/**
 * 
 */
UCLASS()
class AGENTPLUGIN_API UAgentPlatformBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Pureweb Agent")
	static FString GetKeyValue(FString values, FString key);

	UFUNCTION(BlueprintCallable, Category = "Pureweb Agent")
		static FString JsonGetValue(FString json, FString keyValue);
	UFUNCTION(BlueprintCallable, Category = "Pureweb Agent")
		static FAgentPlatformJsonObject JsonGetObject(FString json);
	UFUNCTION(BlueprintCallable, Category = "Pureweb Agent")
		static FAgentPlatformJsonObject JsonAddValue(FString key, FString value, FAgentPlatformJsonObject jsonObject);
	UFUNCTION(BlueprintCallable, Category = "Pureweb Agent")
		static FAgentPlatformJsonObject JsonAddObject(FString key, FAgentPlatformJsonObject newObject, FAgentPlatformJsonObject jsonObject);
	UFUNCTION(BlueprintCallable, Category = "Pureweb Agent")
		static FString JsonToString(FAgentPlatformJsonObject jsonObject);
	UFUNCTION(BlueprintCallable, Category = "Pureweb Agent")
		static void HandleStreamerInputComponent(const FString& descriptor);

	static FString Find(TMap<FString, TSharedPtr<FJsonValue>> values, FString keyValue);

};
