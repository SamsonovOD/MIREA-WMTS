#include "wmtslib.h"

WMTSLib::WMTSLib(){}
WMTSLib::~WMTSLib(){}

void WMTSLib::set_scale(double scale){
    mod.scale = scale;
}

void WMTSLib::set_style(QString style){
   mod.style = style;
}

void WMTSLib::set_matrixset(QString matrixset){
   mod.matrixSet = matrixset;
}

void WMTSLib::set_xout(int xout){
    mod.outx = xout;
}

void WMTSLib::set_yout(int yout){
    mod.outy = yout;
}

void WMTSLib::loadXML(QString url){
    if (mod.oldurl != url){
        int arc = 1;
        char appname[] = "xml";
        char *appid[1] = {appname};
        qputenv("QT_SCALE_FACTOR", "1");
        QApplication app(arc, appid);
        mod.getCapabilities(url);
        app.exec();
    }
}

QImage WMTSLib::getImage(double y1, double x1, double y2, double x2){
    int arc = 1;
    char appname[] = "tile";
    char *appid[1] = {appname};
    qputenv("QT_SCALE_FACTOR", "1");
    QApplication app(arc, appid);
    mod.getTile(y1, x1, y2, x2);
    app.exec();
    return mod.resultimg;
}

void WMTSLib::getPixel(int x, int y){
    mod.getPixel(x, y);
}
