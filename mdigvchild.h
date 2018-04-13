#ifndef MDIGVCHILD_H
#define MDIGVCHILD_H

#include <QGraphicsScene>
#include <QGraphicsView>

#include <opencv/cv.hpp>
#include <opencv2/imgcodecs.hpp>

class MdiGVChild : public QGraphicsView
{
    Q_OBJECT

public:
    MdiGVChild();

    void newFile();
    bool loadFile(const QString &fileName);
    bool save();
    bool saveAs();
    bool saveFile(const QString &fileName);
    QImage readTiff(const QString &path);
    QString userFriendlyCurrentFile();
    QString currentFile() { return curFile; }
    void addRect(qreal x, qreal y, qreal w, qreal h);

protected:
    void closeEvent(QCloseEvent *event) override;
    void wheelEvent(QWheelEvent * event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private slots:
    void documentWasModified();

private:
    bool maybeSave();
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

    QString curFile;
    bool isUntitled;

private:
    QGraphicsScene m_scene;
    QPixmap m_pixmap;
    QImage m_rawimg;
    cv::Mat m_Mat;
    CvMat* mp_Mat;
};

#endif
