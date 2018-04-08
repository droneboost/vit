//#include <QSettings>
#include <QtWidgets>
#include <qdebug.h>
#include "mainwindow.h"
#include "mdigvchild.h"
#include "ctkrangeslider.h"

MainWindow::MainWindow()
    : mdiArea(new QMdiArea)
{
    setWindowTitle(tr("VisualInspectTool"));
    setUnifiedTitleAndToolBarOnMac(true);

    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(mdiArea);
    connect(mdiArea, &QMdiArea::subWindowActivated, this, &MainWindow::updateMenus);

    createFileToolbarActions();
    createZoomToolbarActions();
    createDrawToolbarActions();
    createMarkToolbarActions();
    createGraphToolbarActions();
    createViewToolbarActions();
    createLayoutToolbarActions();
    createGreyscaleToolbarActions();
    createMenuActions();

    createStatusBar();
    updateMenus();

    readSettings();
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    mdiArea->closeAllSubWindows();
    if (mdiArea->currentSubWindow()) {
        event->ignore();
    } else {
        writeSettings();
        event->accept();
    }
}

void MainWindow::newFile()
{
    MdiGVChild *child = createMdiChild();
    child->newFile();
    child->show();
}

void MainWindow::open()
{
    const QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
        openFile(fileName);
}

bool MainWindow::openFile(const QString &fileName)
{
    if (QMdiSubWindow *existing = findMdiChild(fileName)) {
        mdiArea->setActiveSubWindow(existing);
        return true;
    }
    const bool succeeded = loadFile(fileName);
    if (succeeded)
        statusBar()->showMessage(tr("File loaded"), 2000);
    return succeeded;
}

bool MainWindow::loadFile(const QString &fileName)
{
    MdiGVChild *child = createMdiChild();
    const bool succeeded = child->loadFile(fileName);
    if (succeeded)
        child->show();
    else
        child->close();
    MainWindow::prependToRecentFiles(fileName);
    return succeeded;
}

static inline QString recentFilesKey() { return QStringLiteral("recentFileList"); }
static inline QString fileKey() { return QStringLiteral("file"); }

static QStringList readRecentFiles(QSettings &settings)
{
    QStringList result;
    const int count = settings.beginReadArray(recentFilesKey());
    for (int i = 0; i < count; ++i) {
        settings.setArrayIndex(i);
        result.append(settings.value(fileKey()).toString());
    }
    settings.endArray();
    return result;
}

static void writeRecentFiles(const QStringList &files, QSettings &settings)
{
    const int count = files.size();
    settings.beginWriteArray(recentFilesKey());
    for (int i = 0; i < count; ++i) {
        settings.setArrayIndex(i);
        settings.setValue(fileKey(), files.at(i));
    }
    settings.endArray();
}

bool MainWindow::hasRecentFiles()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const int count = settings.beginReadArray(recentFilesKey());
    settings.endArray();
    return count > 0;
}

void MainWindow::prependToRecentFiles(const QString &fileName)
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

    const QStringList oldRecentFiles = readRecentFiles(settings);
    QStringList recentFiles = oldRecentFiles;
    recentFiles.removeAll(fileName);
    recentFiles.prepend(fileName);
    if (oldRecentFiles != recentFiles)
        writeRecentFiles(recentFiles, settings);

    setRecentFilesVisible(!recentFiles.isEmpty());
}

void MainWindow::setRecentFilesVisible(bool visible)
{
    recentFileSubMenuAct->setVisible(visible);
    recentFileSeparator->setVisible(visible);
}

void MainWindow::updateRecentFileActions()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

    const QStringList recentFiles = readRecentFiles(settings);
    const int count = qMin(int(MaxRecentFiles), recentFiles.size());
    int i = 0;
    for ( ; i < count; ++i) {
        const QString fileName = QFileInfo(recentFiles.at(i)).fileName();
        recentFileActs[i]->setText(tr("&%1 %2").arg(i + 1).arg(fileName));
        recentFileActs[i]->setData(recentFiles.at(i));
        recentFileActs[i]->setVisible(true);
    }
    for ( ; i < MaxRecentFiles; ++i)
        recentFileActs[i]->setVisible(false);
}

void MainWindow::openRecentFile()
{
    if (const QAction *action = qobject_cast<const QAction *>(sender()))
        openFile(action->data().toString());
}

