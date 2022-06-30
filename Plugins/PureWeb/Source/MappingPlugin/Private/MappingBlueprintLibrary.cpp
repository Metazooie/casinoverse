// Fill out your copyright notice in the Description page of Project Settings.


#include "MappingBlueprintLibrary.h"

FVector UMappingBlueprintLibrary::ConvertWGS84PointToModelPoint(FVector modelOrigin, FMappingWGS84 realWorldOrigin, float pixelSize, FMappingWGS84 pointToConvert, GridDirectionality directionality)
{
    PureWeb::Mapping::WGS84Point originLocation = Convert(realWorldOrigin);
    PureWeb::Mapping::WGS84Point point = Convert(pointToConvert);
    PureWeb::Mapping::ModelPoint origin = Convert(modelOrigin);

    PureWeb::Mapping::ModelPoint convertedPoint = PureWeb::Mapping::Conversion::ConvertWGS84toModel(origin, originLocation, pixelSize, point, Convert(directionality));

    return FVector(convertedPoint.x, convertedPoint.y, modelOrigin.Z);
}

FMappingWGS84 UMappingBlueprintLibrary::ConvertModelPointToWGS84Point(FVector modelOrigin, FMappingWGS84 realWorldOrigin, float pixelSize, FVector pointToConvert, GridDirectionality directionality)
{
    PureWeb::Mapping::WGS84Point originLocation = Convert(realWorldOrigin);
    PureWeb::Mapping::ModelPoint point = Convert(pointToConvert);
    PureWeb::Mapping::ModelPoint origin = Convert(modelOrigin);

    PureWeb::Mapping::WGS84Point convertedPoint = PureWeb::Mapping::Conversion::ConvertModeltoWGS84(origin, originLocation, pixelSize, point, Convert(directionality));

    return Convert(convertedPoint);
}

float UMappingBlueprintLibrary::DegreesToRadians(float degrees)
{
    return (float)PureWeb::Mapping::Conversion::DegreesToRadians((double)degrees);
}

float UMappingBlueprintLibrary::RadiansToDegrees(float radians)
{
    return (float)PureWeb::Mapping::Conversion::RadiansToDegrees((double)radians);
}

float UMappingBlueprintLibrary::DistanceFromRealWorldOrigin(FMappingWGS84 origin, FMappingWGS84 point)
{
    return (float)PureWeb::Mapping::Conversion::DistanceFromOrigin(Convert(origin), Convert(point));
}

float UMappingBlueprintLibrary::DistanceFromModelOrigin(float pixelSize, FVector origin, FVector point)
{
    return (float)PureWeb::Mapping::Conversion::DistanceFromOrigin((double)pixelSize, Convert(origin), Convert(point));
}

float UMappingBlueprintLibrary::BearingRealWorld(FMappingWGS84 origin, FMappingWGS84 point)
{
    return (float)PureWeb::Mapping::Conversion::Bearing(Convert(origin), Convert(point));
}

float UMappingBlueprintLibrary::BearingModel(FVector origin, FVector point, GridDirectionality gridDirectionality)
{
    return (float)PureWeb::Mapping::Conversion::Bearing(Convert(origin), Convert(point), Convert(gridDirectionality));
}

PureWeb::Mapping::WGS84Point UMappingBlueprintLibrary::Convert(FMappingWGS84 inputPoint)
{
    PureWeb::Mapping::WGS84Point outputPoint;

    std::string latitude = TCHAR_TO_UTF8(*inputPoint.latitude);
    std::string longitude = TCHAR_TO_UTF8(*inputPoint.longitude);

    outputPoint.latitude = std::stod(latitude);
    outputPoint.longitude = std::stod(longitude);

    return outputPoint;
}

PureWeb::Mapping::ModelPoint UMappingBlueprintLibrary::Convert(FVector inputPoint)
{
    PureWeb::Mapping::ModelPoint outputPoint;

    outputPoint.x = inputPoint.X;
    outputPoint.y = inputPoint.Y;

    return outputPoint;
}

FMappingWGS84 UMappingBlueprintLibrary::Convert(PureWeb::Mapping::WGS84Point inputPoint)
{
    FMappingWGS84 output;

    output.latitude = FString::SanitizeFloat(inputPoint.latitude);
    output.longitude = FString::SanitizeFloat(inputPoint.longitude);

    return output;
}

PureWeb::Mapping::Conversion::GridDirectionality UMappingBlueprintLibrary::Convert(GridDirectionality input)
{
    switch (input)
    {
        case GridDirectionality::PositiveYNorthPositiveXWest:
            return PureWeb::Mapping::Conversion::GridDirectionality::PositiveYNorthPositiveXWest;
        case GridDirectionality::PositiveYNorthNegativeXWest:
            return PureWeb::Mapping::Conversion::GridDirectionality::PositiveYNorthNegativeXWest;
        case GridDirectionality::NegativeYNorthPositiveXWest:
            return PureWeb::Mapping::Conversion::GridDirectionality::NegativeYNorthPositiveXWest;
        case GridDirectionality::NegativeYNorthNegativeXWest:
            return PureWeb::Mapping::Conversion::GridDirectionality::NegativeYNorthNegativeXWest;
        case GridDirectionality::PositiveXNorthPositiveYWest:
            return PureWeb::Mapping::Conversion::GridDirectionality::PositiveXNorthPositiveYWest;
        case GridDirectionality::PositiveXNorthNegativeYWest:
            return PureWeb::Mapping::Conversion::GridDirectionality::PositiveXNorthNegativeYWest;
        case GridDirectionality::NegativeXNorthPositiveYWest:
            return PureWeb::Mapping::Conversion::GridDirectionality::NegativeXNorthPositiveYWest;
        case GridDirectionality::NegativeXNorthNegativeYWest:
        default:
            return PureWeb::Mapping::Conversion::GridDirectionality::NegativeXNorthNegativeYWest;
    }
}