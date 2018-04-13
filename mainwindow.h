#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMdiArea>

class MdiGVChild;
class QAction;
class QMenu;
class QMdiArea;
class QMdiSubWindow;
class QLineEdit;
class ctkRangeSlider;
class QLabel;

/*
namespace
{
    const int MaxToobarActions = 20;
    const int MaxToolbarActionTableItems = 10;
    const int MaxRecentFiles = 5;
}

class ToobarActionMap
{
 public:
    QToolBar* toobar;
    QAction* actions[MaxToobarActions];

};
*/

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

    bool openFile(const QString &fileName);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    MdiGVChild *createMdiChild();
    void saveAs();
    void updateRecentFileActions();
    void openRecentFile();
    void about();
    void updateMenus();
    void updateWindowMenu();
    void switchLayoutDirection();
    void todo();

    // Toolbar File
    void newFile();
    void open();
    void save();

    // Toolbar Zoom
    void hand();
    void zoom4x();
    void zoomin();
    void zoomout();
    void zoomregion();
    void zoomfit();

    // Toolbar Draw
    void cursor();
    void line();
    void pointer();
    void opencurve();
    void closedcurve();
    void polyline();
    void ploygon();
    void rectangle();
    void ellipse();
    void removexxx();
    void freehand();
    void text();
    void note();
    void stamp();
    void removeobject();
    void drawsetting();

    // Toolbar Mark
    void ruler();
    void angle();
    void marksetting();

    // Toolbar Graph
    void lineprofile();
    void histogram();

    // Toolbar View
    void flipV();
    void flipH();
    void rotate();

    // Toolbar Layout
    void oneimage();
    void twoimageV();
    void twoimageH();
    void fourimage();

    // Toolbar Greyscale
    void grayscaleChanged(int min, int max);
    void flipcolor();

private:
    enum { MaxRecentFiles = 5 };

    void createFileToolbarActions();
    void createZoomToolbarActions();
    void createDrawToolbarActions();
    void createMarkToolbarActions();
    void createGraphToolbarActions();
    void createViewToolbarActions();
    void createLayoutToolbarActions();
    void createGreyscaleToolbarActions();
    void createMenuActions();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool loadFile(const QString &fileName);
    static bool hasRecentFiles();
    void prependToRecentFiles(const QString &fileName);
    void setRecentFilesVisible(bool visible);
    MdiGVChild *activeMdiChild() const;
    QMdiSubWindow *findMdiChild(const QString &fileName) const;

    QMdiArea *mdiArea;
    QMenu *windowMenu;
    QMenu *fileMenu;

    QToolBar *fileToolBar;
      QAction *newAct;
      QAction *openAct;
      QAction *saveAct;

    QToolBar *zoomToolBar;
      QAction *handAct;
      QAction *zoom4xAct;
      QAction *zoominAct;
      QAction *zoomoutAct;
      QAction *zoomregionAct;
      QAction *zoomfitAct;

    QToolBar *drawToolBar;
      QAction *cursorAct;
      QAction *lineAct;
      QAction *pointerAct;
      // <seperator>
      QAction *opencurveAct;
      QAction *closedcurveAct;
      QAction *polylineAct;
      QAction *ploygonAct;
      QAction *rectangleAct;
      QAction *ellipseAct;
      // <seperator>
      QAction *removexxxAct;
      QAction *freehandAct;
      // <seperator>
      QAction *textAct;
      QAction *noteAct;
      QAction *stampAct;
      // <seperator>
      QAction *removeobjectAct;
      QAction *drawsettingAct;

    QToolBar *markToolBar;
      QAction *rulerAct;
      QAction *angleAct;
      QAction *marksettingAct;

    QToolBar *graphToolBar;
      QAction *lineprofileAct;
      QAction *histogramAct;

    QToolBar *viewToolBar;
      QAction *flipVAct;
      QAction *flipHAct;
      QAction *rotateAct;

    QToolBar *layoutToolBar;
      QAction *oneimageAct;
      QAction *twoimageVAct;
      QAction *twoimageHAct;
      QAction *fourimageAct;

    QToolBar *greyscaleToolBar;
      ctkRangeSlider *rangeSlider;
      QLineEdit *minTxt;
      QLineEdit *maxTxt;
      QAction *flipcolorAct;
      QLabel *greyscaleLbl;

    QAction *saveAsAct;
    QAction *recentFileActs[MaxRecentFiles];
    QAction *recentFileSeparator;
    QAction *recentFileSubMenuAct;
    QAction *pasteAct;
    QAction *closeAct;
    QAction *closeAllAct;
    QAction *tileAct;
    QAction *cascadeAct;
    QAction *nextAct;
    QAction *previousAct;

    QAction *windowMenuSeparatorAct;
};


#endif // MAINWINDOW_H