void MainWindow::save()
{
    if (activeMdiChild() && activeMdiChild()->save())
        statusBar()->showMessage(tr("File saved"), 2000);
}

void MainWindow::saveAs()
{
    MdiGVChild *child = activeMdiChild();
    if (child && child->saveAs()) {
        statusBar()->showMessage(tr("File saved"), 2000);
        MainWindow::prependToRecentFiles(child->currentFile());
    }
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("About MDI"),
            tr("The <b>MDI</b> example demonstrates how to write multiple "
               "document interface applications using Qt."));
}

void MainWindow::todo()
{
   QMessageBox::about(this, tr("TODO"),
            tr("This action is to be implemented "));
}

//Zoom
void MainWindow::hand()
{
    MdiGVChild *child = activeMdiChild();
    if (child)
        child->rotate(90.0);
}
void MainWindow::zoom4x()
{

}
void MainWindow::zoomin()
{

}
void MainWindow::zoomout()
{

}
void MainWindow::zoomregion()
{

}
void MainWindow::zoomfit()
{

}

// Toolbar Draw
void MainWindow::cursor()
{

}
void MainWindow::line()
{

}
void MainWindow::pointer()
{

}
void MainWindow::opencurve()
{

}
void MainWindow::closedcurve()
{

}
void MainWindow::polyline()
{

}
void MainWindow::ploygon()
{

}
void MainWindow::rectangle()
{

}
void MainWindow::ellipse()
{

}
void MainWindow::removexxx()
{

}
void MainWindow::freehand()
{

}
void MainWindow::text()
{

}
void MainWindow::note()
{

}
void MainWindow::stamp()
{

}
void MainWindow::removeobject()
{

}
void MainWindow::drawsetting()
{

}

// Toolbar Mark
void MainWindow::ruler()
{

}
void MainWindow::angle()
{

}
void MainWindow::marksetting()
{

}

// Toolbar Graph
void MainWindow::lineprofile()
{

}
void MainWindow::histogram()
{

}

// Toolbar View
void MainWindow::flipV()
{

}
void MainWindow::flipH()
{

}
void MainWindow::rotate()
{

}

// Toolbar Layout
void MainWindow::oneimage()
{
    mdiArea->cascadeSubWindows();
}
void MainWindow::twoimageV()
{

}
void MainWindow::twoimageH()
{

}
void MainWindow::fourimage()
{
    mdiArea->tileSubWindows();
}

//Greyscale
void MainWindow::flipcolor()
{

}
void MainWindow::updateMenus()
{
    bool hasMdiChild = (activeMdiChild() != 0);
    saveAct->setEnabled(hasMdiChild);
    saveAsAct->setEnabled(hasMdiChild);
    closeAct->setEnabled(hasMdiChild);
    closeAllAct->setEnabled(hasMdiChild);
    tileAct->setEnabled(hasMdiChild);
    cascadeAct->setEnabled(hasMdiChild);
    nextAct->setEnabled(hasMdiChild);
    previousAct->setEnabled(hasMdiChild);
    windowMenuSeparatorAct->setVisible(hasMdiChild);
}

void MainWindow::updateWindowMenu()
{
    windowMenu->clear();
    windowMenu->addAction(closeAct);
    windowMenu->addAction(closeAllAct);
    windowMenu->addSeparator();
    windowMenu->addAction(tileAct);
    windowMenu->addAction(cascadeAct);
    windowMenu->addSeparator();
    windowMenu->addAction(nextAct);
    windowMenu->addAction(previousAct);
    windowMenu->addAction(windowMenuSeparatorAct);

    QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
    windowMenuSeparatorAct->setVisible(!windows.isEmpty());

    for (int i = 0; i < windows.size(); ++i) {
        QMdiSubWindow *mdiSubWindow = windows.at(i);
        MdiGVChild *child = qobject_cast<MdiGVChild *>(mdiSubWindow->widget());

        QString text;
        if (i < 9) {
            text = tr("&%1 %2").arg(i + 1)
                               .arg(child->userFriendlyCurrentFile());
        } else {
            text = tr("%1 %2").arg(i + 1)
                              .arg(child->userFriendlyCurrentFile());
        }
        QAction *action = windowMenu->addAction(text, mdiSubWindow, [this, mdiSubWindow]() {
            mdiArea->setActiveSubWindow(mdiSubWindow);
        });
        action->setCheckable(true);
        action ->setChecked(child == activeMdiChild());
    }
}

