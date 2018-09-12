#include <QCoreApplication>
#include "quasarapp.h"
#include <QtDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

void help() {
    qInfo() << "";
    qInfo() << "Usage: CSSCompresor <-css    [params]> [options]";
    qInfo() << "";
    qInfo() << "Options:";
    qInfo() << "   help / h                 : show help.";
    qInfo() << "   -css    [params]         : scan css file ";
    qInfo() << "   -out    [params]         : out colors file ";
    qInfo() << "   replace                  : replace colors in css file";

    qInfo() << "";
    qInfo() << "Example: CSSCompresor -css myStyle.css -out a.out";
    qInfo() << "         CSSCompresor -css myStyle.css -out a.out replace";

}

void replace(const QMap<int, QString>& in, QString& file) {
    for (auto i = in.begin(); i != in.end(); i++) {
        file.replace(i.key(), i.value().size(), i.value());
    }
    qInfo() << "replace done!";

}

QMap<int, QString> scan(const QString& in) {
    auto index = in.indexOf(QRegExp("#[a-fA-F0-9]{3,8}"));
    QMap<int, QString> result;
    QStringList check;

    while (index >= 0) {
        QString Color = in.mid(index, in.indexOf(QRegExp("[;\"\n]"), index) - index);

        if (!check.contains(Color)) {
            result.insert(index, Color);
            check.push_back(Color);
        }

        index = in.indexOf(QRegExp("#[a-fA-F0-9]{3,8}"), index + 1);
    }

    qInfo() << "scan done!";

    return result;
}

QMap<int, QString> scan(const QJsonDocument& in) {

    QMap<int, QString> result;
    if (in.isObject()) {
    }

    auto object = in.object();

    for (auto i = object.begin(); i != object.end(); i++) {
        result[i.key().toInt()] = i.value().toString();
    }

    qInfo() << "scan done!";

    return result;
}

void save(const QString& out, const QString &text) {
    QFile saveF(out);
    if (saveF.open(QIODevice::ReadWrite| QIODevice::Truncate)) {
        QTextStream stream(&saveF);
        stream << text;
        saveF.close();
    }
    qInfo() << "save done!";

}

void saveJson(const QMap<int, QString>& in, const QString &out) {

    QJsonObject obj;
    for (auto i = in.begin(); i != in.end(); ++i) {
        obj.insert(QString::number(i.key()), i.value());
    }
    QJsonDocument saveDoc(obj);

    QFile saveF(out);
    if (saveF.open(QIODevice::ReadWrite| QIODevice::Truncate)) {
        saveF.write(saveDoc.toJson());
        saveF.close();
    }
    qInfo() << "saveJson done!";

}

int main(int argc, char *argv[])
{

    if (!QuasarAppUtils::parseParams(argc, argv)) {
        qWarning() << "wrong parametrs";
        help();
        exit(1);

    };

    if (QuasarAppUtils::isEndable("h") ||
            QuasarAppUtils::isEndable("help")) {
        help();
        return 0;
    }

    if (!QuasarAppUtils::isEndable("css") ||
            !QuasarAppUtils::isEndable("out")) {
        help();
        return 1;
    }

    if (QuasarAppUtils::isEndable("replace")) {

        QFile outFile(QuasarAppUtils::getStrArg("out"));

        if (!outFile.open(QIODevice::ReadOnly)) {
            qCritical() << QuasarAppUtils::getStrArg("out") << "not readatabe!";
            return 2;
        }

        auto doc = QJsonDocument::fromJson(outFile.readAll());
        auto map = scan(doc);

        QString text;

        QFile cssFile(QuasarAppUtils::getStrArg("css"));

        if (!cssFile.open(QIODevice::ReadOnly)) {
            qCritical() << QuasarAppUtils::getStrArg("css") << "not readatabe!";
            return 2;
        }

        text = cssFile.readAll();

        replace(map, text);

        save(QuasarAppUtils::getStrArg("css"), text);

    } else {
        QString text;
        QFile f(QuasarAppUtils::getStrArg("css"));
        if (!f.open(QIODevice::ReadOnly)) {
            qCritical() << QuasarAppUtils::getStrArg("css") << "not readatabe!";
            return 2;
        }

        text = f.readAll();
        f.close();
        auto map = scan(text);
        saveJson(map, QuasarAppUtils::getStrArg("out"));
    }
    qInfo() << "done!";

    return 0;
}
