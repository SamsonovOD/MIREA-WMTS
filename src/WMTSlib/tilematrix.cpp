#include "tilematrix.h"

TileMatrix::TileMatrix(){}

double TileMatrix::pix2crsX(double x, Layer& layer) {
    x *= layer.crs.ux - layer.crs.lx;
    x /= (matrixWidth * tileWidth);
    x += layer.crs.lx;
    return x;
}

double TileMatrix::pix2crsY(double y, Layer& layer) {
    y *= layer.crs.uy - layer.crs.ly;
    y /= (matrixHeight * tileHeight);
    y = layer.crs.uy - y;
    return y;
}

double TileMatrix::crs2pixX(double x, Layer& layer) {
    x -= layer.crs.lx;
    x *= (matrixWidth * tileWidth);
    x /= layer.crs.ux - layer.crs.lx;
    return x;
}

double TileMatrix::crs2pixY(double y, Layer& layer) {
    y =  layer.crs.uy - y;
    y *= (matrixHeight * tileWidth);
    y /= layer.crs.uy - layer.crs.ly;
    return y;
}

void TileMatrix::debug() {
    qDebug() <<
    identifier << " " <<
    scaleDenominator <<  " " <<
    topLeftCornerX <<  " " <<
    topLeftCornerY <<  " " <<
    tileWidth <<  " " <<
    tileHeight <<  " " <<
    matrixWidth <<  " " <<
    matrixHeight <<  " ";
}
