//#include <QSettings>
#include <QtWidgets>

#include "mainwindow.h"
#include "mdigvchild.h"
#include "ctkrangeslider.h"

MainWindow::MainWindow()
    : mdiArea(new QMdiArea)
{
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(mdiArea);
    connect(mdiArea, &QMdiArea::subWindowActivated,
                this, &MainWindow::updateMenus);

    createActions();
    createStatusBar();
    updateMenus();

    readSettings();

    setWindowTitle(tr("VisualInspectTool"));
    setUnifiedTitleAndToolBarOnMac(true);
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

void MainWindow::hand()
{
    MdiGVChild *child = activeMdiChild();
    if (child)
        child->rotate(90.0);
}

void MainWindow::zoomin() { hand(); }
void MainWindow::zoomout() { todo(); }
void MainWindow::zoom2() { todo(); }
void MainWindow::zoom4() { todo(); }

void MainWindow::cursor()
{
    mdiArea->cascadeSubWindows();
}

void MainWindow::line()
{
    mdiArea->tileSubWindows();
}

void MainWindow::arrow() { todo(); }
void MainWindow::freeline() { todo(); }
void MainWindow::freepolygon() { todo(); }
void MainWindow::breakline() { todo(); }
void MainWindow::polygon() { todo(); }
void MainWindow::circle() { todo(); }

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
    //child->resize(size);
    QMdiSubWindow *sw = mdiArea->addSubWindow(child);
    sw->resize(size);
    return child;
}

