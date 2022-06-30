#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "FProcessInfo.generated.h"

USTRUCT(BlueprintType)
struct FProcessInfo
{
	GENERATED_BODY()
public:
	UPROPERTY() FString Type = "ProcessInfo";
	UPROPERTY() FString AppName;
	UPROPERTY() FString AppID;
	UPROPERTY() int ProcessID;
	UPROPERTY() FString ServiceManagerID;
	UPROPERTY() FString SchedulerID;
	UPROPERTY() FString LaunchReqURL;
	UPROPERTY() FString ProjectToken;
	UPROPERTY() FString AgentToken;
};