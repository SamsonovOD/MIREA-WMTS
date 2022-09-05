#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QtNetwork>
#include <QPainter>
#include <QGraphicsScene>
#include <QtGlobal>
#include <QApplication>

#include "tilematrix.h"

class Download {
public:
    enum DownloadType {XML,TILE} type;
    QUrl url;
    int xpos;
    int ypos;
    Download(DownloadType a_type, QString a_url, int a_xpos, int a_ypos) : type(a_type), url(a_url) {xpos = a_xpos; ypos = a_ypos;}
    Download(DownloadType a_type, QString a_url): type(a_type), url(a_url) {}
};

class DownloadManager: public QObject{
    Q_OBJECT
    private:
        QNetworkAccessManager manager;
        QMap<QNetworkReply*, Download*> currentDownloads;
        QImage *canvas;
        QPainter *painter;
        QGraphicsScene *scene;

        void doLoad(Download* download);
        void findResourceURL(QByteArray data);
        void getTileSet(QByteArray data);

    public:
        DownloadManager();
        QByteArray xml;
        QString oldurl;
        QImage resultimg;
        bool sameurl;
        QString style;
        QString matrixSet;
        double scale;
        QString format;
        int outy;
        int outx;
        Layer layer;
        TileMatrix* matrix;
        QString getTileUrl;
        void getPixel(int x, int y);
    public slots:
        void getCapabilities(QString url);
        void getTile(double wgs_y_max, double wgs_x_min, double wgs_y_min, double wgs_x_max);
        void loadFinished(QNetworkReply *reply);
};

#endif // DOWNLOADMANAGER_H
