#pragma once

#include "../SharedLibraryExport.h"

namespace PureWeb
{
    namespace Mapping
    {
        struct WGS84Point
        {
            double latitude;
            double longitude;
        };

        struct ModelPoint
        {
            double x;
            double y;
        };

        /*
            Based on formulas from: https://www.movable-type.co.uk/scripts/latlong.html

            PixelSize in meters/pixel
        */
        class EXPORT Conversion
        {
        public:
            enum class GridDirectionality 
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

            static ModelPoint ConvertWGS84toModel(ModelPoint origin, WGS84Point originLocation, double pixelSize, WGS84Point pointToConvert, GridDirectionality gridDirectionality = GridDirectionality::PositiveYNorthPositiveXWest);
            static WGS84Point ConvertModeltoWGS84(ModelPoint origin, WGS84Point originLocation, double pixelSize, ModelPoint pointToConvert, GridDirectionality gridDirectionality = GridDirectionality::PositiveYNorthPositiveXWest);

            static double DegreesToRadians(double degrees);
            static double RadiansToDegrees(double radians);

            static double DistanceFromOrigin(WGS84Point origin, WGS84Point point);
            static double DistanceFromOrigin(double pixelSize, ModelPoint origin, ModelPoint point);

            static double Bearing(WGS84Point origin, WGS84Point point);
            static double Bearing(WGS84Point origin, WGS84Point point, GridDirectionality gridDirectionality);
            static double Bearing(ModelPoint origin, ModelPoint point, GridDirectionality gridDirectionality);

        private:
            static const int EARTH_RADIUS;
            enum class Directionality { PositiveX, NegativeX, PositiveY, NegativeY };

            static double CorrectBearingForDirectionality(double bearing, const GridDirectionality directionality);
            static void DecomposeDirectionality(const GridDirectionality directionality, Directionality& north, Directionality& west);
        };
    }
}