#include "tntn/MercatorProjection.h"

namespace tntn {

MercatorProjection::MercatorProjection(int _tileSize) : m_TileSize(_tileSize)
{
    double invTileSize = 1.0 / m_TileSize;
    m_Res = 2.0 * HALF_CIRCUMFERENCE * invTileSize;
}

glm::dvec2 MercatorProjection::LonLatToMeters(const glm::dvec2 _lonLat) const
{
    glm::dvec2 meters;
    meters.x = _lonLat.x * HALF_CIRCUMFERENCE * INV_180;
    meters.y = log(tan(PI * 0.25 + _lonLat.y * PI * INV_360));
    meters.y = meters.y * (double)R_EARTH;
    return (meters);
}

glm::dvec2 MercatorProjection::MetersToLonLat(const glm::dvec2 _meters) const
{
    glm::dvec2 lonLat;
    double invHalfCircum = 1.0 / HALF_CIRCUMFERENCE;
    double invPI = 1.0 / PI;
    lonLat.x = _meters.x * invHalfCircum * 180.0;
    lonLat.y = (2.0 * atan(exp((_meters.y / R_EARTH))) - PI * 0.5) * 180 * invPI;
    return lonLat;
}

glm::dvec2 MercatorProjection::PixelsToMeters(const glm::dvec2 _pix, const int _zoom) const
{
    glm::dvec2 meters;
    double res = m_Res / (1 << _zoom);
    meters.x = _pix.x * res - HALF_CIRCUMFERENCE;
    meters.y = _pix.y * res - HALF_CIRCUMFERENCE;
    return meters;
}

glm::dvec2 MercatorProjection::MetersToPixel(const glm::dvec2 _meters, const int _zoom) const
{
    glm::dvec2 pix;
    double invRes = (1 << _zoom) / m_Res;
    pix.x = (_meters.x + HALF_CIRCUMFERENCE) * invRes;
    pix.y = (_meters.y + HALF_CIRCUMFERENCE) * invRes;
    return pix;
}

glm::ivec2 MercatorProjection::PixelsToTileXY(const glm::dvec2 _pix) const
{
    //returns the tile covering a region of a pixel
    glm::ivec2 tileXY;
    double invTileSize = 1.0 / m_TileSize;
    tileXY.x = int(ceil(_pix.x * invTileSize) - 1);
    tileXY.y = int(ceil(_pix.y * invTileSize) - 1);
    return tileXY;
}

glm::ivec2 MercatorProjection::MetersToTileXY(const glm::dvec2 _meters, const int _zoom) const
{
    glm::dvec2 pix;
    glm::ivec2 tileXY;
    pix = MetersToPixel(_meters, _zoom);
    tileXY = PixelsToTileXY(pix);
    return tileXY;
}

glm::dvec2 MercatorProjection::PixelsToRaster(const glm::dvec2 _pix, const int _zoom) const
{
    glm::dvec2 transformedPix;
    double mapSize;
    mapSize = m_TileSize << _zoom;
    transformedPix = glm::dvec2(_pix.x, (mapSize - _pix.y));
    return transformedPix;
}

BoundingBox MercatorProjection::TileBounds(const int _tx, const int _ty, const int _zoom) const
{
    return {PixelsToMeters({_tx * m_TileSize, _ty * m_TileSize}, _zoom),
            PixelsToMeters({(_tx + 1) * m_TileSize, (_ty + 1) * m_TileSize}, _zoom)};
}

} //namespace tntn
