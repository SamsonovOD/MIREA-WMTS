#include <wmtslib.h>

int main(){
    WMTSLib com;
    //World Gorizont
    com.set_scale(100000000.5);
    com.set_xout(800);
    com.set_yout(400);
    com.loadXML("http://gisgorizont.ru/WMTSCapabilities.xml");
    com.getImage(89, -179, -89, 179).save("result.png");
    com.getPixel(256, 256);
    system("pause");

    //USA OSM
    com.set_scale(5000000.5);
    com.set_style("default");
    com.set_matrixset("default028mm");
    com.loadXML("http://sampleserver6.arcgisonline.com/arcgis/rest/services/World_Street_Map/MapServer/WMTS/1.0.0/WMTSCapabilities.xml");
    com.set_xout(900);
    com.set_yout(500);
    com.getImage(49.791257, -132.078570, 19.453437, -66.254372).save("result.png");
    com.getPixel(256, 256);
    //system("pause");

    //USA Gorizont
    com.set_scale(5000000.5);
    com.set_xout(900);
    com.set_yout(500);
    com.loadXML("http://gisgorizont.ru/WMTSCapabilities.xml");
    com.loadXML("http://gisgorizont.ru/WMTSCapabilities.xml");
    for (int i = 0; i < 10; i++) {
        com.getImage(49.791257, -132.078570+(20*i), 19.453437, -66.254372+(20*i)).save("result.png");
    }
    com.getPixel(256, 256);
    system("pause");

    //Europe OSM
    com.set_scale(5000000.5);
    com.set_style("default");
    com.set_matrixset("default028mm");
    com.set_xout(2400);
    com.set_yout(2400);
    com.loadXML("http://sampleserver6.arcgisonline.com/arcgis/rest/services/World_Street_Map/MapServer/WMTS/1.0.0/WMTSCapabilities.xml");
    com.getImage(64.413208, -19.015425, 34.903686, 34.144555).save("result.png");
    com.getPixel(256, 256);
    system("pause");

    //Japan OSM
    com.set_scale(15000000.5);
    com.set_style("default");
    com.set_matrixset("GoogleMapsCompatible");
    com.set_xout(600);
    com.set_yout(1000);
    com.loadXML("http://sampleserver6.arcgisonline.com/arcgis/rest/services/World_Street_Map/MapServer/WMTS/1.0.0/WMTSCapabilities.xml");
    com.getImage(50.715368, 124.575837, 24.395339, 148.120601).save("result.png");
    com.getPixel(256, 256);
    system("pause");

}