void MainWindow::createActions()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QToolBar *fileToolBar = addToolBar(tr("File"));
    QToolBar *opToolBar = addToolBar(tr("Operate"));
    QToolBar *gsToolBar = addToolBar(tr("GreyScale"));

    const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(":/images/new.png"));
    newAct = new QAction(newIcon, tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
    fileMenu->addAction(newAct);
    fileToolBar->addAction(newAct);

    const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/images/open.png"));
    QAction *openAct = new QAction(openIcon, tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open); //TODO
    fileMenu->addAction(openAct);
    fileToolBar->addAction(openAct);

    const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));
    saveAct = new QAction(saveIcon, tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::save); //TODO
    fileToolBar->addAction(saveAct);

    fileToolBar->addSeparator();

    const QIcon zoominIcon = QIcon::fromTheme("document-save", QIcon(":/images/zoomin.png"));
    zoominAct = new QAction(zoominIcon, tr("&Zoomin"), this);
    zoominAct->setShortcuts(QKeySequence::Save);
    zoominAct->setStatusTip(tr("Save the document to disk"));
    connect(zoominAct, &QAction::triggered, this, &MainWindow::zoomin); //TODO
    fileToolBar->addAction(zoominAct);

    const QIcon zoomoutIcon = QIcon::fromTheme("document-save", QIcon(":/images/zoomout.png"));
    zoomoutAct = new QAction(zoomoutIcon, tr("&Zoomout"), this);
    zoomoutAct->setShortcuts(QKeySequence::Save);
    zoomoutAct->setStatusTip(tr("Save the document to disk"));
    connect(zoomoutAct, &QAction::triggered, this, &MainWindow::todo); //TODO
    fileToolBar->addAction(zoomoutAct);

    const QIcon zoom2Icon = QIcon::fromTheme("document-save", QIcon(":/images/zoom2.png"));
    zoom2Act = new QAction(zoom2Icon, tr("&Zoom2"), this);
    zoom2Act->setShortcuts(QKeySequence::Save);
    zoom2Act->setStatusTip(tr("Save the document to disk"));
    connect(zoom2Act, &QAction::triggered, this, &MainWindow::todo); //TODO
    fileToolBar->addAction(zoom2Act);

    const QIcon zoom4Icon = QIcon::fromTheme("document-save", QIcon(":/images/zoom4.png"));
    zoom4Act = new QAction(zoom4Icon, tr("&Zoom4"), this);
    zoom4Act->setShortcuts(QKeySequence::Save);
    zoom4Act->setStatusTip(tr("Save the document to disk"));
    connect(zoom4Act, &QAction::triggered, this, &MainWindow::todo); //TODO
    fileToolBar->addAction(zoom4Act);

    const QIcon pasteIcon = QIcon::fromTheme("document-paste", QIcon(":/images/paste.png"));
    pasteAct = new QAction(pasteIcon, tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste content to document"));
    connect(pasteAct, &QAction::triggered, this, &MainWindow::todo); //TODO
    fileToolBar->addAction(pasteAct);

    //optoolbar
    const QIcon cursorIcon = QIcon::fromTheme("document-paste", QIcon(":/images/cursor.png"));
    cursorAct = new QAction(cursorIcon, tr("&Cursor"), this);
    cursorAct->setShortcuts(QKeySequence::Paste);
    cursorAct->setStatusTip(tr("Paste content to document"));
    connect(cursorAct, &QAction::triggered, this, &MainWindow::cursor); //TODO
    opToolBar->addAction(cursorAct);

    const QIcon lineIcon = QIcon::fromTheme("document-paste", QIcon(":/images/line.png"));
    lineAct = new QAction(lineIcon, tr("&Line"), this);
    lineAct->setShortcuts(QKeySequence::Paste);
    lineAct->setStatusTip(tr("Paste content to document"));
    connect(lineAct, &QAction::triggered, this, &MainWindow::line); //TODO
    opToolBar->addAction(lineAct);

    const QIcon arrowIcon = QIcon::fromTheme("document-paste", QIcon(":/images/arrow.png"));
    arrowAct = new QAction(arrowIcon, tr("&Arrow"), this);
    arrowAct->setShortcuts(QKeySequence::Paste);
    arrowAct->setStatusTip(tr("Paste content to document"));
    connect(arrowAct, &QAction::triggered, this, &MainWindow::todo); //TODO
    opToolBar->addAction(arrowAct);

    opToolBar->addSeparator();

    const QIcon freelineIcon = QIcon::fromTheme("document-paste", QIcon(":/images/freeline.png"));
    freelineAct = new QAction(freelineIcon, tr("&Freeline"), this);
    freelineAct->setShortcuts(QKeySequence::Paste);
    freelineAct->setStatusTip(tr("Paste content to document"));
    connect(freelineAct, &QAction::triggered, this, &MainWindow::todo); //TODO
    opToolBar->addAction(freelineAct);

    const QIcon freepolygonIcon = QIcon::fromTheme("document-paste", QIcon(":/images/freepolygon.png"));
    freepolygonAct = new QAction(freepolygonIcon, tr("&FreePolygon"), this);
    freepolygonAct->setShortcuts(QKeySequence::Paste);
    freepolygonAct->setStatusTip(tr("Paste content to document"));
    connect(freepolygonAct, &QAction::triggered, this, &MainWindow::todo); //TODO
    opToolBar->addAction(freepolygonAct);

    const QIcon breaklineIcon = QIcon::fromTheme("document-paste", QIcon(":/images/breakline.png"));
    breaklineAct = new QAction(breaklineIcon, tr("&Breakline"), this);
    breaklineAct->setShortcuts(QKeySequence::Paste);
    breaklineAct->setStatusTip(tr("Paste content to document"));
    connect(breaklineAct, &QAction::triggered, this, &MainWindow::todo); //TODO
    opToolBar->addAction(breaklineAct);

    const QIcon polygonIcon = QIcon::fromTheme("document-paste", QIcon(":/images/polygon.png"));
    polygonAct = new QAction(polygonIcon, tr("&Polygon"), this);
    polygonAct->setShortcuts(QKeySequence::Paste);
    polygonAct->setStatusTip(tr("Paste content to document"));
    connect(polygonAct, &QAction::triggered, this, &MainWindow::todo); //TODO
    opToolBar->addAction(polygonAct);

    const QIcon circleIcon = QIcon::fromTheme("document-paste", QIcon(":/images/circle.png"));
    circleAct = new QAction(circleIcon, tr("&Circle"), this);
    circleAct->setShortcuts(QKeySequence::Paste);
    circleAct->setStatusTip(tr("Paste content to document"));
    connect(circleAct, &QAction::triggered, this, &MainWindow::todo); //TODO
    opToolBar->addAction(circleAct);
    opToolBar->resize(300, opToolBar->height());


    //greyscale slider
    ctkRangeSlider *rs= new ctkRangeSlider(Qt::Horizontal);
    rs->setValues(0, 65535);
    rs->setMaximumWidth(250);
    gsToolBar->addWidget(rs);
    //gsToolBar->window()->resize(128, gsToolBar->height());
    //gsToolBar->setGeometry(gsToolBar->geometry().x(), gsToolBar->geometry().y(), 128, gsToolBar->height());
    //gsToolBar->resize(128, gsToolBar->height());
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
