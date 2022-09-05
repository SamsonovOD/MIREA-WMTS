#include "downloadmanager.h"
#include "xmlprocessor.h"

DownloadManager::DownloadManager(){
    connect(&manager, &QNetworkAccessManager::finished, this, &DownloadManager::loadFinished, Qt::UniqueConnection);
}

void DownloadManager::getCapabilities(QString url){
    if (oldurl != url){
        oldurl = url;
        Download *download = new Download(Download::XML, url);
        doLoad(download);
    } else {
        getTileSet(xml);
    }
}

void DownloadManager::doLoad(Download* download){
    QLoggingCategory::setFilterRules("qt.network.ssl.warning=false");
    //qDebug() << "Download Url:" << download->url;
    QNetworkRequest request(download->url);
    QNetworkReply *reply = manager.get(request);
    currentDownloads[reply] = download;
}

void DownloadManager::loadFinished(QNetworkReply *reply){
    QUrl url = reply->url();
    if (reply->error()){
        fprintf(stderr, "Download of %s failed: %s\n", url.toEncoded().constData(), qPrintable(reply->errorString()));
    }else{
        if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() != 200){
            fputs("Request was redirected.\n", stderr);
        }else{
            qDebug() << "downloaded from" << reply->url();
            Download* download = currentDownloads[reply];
            if(download->type == Download::XML){
                xml = reply->peek(reply->bytesAvailable());
                getTileSet(xml);
                delete currentDownloads[reply];
                currentDownloads.remove(reply);
                reply->deleteLater();
                QApplication::instance()->quit();
            } else {
                QImage img;
                //qDebug() << "drawImage to canvas xy" << download->xpos << download->ypos;
                img.loadFromData(reply->peek(reply->bytesAvailable()), format.toLocal8Bit());
                painter->drawImage(download->xpos, download->ypos, img);
                delete currentDownloads[reply];
                currentDownloads.remove(reply);
                reply->deleteLater();
                if (currentDownloads.isEmpty()){
                    qDebug() << "End app";
                    if(outx > 0 && outy > 0){
                        qDebug() << "Apply resize";
                        resultimg = canvas->scaled(outx, outy, Qt::IgnoreAspectRatio);
                    } else {
                        resultimg = *canvas;
                    }
                    painter->end();
                    QApplication::instance()->quit();
                }
            }
        }
    }
}

void fillBox(BoundingBox &bbox, XMLElement *box) {
    QStringList corner;
    corner = box->getSubText("LowerCorner").split(" ");
    bbox.lx = corner[0].toDouble();
    bbox.ly = corner[1].toDouble();
    corner = box->getSubText("UpperCorner").split(" ");
    bbox.ux = corner[0].toDouble();
    bbox.uy = corner[1].toDouble();
}

bool lessThan(TileMatrix *one, TileMatrix *two) { return one->scaleDenominator < two->scaleDenominator; }

void DownloadManager::getTileSet(QByteArray data){
    XMLProcessor xml(data);
    QStringList path;

    path = QStringList({"Capabilities", "Contents", "Layer", "WGS84BoundingBox"});
    XMLElement *box = xml.getElementByPath(path);
    fillBox(layer.wgs, box);

    try {
        path = QStringList({"Capabilities", "Contents", "Layer", "BoundingBox"});
        XMLElement* box = xml.getElementByPath(path);
        fillBox(layer.crs, box);
    } catch (XmlException &e) {
        Q_UNUSED(e)
        layer.crs = layer.wgs;
    }

    path = QStringList({"Capabilities", "Contents", "Layer", "ResourceURL"});
    XMLElement *resourceURL = xml.getElementByPath(path);
    if(resourceURL->attributes.hasAttribute("template")){
        getTileUrl = resourceURL->attributes.value("template").toString();
    } else {
        qDebug() << "No URL template";
        return;
    }
    if(resourceURL->attributes.hasAttribute("format")){
        DownloadManager::format = resourceURL->attributes.value("format").toString();
    } else {
        DownloadManager::format = "image/png";
    }

    path = QStringList({"Capabilities", "Contents", "TileMatrixSet"});
    XMLElement *tileMatrixSet = xml.getElementByPath(path);
    if(tileMatrixSet->isArray()) {
        bool found = false;
        for(auto i = tileMatrixSet->getSiblingsBegin(); i != tileMatrixSet->getSiblingsEnd(); ++i) {
            qDebug() << "Found matrix set " << (*i)->getSubText("Identifier");
            if((*i)->getSubText("Identifier") == matrixSet){
                tileMatrixSet = *i;
                found = true;
                break;
            }
        }
        if(!found) {
            qDebug() << "Cannot find tile matrix set";
            return;
        }
    } else if(tileMatrixSet->getSubText("Identifier") == matrixSet){
        qDebug() << "Cannot find tile matrix set";
        return;
    }

    auto supportedCRS = tileMatrixSet->getSubText("SupportedCRS");
    qDebug() << "CRS" << supportedCRS;
    if (supportedCRS.endsWith("4326")) {
        layer.calculator = &layer.wgsclass;
    } else if(supportedCRS.endsWith("3395") || supportedCRS.endsWith("3857")) {
        layer.calculator = &layer.mercator;
    } else {
        layer.calculator = &layer.google;
    }

    path = QStringList({"TileMatrix"});
    XMLElement *tileMatrix = xml.getElementByPath(path, tileMatrixSet);
    QVector<TileMatrix*> vec;
    for(auto i = tileMatrix->getSiblingsBegin(); i != tileMatrix->getSiblingsEnd(); ++i){
        auto tmP = new TileMatrix();
        tmP->identifier = (*i)->getSubText("Identifier");
        tmP->scaleDenominator = (*i)->getSubText("ScaleDenominator").toDouble();
        QStringList corner = (*i)->getSubText("TopLeftCorner").split(" ");
        tmP->topLeftCornerX = corner.value(0).toDouble();
        tmP->topLeftCornerY = corner.value(1).toDouble();
        tmP->tileWidth = (*i)->getSubText("TileWidth").toInt();
        tmP->tileHeight = (*i)->getSubText("TileHeight").toInt();
        tmP->matrixWidth = (*i)->getSubText("MatrixWidth").toInt();
        tmP->matrixHeight = (*i)->getSubText("MatrixHeight").toInt();
        tmP->debug();
        vec.push_back(tmP);
    }

    std::sort(vec.begin(), vec.end(), lessThan);
    int i;
    for(i = 0; i < vec.size(); i++){
        if(DownloadManager::scale < vec[i]->scaleDenominator) {
            matrix = vec[i];
            break;
        }
    }
    if(i == vec.size()){
        matrix = vec[i-1];
    }
    matrix->debug();
}

