#ifndef XMLPROCESSOR_H
#define XMLPROCESSOR_H

#include <QtCore>
#include <QXmlParseException>

class XmlException : public QException{
public:
    void raise() const override { throw *this; }
    XmlException *clone() const override { return new XmlException(*this); }
};

class XMLElement: public QObject{
    Q_OBJECT
protected:
    friend class XMLProcessor;
    QList<XMLElement*> siblings;
    QMap<QString, XMLElement*> sub_elements;
    QString text;
    QString name;
public:
    QXmlStreamAttributes attributes;
    XMLElement(QXmlStreamReader* reader);
    ~XMLElement();
    QList<XMLElement*>::iterator getSiblingsBegin() { return siblings.begin(); }
    QList<XMLElement*>::iterator getSiblingsEnd() { return siblings.end(); }
    QString getSubText(QString name);
    QString getName() { return name; }
    bool isArray() { return siblings.size() > 0; }
};

class XMLProcessor: public QObject{
    Q_OBJECT
private:
    QByteArray data;
    QXmlStreamReader* reader;
    void navigateTo(QList<QString>& path);
    XMLElement* root;
    QList<XMLElement*> stack;
    void scanAll();
public:
    XMLProcessor(QByteArray data);
    XMLElement* getElementByPath(QStringList& path, XMLElement* curr_element = NULL);
};

#endif // XMLPROCESSOR_H
