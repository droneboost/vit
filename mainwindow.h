#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMdiArea>

class MdiGVChild;
class QAction;
class QMenu;
class QMdiArea;
class QMdiSubWindow;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

    bool openFile(const QString &fileName);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void newFile();
    void open();
    void save();
    void saveAs();
    void updateRecentFileActions();
    void openRecentFile();
    void about();
    void updateMenus();
    void updateWindowMenu();
    void switchLayoutDirection();
    void todo();
    void hand();
    void zoomin();
    void zoomout();
    void zoom2();
    void zoom4();
    void cursor();
    void line();
    void arrow();
    void freeline();
    void freepolygon();
    void breakline();
    void polygon();
    void circle();
    MdiGVChild *createMdiChild();

private:
    enum { MaxRecentFiles = 5 };

    void createActions();
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
    QAction *newAct;
    QAction *saveAct;
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
    QAction *handAct;
    QAction *zoominAct;
    QAction *zoomoutAct;
    QAction *zoom2Act;
    QAction *zoom4Act;
    QAction *cursorAct;
    QAction *lineAct;
    QAction *arrowAct;
    QAction *freelineAct;
    QAction *freepolygonAct;
    QAction *breaklineAct;
    QAction *polygonAct;
    QAction *circleAct;
    QAction *windowMenuSeparatorAct;
};


#endif // MAINWINDOW_H