MdiGVChild *MainWindow::createMdiChild()
{
    //TODO: only support 4 child windows
    MdiGVChild *child = new MdiGVChild;
    QSize size = mdiArea->size();
    size = size / 2;
    QMdiSubWindow *sw = mdiArea->addSubWindow(child);
    sw->resize(size);
    return child;
}

void MainWindow::createMenuActions()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);

    const QIcon saveAsIcon = QIcon::fromTheme("document-save-as");
    saveAsAct = new QAction(saveAsIcon, tr("Save &As..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(saveAsAct, &QAction::triggered, this, &MainWindow::saveAs);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();

    QMenu *recentMenu = fileMenu->addMenu(tr("Recent..."));
    connect(recentMenu, &QMenu::aboutToShow, this, &MainWindow::updateRecentFileActions);
    recentFileSubMenuAct = recentMenu->menuAction();

    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActs[i] = recentMenu->addAction(QString(), this, &MainWindow::openRecentFile);
        recentFileActs[i]->setVisible(false);
    }

    recentFileSeparator = fileMenu->addSeparator();

    setRecentFilesVisible(MainWindow::hasRecentFiles());

    fileMenu->addAction(tr("Switch layout direction"), this, &MainWindow::switchLayoutDirection);

    fileMenu->addSeparator();

    const QIcon exitIcon = QIcon::fromTheme("application-exit");
    QAction *exitAct = fileMenu->addAction(exitIcon, tr("E&xit"), qApp, &QApplication::closeAllWindows);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    fileMenu->addAction(exitAct);

    windowMenu = menuBar()->addMenu(tr("&Window"));
    connect(windowMenu, &QMenu::aboutToShow, this, &MainWindow::updateWindowMenu);

    closeAct = new QAction(tr("Cl&ose"), this);
    closeAct->setStatusTip(tr("Close the active window"));
    connect(closeAct, &QAction::triggered,
            mdiArea, &QMdiArea::closeActiveSubWindow);

    closeAllAct = new QAction(tr("Close &All"), this);
    closeAllAct->setStatusTip(tr("Close all the windows"));
    connect(closeAllAct, &QAction::triggered, mdiArea, &QMdiArea::closeAllSubWindows);

    tileAct = new QAction(tr("&Tile"), this);
    tileAct->setStatusTip(tr("Tile the windows"));
    connect(tileAct, &QAction::triggered, mdiArea, &QMdiArea::tileSubWindows);

    cascadeAct = new QAction(tr("&Cascade"), this);
    cascadeAct->setStatusTip(tr("Cascade the windows"));
    connect(cascadeAct, &QAction::triggered, mdiArea, &QMdiArea::cascadeSubWindows);

    nextAct = new QAction(tr("Ne&xt"), this);
    nextAct->setShortcuts(QKeySequence::NextChild);
    nextAct->setStatusTip(tr("Move the focus to the next window"));
    connect(nextAct, &QAction::triggered, mdiArea, &QMdiArea::activateNextSubWindow);

    previousAct = new QAction(tr("Pre&vious"), this);
    previousAct->setShortcuts(QKeySequence::PreviousChild);
    previousAct->setStatusTip(tr("Move the focus to the previous "
                                 "window"));
    connect(previousAct, &QAction::triggered, mdiArea, &QMdiArea::activatePreviousSubWindow);

    windowMenuSeparatorAct = new QAction(this);
    windowMenuSeparatorAct->setSeparator(true);

    updateWindowMenu();

    menuBar()->addSeparator();

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

    QAction *aboutAct = helpMenu->addAction(tr("&About"), this, &MainWindow::about);
    aboutAct->setStatusTip(tr("Show the application's About box"));

    QAction *aboutQtAct = helpMenu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
}

void MainWindow::createFileToolbarActions()
{
    fileToolBar = addToolBar(tr("FileToolBar"));

    const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(":/images/new.png"));
    newAct = new QAction(newIcon, tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
    fileToolBar->addAction(newAct);

    const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/images/open.png"));
    QAction *openAct = new QAction(openIcon, tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);
    fileToolBar->addAction(openAct);

    const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));
    saveAct = new QAction(saveIcon, tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);
    fileToolBar->addAction(saveAct);
}