void DownloadManager::getTile(double wgs_y_max, double wgs_x_min, double wgs_y_min, double wgs_x_max){
    if (oldurl == nullptr){
        qDebug() << "no xml";
        return;
    }
    qDebug() << "input: xmin" << wgs_x_min << "xmax" << wgs_x_max << "ymin"  << wgs_y_min << "ymax"  << wgs_y_max << "width" << wgs_x_max-wgs_x_min << "height" << wgs_y_max-wgs_y_min;

    double crs_x1 =  layer.wgs2crsX(wgs_x_min);
    double crs_y1 =  layer.wgs2crsY(wgs_y_min);
    double crs_x2 =  layer.wgs2crsX(wgs_x_max);
    double crs_y2 =  layer.wgs2crsY(wgs_y_max);
    double pix_x1 =  matrix->crs2pixX(crs_x1, layer);
    double pix_x2 =  matrix->crs2pixX(crs_x2, layer);
    double pix_y1 =  matrix->crs2pixY(crs_y1, layer);
    double pix_y2 =  matrix->crs2pixY(crs_y2, layer);

    qDebug() << "CRS: x" << crs_x1 << crs_x2 << "y"  << crs_y1 << crs_y2 << "width" << crs_x2-crs_x1 << "height" << crs_y2-crs_y1;
    qDebug() << "pixels: p1" << pix_x1 << pix_y2 << "p2"  << pix_x2 << pix_y1 << "width" << pix_x2-pix_x1 << "height" << pix_y1-pix_y2;

    canvas = new QImage(qRound(abs(pix_x2 - pix_x1)), qRound(abs(pix_y1 - pix_y2)), QImage::Format_ARGB32);
    canvas->fill(0);
    painter = new QPainter(canvas);

    for(int x = 0; x <= matrix->matrixWidth; x++){
        for (int y = 0; y <= matrix->matrixHeight; y++) {
            QString getCertainTileUrl = getTileUrl;
            auto x1 = x * matrix->tileWidth;
            auto x2 = (x + 1)*matrix->tileWidth;
            auto y1 = y * matrix->tileHeight;
            auto y2 = (y + 1) * matrix->tileHeight;
            //qDebug() << "current tile: x" << x1 << x2 << "y"  << y1 << y2;
            //qDebug() << "comparing to: p1" << pix_x1 << pix_y2 << "p2"  << pix_x2 << pix_y1;
            if(pix_x1 >= x2) {
                //qDebug() << "target tile is right";
                continue;
            }
            if(pix_x2 < x1) {
                //qDebug() << "target tile is left";
                continue;
            }
            if(pix_y2 >= y2) {
                //qDebug() << "target tile is down";
                continue;
            }
            if(pix_y1 < y1) {
                //qDebug() << "target tile is up";
                continue;
            }
            //qDebug() << "accepting this tile: x" << x1 << x2 << "y"  << y1 << y2;

            //qDebug() << "Template:" << getCertainTileUrl;
            getCertainTileUrl.replace(QString("{Style}"), style);
            getCertainTileUrl.replace(QString("{TileMatrixSet}"), matrixSet);
            getCertainTileUrl.replace(QString("{TileMatrix}"), matrix->identifier);
            getCertainTileUrl.replace(QString("{TileCol}"), QString::number(x));
            getCertainTileUrl.replace(QString("{TileRow}"), QString::number(y));
            //qDebug() << "Url: " << getCertainTileUrl << x << y;
            Download *download = new Download(Download::TILE, getCertainTileUrl, qRound(x*matrix->tileWidth - pix_x1), qRound(y*matrix->tileHeight - pix_y2));
            doLoad(download);
        }
    }
}

void DownloadManager::getPixel(int x, int y){
    if (oldurl == nullptr){
        qDebug() << "no xml";
        return;
    }
    qDebug() << "Pix to WGS" << layer.crs2wgsX(matrix->pix2crsX(x, layer)) << layer.crs2wgsX(matrix->pix2crsY(y, layer));
}
