#include <QtWidgets>
#include <QMdiArea>
#include <QImageReader>
#include <tiffio.h>

#include "mdigvchild.h"
#include "tiffhandler.h"

#define USE_OPENCV_CPLUSPLUS_IF

MdiGVChild::MdiGVChild():mp_Mat(NULL)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setDragMode(QGraphicsView::ScrollHandDrag);
    isUntitled = true;
    m_scene.setBackgroundBrush(Qt::gray);
    m_scene.addText("Empty");
    setScene(&m_scene);
    TIFFSetWarningHandler(NULL);
    TIFFSetWarningHandlerExt(NULL);
}

void MdiGVChild::newFile()
{
    static int sequenceNumber = 1;

    isUntitled = true;
    curFile = tr("imaget%1.tif").arg(sequenceNumber++);
    setWindowTitle(curFile + "[*]");
    // connect(document(), &QTextDocument::contentsChanged, this, &MdiChild::documentWasModified);
}


bool MdiGVChild::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadWrite)) {
        QMessageBox::warning(this, tr("MDI"),
                             tr("Cannot read image file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

#ifdef USE_OPENCV_CPLUSPLUS_IF
    // Seems imread() use EXIF information and will crash in ApplyExifOrientataion()
    // See https://github.com/opencv/opencv/issues/6673
    // set CV_LOAD_IMAGE_IGNORE_ORIENTATION to avoid ApplyExifOrientataion crash.
    //m_Mat = cv::imread (fileName.toStdString(), CV_LOAD_IMAGE_IGNORE_ORIENTATION|CV_LOAD_IMAGE_ANYDEPTH|CV_LOAD_IMAGE_GRAYSCALE);
    //if (m_Mat.empty()) {
    //    qDebug( "!!! Failed imread(): image not found");
    //    return false;
    //}
    //TODO: Aaron, add your image filter code herebelow for test!

    //cv::namedWindow( "Display OpenCV window", CV_WINDOW_AUTOSIZE );// Create a window for display.
    //cv::imshow( "Display OpenCV window", m_Mat );

    //Save file back
    //std::vector<int> params;
    //params.push_back(TIFFTAG_COMPRESSION);
    //params.push_back(1);
    //params.push_back(TIFFTAG_ROWSPERSTRIP );
    //params.push_back(512);
    //cv::imwrite("D:/misc/testimg/000.tif", m_Mat, params);
#else // USE_OPENCV_C_IF
    mp_Mat = cvLoadImageM(fileName.toStdString().c_str(), CV_LOAD_IMAGE_ANYDEPTH|CV_LOAD_IMAGE_GRAYSCALE );
    if (!mp_Mat) {
        qDebug( "!!! Failed imread(): image not found");
        return false;
    }
    cv::Mat img_mat = cv::cvarrToMat(mp_Mat);
    //TODO: Aaron, add your image filter code herebelow for test!

	//Show Opencv window with read tiff image
	cvNamedWindow( "Display OpenCV window", CV_WINDOW_AUTOSIZE );
    cvShowImage( "Display OpenCV window", mp_Mat );
    cvSaveImage("D:/misc/testimg/000.tif", mp_Mat);
    // Caution! didn't destory window and release mp_Mat, will cause memery leak in real use.
#endif

   // Also open an QGraphicView to show img read by raw Tiff IF wrap in vitiffhandler class.
    m_rawimg = readTiff(fileName);
    m_pixmap = QPixmap::fromImage(m_rawimg);
    m_scene.addPixmap(m_pixmap);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::restoreOverrideCursor();
    setCurrentFile(fileName);

    // connect(document(), &QTextDocument::contentsChanged, this, &MdiChild::documentWasModified);
    return true;
}

QImage MdiGVChild::readTiff(const QString &path)
{
  QImage img;
  QFile *file = new QFile(path);
  file->open(QIODevice::ReadOnly);
  ViTiffHandler handler;
  handler.setDevice(file);

  if(ViTiffHandler::canRead(file)) {
    handler.read(&img);
  }
  return img;

  TIFF* tiff = TIFFOpen(path.toStdString().c_str(), "r");
  if (!tiff) {
    QString msg = "Failed to open TIFF: '" + path + "'";
    //throw new Exception(NULL, msg, this, __FUNCTION__, __LINE__);
    return  QImage();
  }

  // Temporary variables
  uint32 width, height;
  tsize_t scanlength;

  // Read dimensions of image
  if (TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &width) != 1) {
    QString msg = "Failed to read width of TIFF: '" + path + "'";
    //throw new Exception(NULL, msg, this, __FUNCTION__, __LINE__);
    return  QImage();
  }
  if (TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &height) != 1) {
    QString msg = "Failed to read height of TIFF: '" + path + "'";
    //throw new Exception(NULL, msg, this, __FUNCTION__, __LINE__);
    return  QImage();
  }

  // Number of bytes in a decoded scanline
  scanlength = TIFFScanlineSize(tiff);

  QImage image(width, height, QImage::Format_RGB16);

  // TEMPORARY: Save to PNG for preview
  image.save("D:\\misc\\testimg\\aaa.png", "PNG");

  if (image.isNull() || scanlength != image.bytesPerLine()) {
    TIFFClose(tiff);
    QString msg = "Failed to create QImage from TIFF: '" + path + "'";
    //throw new Exception(NULL, msg, this, __FUNCTION__, __LINE__);
    return  QImage();
  }

  // Read image data
  for (uint32 y = 0; y < height; y++) {
    TIFFReadScanline(tiff, image.scanLine(y), y);
  }
  TIFFClose(tiff);
  return image;
}