void MainWindow::createZoomToolbarActions()
{
    zoomToolBar = addToolBar(tr("ZoomToolBar"));

    const QIcon handIcon = QIcon::fromTheme("document-new", QIcon(":/images/hand.png"));
    handAct = new QAction(handIcon, tr("&Hand"), this);
    handAct->setStatusTip(tr("Toggle the image"));
    connect(handAct, &QAction::triggered, this, &MainWindow::hand);
    zoomToolBar->addAction(handAct);

    const QIcon zoom4xIcon = QIcon::fromTheme("document-new", QIcon(":/images/zoom4x.png"));
    zoom4xAct = new QAction(zoom4xIcon, tr("&Zoom4x"), this);
    zoom4xAct->setStatusTip(tr("Zoom 4times"));
    connect(zoom4xAct, &QAction::triggered, this, &MainWindow::zoom4x);
    zoomToolBar->addAction(zoom4xAct);

    const QIcon zoominIcon = QIcon::fromTheme("document-new", QIcon(":/images/zoomin.png"));
    zoominAct = new QAction(zoominIcon, tr("&ZoomIn"), this);
    zoominAct->setStatusTip(tr("Zoom In"));
    connect(zoominAct, &QAction::triggered, this, &MainWindow::zoomin);
    zoomToolBar->addAction(zoominAct);

    const QIcon zoomoutIcon = QIcon::fromTheme("document-new", QIcon(":/images/zoomout.png"));
    zoomoutAct = new QAction(zoomoutIcon, tr("&ZoomOut"), this);
    zoomoutAct->setStatusTip(tr("Zoom out"));
    connect(zoomoutAct, &QAction::triggered, this, &MainWindow::zoomout);
    zoomToolBar->addAction(zoomoutAct);

    const QIcon zoomregionIcon = QIcon::fromTheme("document-new", QIcon(":/images/zoomregion.png"));
    zoomregionAct = new QAction(zoomregionIcon, tr("&ZoomRegion"), this);
    zoomregionAct->setStatusTip(tr("Zoom Region"));
    connect(zoomregionAct, &QAction::triggered, this, &MainWindow::zoomregion);
    zoomToolBar->addAction(zoomregionAct);

    const QIcon zoomfitIcon = QIcon::fromTheme("document-new", QIcon(":/images/zoomfit.png"));
    zoomfitAct = new QAction(zoomfitIcon, tr("&ZoomFit"), this);
    zoomfitAct->setStatusTip(tr("Zoom Fit"));
    connect(zoomfitAct, &QAction::triggered, this, &MainWindow::zoomfit);
    zoomToolBar->addAction(zoomfitAct);
}

