#ifndef WMTSLIB_H
#define WMTSLIB_H

#include "wmtslib_global.h"
#include "downloadmanager.h"

class WMTSLIBSHARED_EXPORT WMTSLib{
public:
    WMTSLib();
    ~WMTSLib();
    DownloadManager mod;
    bool args2manager(QStringList args);
    bool getInputs(int argc, char *argv[]);
    QImage start(int argc, char *argv[]);
    void set_scale(double scale);
    void set_style(QString style);
    void set_matrixset(QString matrixset);
    void set_xout(int xout);
    void set_yout(int yout);
    void getCapabilities();
    void loadXML(QString xml_url);
    QImage getImage(double area_y1, double area_x1, double area_y2, double area_x2);
    void getPixel(int pix_x, int pix_y);
};

#endif // WMTSLIB_H