bool MdiGVChild::save()
{
    if (isUntitled) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool MdiGVChild::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                                                    curFile);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

bool MdiGVChild::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("MDI"),
                             tr("Cannot write image file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return false;
    }

    m_pixmap.save(fileName, "TIF");
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::restoreOverrideCursor();
    setCurrentFile(fileName);

    return true;
}

QString MdiGVChild::userFriendlyCurrentFile()
{
    return strippedName(curFile);
}

void MdiGVChild::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
        QMdiArea* MdiArea = qobject_cast<QMdiArea *>(this->parent()->parent()->parent());
        MdiArea->removeSubWindow(this);
    } else {
        event->ignore();
    }
}

void MdiGVChild::wheelEvent(QWheelEvent * event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        const QPointF p0scene = mapToScene(event->pos());

        qreal factor = qPow(1.2, event->delta() / 240.0); //TODO: adjust scale factor
        scale(factor, factor);

        const QPointF p1mouse = mapFromScene(p0scene);
        const QPointF move = p1mouse - event->pos(); // The move
        horizontalScrollBar()->setValue(move.x() + horizontalScrollBar()->value());
        verticalScrollBar()->setValue(move.y() + verticalScrollBar()->value());
    }
    else {
        QGraphicsView::wheelEvent(event);
    }

}

void MdiGVChild::documentWasModified()
{
    //setWindowModified(document()->isModified());
}

bool MdiGVChild::maybeSave()
{
 /*
    if (!document()->isModified())
        return true;
    const QMessageBox::StandardButton ret
            = QMessageBox::warning(this, tr("MDI"),
                                   tr("'%1' has been modified.\n"
                                      "Do you want to save your changes?")
                                   .arg(userFriendlyCurrentFile()),
                                   QMessageBox::Save | QMessageBox::Discard
                                   | QMessageBox::Cancel);
    switch (ret) {
    case QMessageBox::Save:
        return save();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }
    */
    return true;
}

void MdiGVChild::setCurrentFile(const QString &fileName)
{
    //curFile = "D:\\misc\\testimg\\1.tiff";
    curFile = QFileInfo(fileName).canonicalFilePath();
    isUntitled = false;
    //document()->setModified(false);
    setWindowModified(false);
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

QString MdiGVChild::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}