void MainWindow::createDrawToolbarActions()
{
    drawToolBar = addToolBar(tr("DrawToolBar"));

    const QIcon cursorIcon = QIcon::fromTheme("document-new", QIcon(":/images/cursor.png"));
    cursorAct = new QAction(cursorIcon, tr("&Cursor"), this);
    cursorAct->setStatusTip(tr("Cursor"));
    connect(cursorAct, &QAction::triggered, this, &MainWindow::cursor);
    drawToolBar->addAction(cursorAct);

    const QIcon lineIcon = QIcon::fromTheme("document-new", QIcon(":/images/line.png"));
    lineAct = new QAction(lineIcon, tr("&Line"), this);
    lineAct->setStatusTip(tr("Draw Line"));
    connect(lineAct, &QAction::triggered, this, &MainWindow::line);
    drawToolBar->addAction(lineAct);

    const QIcon pointerIcon = QIcon::fromTheme("document-new", QIcon(":/images/pointer.png"));
    pointerAct = new QAction(pointerIcon, tr("&Pointer"), this);
    pointerAct->setStatusTip(tr("Draw Pointer"));
    connect(pointerAct, &QAction::triggered, this, &MainWindow::pointer);
    drawToolBar->addAction(pointerAct);

    drawToolBar->addSeparator();

    const QIcon opencurveIcon = QIcon::fromTheme("document-new", QIcon(":/images/opencurve.png"));
    opencurveAct = new QAction(opencurveIcon, tr("&OpenCurve"), this);
    opencurveAct->setStatusTip(tr("Draw Open Curve"));
    connect(opencurveAct, &QAction::triggered, this, &MainWindow::opencurve);
    drawToolBar->addAction(opencurveAct);

    const QIcon closedcurveIcon = QIcon::fromTheme("document-new", QIcon(":/images/closedcurve.png"));
    closedcurveAct = new QAction(closedcurveIcon, tr("&ClosedCurve"), this);
    closedcurveAct->setStatusTip(tr("Draw Closed Curve"));
    connect(closedcurveAct, &QAction::triggered, this, &MainWindow::closedcurve);
    drawToolBar->addAction(closedcurveAct);

    const QIcon polylineIcon = QIcon::fromTheme("document-new", QIcon(":/images/polyline.png"));
    polylineAct = new QAction(polylineIcon, tr("&Ployline"), this);
    polylineAct->setStatusTip(tr("Draw Ployline"));
    connect(polylineAct, &QAction::triggered, this, &MainWindow::polyline);
    drawToolBar->addAction(polylineAct);

    const QIcon ploygonIcon = QIcon::fromTheme("document-new", QIcon(":/images/polygon.png"));
    ploygonAct = new QAction(ploygonIcon, tr("&Ploygon"), this);
    ploygonAct->setStatusTip(tr("Draw Ploygon"));
    connect(ploygonAct, &QAction::triggered, this, &MainWindow::ploygon);
    drawToolBar->addAction(ploygonAct);

    const QIcon rectangleIcon = QIcon::fromTheme("document-new", QIcon(":/images/rectangle.png"));
    rectangleAct = new QAction(rectangleIcon, tr("&Rectangle"), this);
    rectangleAct->setStatusTip(tr("Draw Rectangle"));
    connect(rectangleAct, &QAction::triggered, this, &MainWindow::rectangle);
    drawToolBar->addAction(rectangleAct);

    const QIcon ellipseIcon = QIcon::fromTheme("document-new", QIcon(":/images/ellipse.png"));
    ellipseAct = new QAction(ellipseIcon, tr("&Ellipse"), this);
    ellipseAct->setStatusTip(tr("Draw Ellipse"));
    connect(ellipseAct, &QAction::triggered, this, &MainWindow::ellipse);
    drawToolBar->addAction(ellipseAct);

    drawToolBar->addSeparator();

    const QIcon removexxxIcon = QIcon::fromTheme("document-new", QIcon(":/images/removexxx.png"));
    removexxxAct = new QAction(removexxxIcon, tr("&RemoveXXX"), this);
    removexxxAct->setStatusTip(tr("Remove XXX"));
    connect(removexxxAct, &QAction::triggered, this, &MainWindow::removexxx);
    drawToolBar->addAction(removexxxAct);

    const QIcon freehandIcon = QIcon::fromTheme("document-new", QIcon(":/images/freehand.png"));
    freehandAct = new QAction(freehandIcon, tr("&FreeHand"), this);
    freehandAct->setStatusTip(tr("Free Hand"));
    connect(freehandAct, &QAction::triggered, this, &MainWindow::freehand);
    drawToolBar->addAction(freehandAct);

    drawToolBar->addSeparator();

    const QIcon textIcon = QIcon::fromTheme("document-new", QIcon(":/images/text.png"));
    textAct = new QAction(textIcon, tr("&Text"), this);
    textAct->setStatusTip(tr("Add Text"));
    connect(textAct, &QAction::triggered, this, &MainWindow::text);
    drawToolBar->addAction(textAct);

    const QIcon noteIcon = QIcon::fromTheme("document-new", QIcon(":/images/note.png"));
    noteAct = new QAction(noteIcon, tr("&Note"), this);
    noteAct->setStatusTip(tr("Add Note"));
    connect(noteAct, &QAction::triggered, this, &MainWindow::note);
    drawToolBar->addAction(noteAct);

    drawToolBar->addSeparator();

    const QIcon removeobjectIcon = QIcon::fromTheme("document-new", QIcon(":/images/removeobject.png"));
    removeobjectAct = new QAction(removeobjectIcon, tr("&RemoveObject"), this);
    removeobjectAct->setStatusTip(tr("Remove Object"));
    connect(removeobjectAct, &QAction::triggered, this, &MainWindow::removeobject);
    drawToolBar->addAction(removeobjectAct);

    const QIcon drawsettingIcon = QIcon::fromTheme("document-new", QIcon(":/images/drawsetting.png"));
    drawsettingAct = new QAction(drawsettingIcon, tr("&DrawSetting"), this);
    drawsettingAct->setStatusTip(tr("Draw Setting"));
    connect(drawsettingAct, &QAction::triggered, this, &MainWindow::drawsetting);
    drawToolBar->addAction(drawsettingAct);
}

