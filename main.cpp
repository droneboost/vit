#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QStyleFactory>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(resources);

    //std::setlocale(LC_ALL, "en_US.UTF-8");
    QLocale::setDefault(QLocale(QLocale::Chinese, QLocale::China));
    QApplication app(argc, argv);
    QApplication::setStyle(QStyleFactory::create("windows")); //other styles: windows, Fusion, windowsxp, windowsvista, macintosh
    QCoreApplication::setApplicationName("VisualInspectTool");
    QCoreApplication::setOrganizationName("Bithollow");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);
    QCommandLineParser parser;
    parser.setApplicationDescription("Bithollow advanced image processing tool");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", "The file to open.");
    parser.process(app);

    MainWindow mainWin;
    foreach (const QString &fileName, parser.positionalArguments())
        mainWin.openFile(fileName);
    mainWin.show();
    return app.exec();
}
