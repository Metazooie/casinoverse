// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <string>
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Json/Public/Serialization/JsonReader.h"
#include "Json/Public/Serialization/JsonSerializer.h"
#include "Json/Public/Dom/JsonObject.h"
#include "JsonUtilities/Public/JsonObjectConverter.h"
#include "Core/Public/Containers/UnrealString.h"
#include "Policies/CondensedJsonPrintPolicy.h"
#include "Mapping/Conversion.h"
#include "MappingBlueprintLibrary.generated.h"

USTRUCT(BlueprintType)
struct FMappingWGS84
{
	GENERATED_BODY()
	FMappingWGS84()
	{

	}

	UPROPERTY(BlueprintReadWrite, Category = "Pureweb Mapping")
	FString longitude;

	UPROPERTY(BlueprintReadWrite, Category = "Pureweb Mapping")
	FString latitude;
};

UENUM()
enum GridDirectionality 
{ 
	PositiveYNorthPositiveXWest,
	PositiveYNorthNegativeXWest,
	NegativeYNorthPositiveXWest,
	NegativeYNorthNegativeXWest,
	PositiveXNorthPositiveYWest,
	PositiveXNorthNegativeYWest,
	NegativeXNorthPositiveYWest,
	NegativeXNorthNegativeYWest
};

/**
 * 
 */
UCLASS()
class MAPPINGPLUGIN_API UMappingBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Pureweb Mapping")
		static FVector ConvertWGS84PointToModelPoint(FVector modelOrigin, FMappingWGS84 realWorldOrigin, float pixelSize, FMappingWGS84 pointToConvert, GridDirectionality directionality);
	UFUNCTION(BlueprintCallable, Category = "Pureweb Mapping")
		static FMappingWGS84 ConvertModelPointToWGS84Point(FVector modelOrigin, FMappingWGS84 realWorldOrigin, float pixelSize, FVector pointToConvert, GridDirectionality directionality);

	UFUNCTION(BlueprintCallable, Category = "Pureweb Mapping")
		static float DegreesToRadians(float degrees);
	UFUNCTION(BlueprintCallable, Category = "Pureweb Mapping")
		static float RadiansToDegrees(float radians);

	UFUNCTION(BlueprintCallable, Category = "Pureweb Mapping")
		static float DistanceFromRealWorldOrigin(FMappingWGS84 origin, FMappingWGS84 point);
	UFUNCTION(BlueprintCallable, Category = "Pureweb Mapping")
		static float DistanceFromModelOrigin(float pixelSize, FVector origin, FVector point);

	UFUNCTION(BlueprintCallable, Category = "Pureweb Mapping")
		static float BearingRealWorld(FMappingWGS84 origin, FMappingWGS84 point);
	UFUNCTION(BlueprintCallable, Category = "Pureweb Mapping")
		static float BearingModel(FVector origin, FVector point, GridDirectionality gridDirectionality);
private:
	static PureWeb::Mapping::WGS84Point Convert(FMappingWGS84 inputPoint);
	static PureWeb::Mapping::ModelPoint Convert(FVector inputPoint);
	static FMappingWGS84 Convert(PureWeb::Mapping::WGS84Point inputPoint);
	static PureWeb::Mapping::Conversion::GridDirectionality Convert(GridDirectionality input);
};