void MainWindow::createMarkToolbarActions()
{
    markToolBar = addToolBar(tr("MarkToolBar"));

    const QIcon rulerIcon = QIcon::fromTheme("document-new", QIcon(":/images/ruler.png"));
    rulerAct = new QAction(rulerIcon, tr("&Ruler"), this);
    rulerAct->setStatusTip(tr("Ruler Mark"));
    connect(rulerAct, &QAction::triggered, this, &MainWindow::ruler);
    markToolBar->addAction(rulerAct);

    const QIcon angleIcon = QIcon::fromTheme("document-new", QIcon(":/images/angle.png"));
    angleAct = new QAction(angleIcon, tr("&Angle"), this);
    angleAct->setStatusTip(tr("Angle Mark"));
    connect(angleAct, &QAction::triggered, this, &MainWindow::angle);
    markToolBar->addAction(angleAct);

    const QIcon marksettingIcon = QIcon::fromTheme("document-new", QIcon(":/images/marksetting.png"));
    marksettingAct = new QAction(marksettingIcon, tr("&MarkSetting"), this);
    marksettingAct->setStatusTip(tr("Mark Setting"));
    connect(marksettingAct, &QAction::triggered, this, &MainWindow::marksetting);
    markToolBar->addAction(marksettingAct);
}

void MainWindow::createGraphToolbarActions()
{
    graphToolBar = addToolBar(tr("GraphToolBar"));

    const QIcon lineprofileIcon = QIcon::fromTheme("document-new", QIcon(":/images/lineprofile.png"));
    lineprofileAct = new QAction(lineprofileIcon, tr("&LineProfile"), this);
    lineprofileAct->setStatusTip(tr("Line Profile"));
    connect(lineprofileAct, &QAction::triggered, this, &MainWindow::lineprofile);
    graphToolBar->addAction(lineprofileAct);

    const QIcon histogramIcon = QIcon::fromTheme("document-new", QIcon(":/images/histogram.png"));
    histogramAct = new QAction(histogramIcon, tr("&Histogram"), this);
    histogramAct->setStatusTip(tr("Histogram"));
    connect(histogramAct, &QAction::triggered, this, &MainWindow::histogram);
    graphToolBar->addAction(histogramAct);
}

void MainWindow::createViewToolbarActions()
{
    viewToolBar = addToolBar(tr("ViewToolBar"));

    const QIcon flipVIcon = QIcon::fromTheme("document-new", QIcon(":/images/flipV.png"));
    flipVAct = new QAction(flipVIcon, tr("&Mirror"), this);
    flipVAct->setStatusTip(tr("Mirror Image"));
    connect(flipVAct, &QAction::triggered, this, &MainWindow::flipV);
    viewToolBar->addAction(flipVAct);

    const QIcon flipHIcon = QIcon::fromTheme("document-new", QIcon(":/images/flipH.png"));
    flipHAct = new QAction(flipHIcon, tr("&Flip"), this);
    flipHAct->setStatusTip(tr("Flip Image"));
    connect(flipHAct, &QAction::triggered, this, &MainWindow::flipH);
    viewToolBar->addAction(flipHAct);

    const QIcon rotateIcon = QIcon::fromTheme("document-new", QIcon(":/images/rotate.png"));
    rotateAct = new QAction(rotateIcon, tr("&Rotate"), this);
    rotateAct->setStatusTip(tr("Rotate Image"));
    connect(rotateAct, &QAction::triggered, this, &MainWindow::rotate);
    viewToolBar->addAction(rotateAct);
}

