#ifndef TILEMATRIX_H
#define TILEMATRIX_H

#include <QtCore>

#define R_MAJOR 6378137.0
#define R_MINOR 6356752.3142
#define RATIO (R_MINOR/R_MAJOR)
#define ECCENT (sqrt(1.0 - (RATIO * RATIO)))
#define COM (0.5 * ECCENT)

static double deg_rad (double ang) { return ang * (M_PI / 180.0); }
static double rad_deg (double ang) { return ang * (180.0 / M_PI); }

class CRS {
public:
    virtual double m_lon(double /*x*/) { return 0; }
    virtual double m_lat(double /*y*/) { return 0; }
    virtual double lon_m(double /*lon*/) { return 0; }
    virtual double lat_m(double /*lat*/) { return 0; }
};

class WGS: public CRS {
public:
    virtual double m_lon(double x) { return x; }
    virtual double m_lat(double y) { return y; }
    virtual double lon_m(double lon) { return lon; }
    virtual double lat_m(double lat) { return lat; }
} ;

class Mercator: public CRS {
public:
    virtual double m_lon (double x) {
            return rad_deg(x) / R_MAJOR;
    }
    virtual double m_lat (double y) {
            double ts = exp( -y / R_MAJOR);
            double phi = M_PI_2 - 2 * atan(ts);
            double dphi = 1.0;
            int i;
            for (i = 0; fabs(dphi) > 0.000000001 && i < 15; i++) {
                    double con = ECCENT * sin (phi);
                    dphi = M_PI_2 - 2 * atan (ts * pow((1.0 - con) / (1.0 + con), COM)) - phi;
                    phi += dphi;
            }
            return rad_deg (phi);
    }
    virtual double lon_m (double lon) {
            return R_MAJOR * deg_rad(lon);
    }
    virtual double lat_m (double lat) {
            lat = fmin (89.5, fmax (lat, -89.5));
            double phi = deg_rad(lat);
            double sinphi = sin(phi);
            double con = ECCENT * sinphi;
            con = pow((1.0 - con) / (1.0 + con), COM);
            double ts = tan(0.5 * (M_PI * 0.5 - phi)) / con;
            return 0 - R_MAJOR * log(ts);
    }
} ;

class Google: public CRS {
public:
    virtual double m_lon(double x) { return rad_deg(x/R_MAJOR); }
    virtual double m_lat(double y) { return rad_deg(2 * atan(exp( y/R_MAJOR)) - M_PI/2); }
    virtual double lon_m(double lon) { return deg_rad(lon) * R_MAJOR; }
    virtual double lat_m(double lat) { return log(tan( deg_rad(lat) / 2 + M_PI/4 )) * R_MAJOR; }
} ;

class BoundingBox {
    public:
        double lx, ly, ux, uy;
};

class Layer {
    public:
        BoundingBox crs;
        BoundingBox wgs;
        CRS *calculator;
        WGS wgsclass;
        Mercator mercator;
        Google google;
        double wgs2crsX(double x){ return calculator->lon_m(x); }
        double wgs2crsY(double y){ return calculator->lat_m(y); }
        double crs2wgsX(double x){ return calculator->m_lon(x); }
        double crs2wgsY(double y){ return calculator->m_lat(y); }
};

class TileMatrix {
    public:
        TileMatrix();
        QString identifier;
        double scaleDenominator;
        double topLeftCornerX;
        double topLeftCornerY;
        int tileWidth;
        int tileHeight;
        int matrixWidth;
        int matrixHeight;
        double pix2crsX(double x, Layer& layer);
        double pix2crsY(double y, Layer& layer);
        double crs2pixX(double x, Layer& layer);
        double crs2pixY(double y, Layer& layer);
        void debug();
};

#endif // TILEMATRIX_H
