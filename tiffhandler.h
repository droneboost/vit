#ifndef VITIFFHANDLER_H
#define VITIFFHANDLER_H

#include <QScopedPointer>
#include <QImageIOHandler>
#include <qvariant.h>
#include <qdebug.h>
#include <qimage.h>
#include <qglobal.h>

extern "C" {
#include "tiffio.h"
}

class ViTiffHandlerPrivate
{
public:
    ViTiffHandlerPrivate();
    ~ViTiffHandlerPrivate();

    static bool canRead(QIODevice *device);
    bool openForRead(QIODevice *device);
    bool readHeaders(QIODevice *device);
    void close();

    TIFF *tiff;
    int compression;
    QImageIOHandler::Transformations transformation;
    QImage::Format format;
    QSize size;
    uint16 photometric;
    bool grayscale;
    bool headersRead;
    int currentDirectory;
    int directoryCount;
};

class ViTiffHandler : public QImageIOHandler
{
public:
    ViTiffHandler();

    bool canRead() const override;
    bool read(QImage *image) override;
    bool write(const QImage &image) override;

    QByteArray name() const override;

    static bool canRead(QIODevice *device);

    QVariant option(ImageOption option) const override;
    void setOption(ImageOption option, const QVariant &value) override;
    bool supportsOption(ImageOption option) const override;

    bool jumpToNextImage() Q_DECL_OVERRIDE;
    bool jumpToImage(int imageNumber) Q_DECL_OVERRIDE;
    int imageCount() const Q_DECL_OVERRIDE;
    int currentImageNumber() const Q_DECL_OVERRIDE;

    enum Compression {
        NoCompression = 0,
        LzwCompression = 1
    };
private:
    void convert32BitOrder(void *buffer, int width);
    const QScopedPointer<ViTiffHandlerPrivate> d;
    bool ensureHaveDirectoryCount() const;
};

#endif // VITIFFHANDLER_H