void MainWindow::createLayoutToolbarActions()
{
    layoutToolBar = addToolBar(tr("LayoutToolBar"));

    const QIcon oneimageIcon = QIcon::fromTheme("document-new", QIcon(":/images/oneimage.png"));
    oneimageAct = new QAction(oneimageIcon, tr("&OneImage"), this);
    oneimageAct->setStatusTip(tr("One Image"));
    connect(oneimageAct, &QAction::triggered, this, &MainWindow::oneimage);
    layoutToolBar->addAction(oneimageAct);

    const QIcon twoimageVIcon = QIcon::fromTheme("document-new", QIcon(":/images/twoimageV.png"));
    twoimageVAct = new QAction(twoimageVIcon, tr("&TwoImageV"), this);
    twoimageVAct->setStatusTip(tr("Two ImageV"));
    connect(twoimageVAct, &QAction::triggered, this, &MainWindow::twoimageV);
    layoutToolBar->addAction(twoimageVAct);

    const QIcon twoimageHIcon = QIcon::fromTheme("document-new", QIcon(":/images/twoimageH.png"));
    twoimageHAct = new QAction(twoimageHIcon, tr("&TwoImageH"), this);
    twoimageHAct->setStatusTip(tr("Two ImageH"));
    connect(twoimageHAct, &QAction::triggered, this, &MainWindow::twoimageH);
    layoutToolBar->addAction(twoimageHAct);

    const QIcon fourimageIcon = QIcon::fromTheme("document-new", QIcon(":/images/fourimage.png"));
    fourimageAct = new QAction(fourimageIcon, tr("&FourImage"), this);
    fourimageAct->setStatusTip(tr("Four Image"));
    connect(fourimageAct, &QAction::triggered, this, &MainWindow::fourimage);
    layoutToolBar->addAction(fourimageAct);
}

void MainWindow::createGreyscaleToolbarActions()
{
    greyscaleToolBar = addToolBar(tr("GreyScale"));

    rangeSlider = new ctkRangeSlider(Qt::Horizontal);
    rangeSlider->setMinimum(0);
    rangeSlider->setMaximum(65535);
    rangeSlider->setMaximumPosition(65535);
    rangeSlider->setMaximumWidth(250);
    greyscaleToolBar->addWidget(rangeSlider);
    connect(rangeSlider, &ctkRangeSlider::valuesChanged, this, &MainWindow::grayscaleChanged);

    const QIcon flipcolorIcon = QIcon::fromTheme("document-new", QIcon(":/images/flipcolor.png"));
    flipcolorAct = new QAction(flipcolorIcon, tr("&FlipColor"), this);
    flipcolorAct->setStatusTip(tr("FlipColor"));
    connect(flipcolorAct, &QAction::triggered, this, &MainWindow::flipcolor);
    greyscaleToolBar->addAction(flipcolorAct);

    minTxt = new QLineEdit();
    minTxt->setText(QString("%1").arg(QString::number(rangeSlider->minimumValue())));
    minTxt->setMaximumWidth(50);
    minTxt->setInputMask(tr("99999"));
    greyscaleToolBar->addWidget(minTxt);

    maxTxt = new QLineEdit();
    maxTxt->setText(QString("%1").arg(QString::number(rangeSlider->maximumValue())));
    maxTxt->setMaximumWidth(50);
    maxTxt->setInputMask(tr("99999"));
    greyscaleToolBar->addWidget(maxTxt);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }
}

void MainWindow::writeSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("geometry", saveGeometry());
}

MdiGVChild *MainWindow::activeMdiChild() const
{
    if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
        return qobject_cast<MdiGVChild *>(activeSubWindow->widget());
    return 0;
}

QMdiSubWindow *MainWindow::findMdiChild(const QString &fileName) const
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    foreach (QMdiSubWindow *window, mdiArea->subWindowList()) {
        MdiGVChild *mdiChild = qobject_cast<MdiGVChild *>(window->widget());
        if (mdiChild->currentFile() == canonicalFilePath)
            return window;
    }
    return 0;
}

void MainWindow::switchLayoutDirection()
{
    if (layoutDirection() == Qt::LeftToRight)
        QGuiApplication::setLayoutDirection(Qt::RightToLeft);
    else
        QGuiApplication::setLayoutDirection(Qt::LeftToRight);
}

void MainWindow::grayscaleChanged(int min, int max)
{
    QString str =  QString("range slider value(%1, %2)").arg(QString::number(min), QString::number(max));
    qDebug(str.toLatin1());
    minTxt->setText(QString("%1").arg(QString::number(min)));
    maxTxt->setText(QString("%1").arg(QString::number(max)));
}
