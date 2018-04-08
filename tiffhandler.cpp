#include <qvariant.h>
#include <qdebug.h>
#include <qimage.h>
#include <qglobal.h>

#include "tiffhandler.h"
extern "C" {
#include "tiffio.h"
}

tsize_t vitiffReadProc(thandle_t fd, tdata_t buf, tsize_t size)
{
    QIODevice *device = static_cast<QIODevice *>(fd);
    return device->isReadable() ? device->read(static_cast<char *>(buf), size) : -1;
}

tsize_t vitiffWriteProc(thandle_t fd, tdata_t buf, tsize_t size)
{
    return static_cast<QIODevice *>(fd)->write(static_cast<char *>(buf), size);
}

toff_t vitiffSeekProc(thandle_t fd, toff_t off, int whence)
{
    QIODevice *device = static_cast<QIODevice *>(fd);
    switch (whence) {
    case SEEK_SET:
        device->seek(off);
        break;
    case SEEK_CUR:
        device->seek(device->pos() + off);
        break;
    case SEEK_END:
        device->seek(device->size() + off);
        break;
    }

    return device->pos();
}

int vitiffCloseProc(thandle_t /*fd*/)
{
    return 0;
}

toff_t vitiffSizeProc(thandle_t fd)
{
    return static_cast<QIODevice *>(fd)->size();
}

int vitiffMapProc(thandle_t /*fd*/, tdata_t* /*pbase*/, toff_t* /*psize*/)
{
    return 0;
}

void vitiffUnmapProc(thandle_t /*fd*/, tdata_t /*base*/, toff_t /*size*/)
{
}

static QImageIOHandler::Transformations exif2Qt(int exifOrientation)
{
    switch (exifOrientation) {
    case 1: // normal
        return QImageIOHandler::TransformationNone;
    case 2: // mirror horizontal
        return QImageIOHandler::TransformationMirror;
    case 3: // rotate 180
        return QImageIOHandler::TransformationRotate180;
    case 4: // mirror vertical
        return QImageIOHandler::TransformationFlip;
    case 5: // mirror horizontal and rotate 270 CW
        return QImageIOHandler::TransformationFlipAndRotate90;
    case 6: // rotate 90 CW
        return QImageIOHandler::TransformationRotate90;
    case 7: // mirror horizontal and rotate 90 CW
        return QImageIOHandler::TransformationMirrorAndRotate90;
    case 8: // rotate 270 CW
        return QImageIOHandler::TransformationRotate270;
    }
    qWarning("Invalid EXIF orientation");
    return QImageIOHandler::TransformationNone;
}

static int qt2Exif(QImageIOHandler::Transformations transformation)
{
    switch (transformation) {
    case QImageIOHandler::TransformationNone:
        return 1;
    case QImageIOHandler::TransformationMirror:
        return 2;
    case QImageIOHandler::TransformationRotate180:
        return 3;
    case QImageIOHandler::TransformationFlip:
        return 4;
    case QImageIOHandler::TransformationFlipAndRotate90:
        return 5;
    case QImageIOHandler::TransformationRotate90:
        return 6;
    case QImageIOHandler::TransformationMirrorAndRotate90:
        return 7;
    case QImageIOHandler::TransformationRotate270:
        return 8;
    }
    qWarning("Invalid Qt image transformation");
    return 1;
}

ViTiffHandlerPrivate::ViTiffHandlerPrivate()
    : tiff(0)
    , compression(ViTiffHandler::NoCompression)
    , transformation(QImageIOHandler::TransformationNone)
    , format(QImage::Format_Invalid)
    , photometric(false)
    , grayscale(false)
    , headersRead(false)
    , currentDirectory(0)
    , directoryCount(0)
{
}

ViTiffHandlerPrivate::~ViTiffHandlerPrivate()
{
    close();
}

void ViTiffHandlerPrivate::close()
{
    if (tiff)
        TIFFClose(tiff);
    tiff = 0;
}

bool ViTiffHandlerPrivate::canRead(QIODevice *device)
{
    if (!device) {
        qWarning("ViTiffHandler::canRead() called with no device");
        return false;
    }

    // current implementation uses TIFFClientOpen which needs to be
    // able to seek, so sequential devices are not supported
    QByteArray header = device->peek(4);
    return header == QByteArray::fromRawData("\x49\x49\x2A\x00", 4)
           || header == QByteArray::fromRawData("\x4D\x4D\x00\x2A", 4);
}

bool ViTiffHandlerPrivate::openForRead(QIODevice *device)
{
    if (tiff)
        return true;

    if (!canRead(device))
        return false;

    tiff = TIFFClientOpen("foo",
                          "r",
                          device,
                          vitiffReadProc,
                          vitiffWriteProc,
                          vitiffSeekProc,
                          vitiffCloseProc,
                          vitiffSizeProc,
                          vitiffMapProc,
                          vitiffUnmapProc);

    if (!tiff) {
        return false;
    }
    return true;
}

bool ViTiffHandlerPrivate::readHeaders(QIODevice *device)
{
    if (headersRead)
        return true;

   if (!openForRead(device))
        return false;

    TIFFSetDirectory(tiff, currentDirectory);

    uint32 width;
    uint32 height;
    if (!TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &width)
        || !TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &height)
        || !TIFFGetField(tiff, TIFFTAG_PHOTOMETRIC, &photometric)) {
        close();
        return false;
    }
    size = QSize(width, height);

    uint16 orientationTag;
    if (TIFFGetField(tiff, TIFFTAG_ORIENTATION, &orientationTag))
        transformation = exif2Qt(orientationTag);

    // BitsPerSample defaults to 1 according to the TIFF spec.
    uint16 bitPerSample;
    if (!TIFFGetField(tiff, TIFFTAG_BITSPERSAMPLE, &bitPerSample))
        bitPerSample = 1;
    uint16 samplesPerPixel; // they may be e.g. grayscale with 2 samples per pixel
    if (!TIFFGetField(tiff, TIFFTAG_SAMPLESPERPIXEL, &samplesPerPixel))
        samplesPerPixel = 1;

    grayscale = photometric == PHOTOMETRIC_MINISBLACK || photometric == PHOTOMETRIC_MINISWHITE;

    if (grayscale && bitPerSample == 1 && samplesPerPixel == 1)
        format = QImage::Format_Mono;
    else if (photometric == PHOTOMETRIC_MINISBLACK && bitPerSample == 8 && samplesPerPixel == 1)
        format = QImage::Format_Grayscale8;
    else if ((grayscale || photometric == PHOTOMETRIC_PALETTE) && bitPerSample == 8 && samplesPerPixel == 1)
        format = QImage::Format_Indexed8;
    else if (samplesPerPixel < 4)
        format = QImage::Format_RGB32;
    else {
        uint16 count;
        uint16 *extrasamples;
        // If there is any definition of the alpha-channel, libtiff will return premultiplied
        // data to us. If there is none, libtiff will not touch it and  we assume it to be
        // non-premultiplied, matching behavior of tested image editors, and how older Qt
        // versions used to save it.
        bool gotField = TIFFGetField(tiff, TIFFTAG_EXTRASAMPLES, &count, &extrasamples);
        if (!gotField || !count || extrasamples[0] == EXTRASAMPLE_UNSPECIFIED)
            format = QImage::Format_ARGB32;
        else
            format = QImage::Format_ARGB32_Premultiplied;
    }

    headersRead = true;
    return true;
}

ViTiffHandler::ViTiffHandler()
    : QImageIOHandler()
    , d(new ViTiffHandlerPrivate)
{
}

bool ViTiffHandler::canRead() const
{
    if (d->tiff)
        return true;
    if (ViTiffHandlerPrivate::canRead(device())) {
        setFormat("tiff");
        return true;
    }
    return false;
}

bool ViTiffHandler::canRead(QIODevice *device)
{
    return ViTiffHandlerPrivate::canRead(device);
}

bool ViTiffHandler::read(QImage *image)
{
    // Open file and read headers if it hasn't already been done.
    if (!d->readHeaders(device()))
        return false;

    QImage::Format format = d->format;
    if (format == QImage::Format_RGB32 &&
            (image->format() == QImage::Format_ARGB32 ||
             image->format() == QImage::Format_ARGB32_Premultiplied))
        format = image->format();

    if (image->size() != d->size || image->format() != format)
        *image = QImage(d->size, format);

    if (image->isNull()) {
        d->close();
        return false;
    }

    TIFF *const tiff = d->tiff;
    const uint32 width = d->size.width();
    const uint32 height = d->size.height();

    if (format == QImage::Format_Mono || format == QImage::Format_Indexed8 || format == QImage::Format_Grayscale8) {
        if (format == QImage::Format_Mono) {
            QVector<QRgb> colortable(2);
            if (d->photometric == PHOTOMETRIC_MINISBLACK) {
                colortable[0] = 0xff000000;
                colortable[1] = 0xffffffff;
            } else {
                colortable[0] = 0xffffffff;
                colortable[1] = 0xff000000;
            }
            image->setColorTable(colortable);
        } else if (format == QImage::Format_Indexed8) {
            const uint16 tableSize = 256;
            QVector<QRgb> qtColorTable(tableSize);
            if (d->grayscale) {
                for (int i = 0; i<tableSize; ++i) {
                    const int c = (d->photometric == PHOTOMETRIC_MINISBLACK) ? i : (255 - i);
                    qtColorTable[i] = qRgb(c, c, c);
                }
            } else {
                // create the color table
                uint16 *redTable = 0;
                uint16 *greenTable = 0;
                uint16 *blueTable = 0;
                if (!TIFFGetField(tiff, TIFFTAG_COLORMAP, &redTable, &greenTable, &blueTable)) {
                    d->close();
                    return false;
                }
                if (!redTable || !greenTable || !blueTable) {
                    d->close();
                    return false;
                }

                for (int i = 0; i<tableSize ;++i) {
                    const int red = redTable[i] / 257;
                    const int green = greenTable[i] / 257;
                    const int blue = blueTable[i] / 257;
                    qtColorTable[i] = qRgb(red, green, blue);
                }
            }
            image->setColorTable(qtColorTable);
            // free redTable, greenTable and greenTable done by libtiff
        }

        if (TIFFIsTiled(tiff)) {
            quint32 tileWidth, tileLength;
            TIFFGetField(tiff, TIFFTAG_TILEWIDTH, &tileWidth);
            TIFFGetField(tiff, TIFFTAG_TILELENGTH, &tileLength);
            uchar *buf = (uchar *)_TIFFmalloc(TIFFTileSize(tiff));
            if (!tileWidth || !tileLength || !buf) {
                _TIFFfree(buf);
                d->close();
                return false;
            }
            quint32 byteWidth = (format == QImage::Format_Mono) ? (width + 7)/8 : width;
            quint32 byteTileWidth = (format == QImage::Format_Mono) ? tileWidth/8 : tileWidth;
            for (quint32 y = 0; y < height; y += tileLength) {
                for (quint32 x = 0; x < width; x += tileWidth) {
                    if (TIFFReadTile(tiff, buf, x, y, 0, 0) < 0) {
                        _TIFFfree(buf);
                        d->close();
                        return false;
                    }
                    quint32 linesToCopy = qMin(tileLength, height - y);
                    quint32 byteOffset = (format == QImage::Format_Mono) ? x/8 : x;
                    quint32 widthToCopy = qMin(byteTileWidth, byteWidth - byteOffset);
                    for (quint32 i = 0; i < linesToCopy; i++) {
                        ::memcpy(image->scanLine(y + i) + byteOffset, buf + (i * byteTileWidth), widthToCopy);
                    }
                }
            }
            _TIFFfree(buf);
        } else {
            for (uint32 y=0; y<height; ++y) {
                if (TIFFReadScanline(tiff, image->scanLine(y), y, 0) < 0) {
                    d->close();
                    return false;
                }
            }
        }
    } else {
        const int stopOnError = 1;
        if (TIFFReadRGBAImageOriented(tiff, width, height, reinterpret_cast<uint32 *>(image->bits()), qt2Exif(d->transformation), stopOnError)) {
            for (uint32 y=0; y<height; ++y)
                convert32BitOrder(image->scanLine(y), width);
        } else {
            d->close();
            return false;
        }
    }


    float resX = 0;
    float resY = 0;
    uint16 resUnit;
    if (!TIFFGetField(tiff, TIFFTAG_RESOLUTIONUNIT, &resUnit))
        resUnit = RESUNIT_INCH;

    if (TIFFGetField(tiff, TIFFTAG_XRESOLUTION, &resX)
        && TIFFGetField(tiff, TIFFTAG_YRESOLUTION, &resY)) {

        switch(resUnit) {
        case RESUNIT_CENTIMETER:
            image->setDotsPerMeterX(qRound(resX * 100));
            image->setDotsPerMeterY(qRound(resY * 100));
            break;
        case RESUNIT_INCH:
            image->setDotsPerMeterX(qRound(resX * (100 / 2.54)));
            image->setDotsPerMeterY(qRound(resY * (100 / 2.54)));
            break;
        default:
            // do nothing as defaults have already
            // been set within the QImage class
            break;
        }
    }

    return true;
}

static bool checkGrayscale(const QVector<QRgb> &colorTable)
{
    if (colorTable.size() != 256)
        return false;

    const bool increasing = (colorTable.at(0) == 0xff000000);
    for (int i = 0; i < 256; ++i) {
        if ((increasing && colorTable.at(i) != qRgb(i, i, i))
            || (!increasing && colorTable.at(i) != qRgb(255 - i, 255 - i, 255 - i)))
            return false;
    }
    return true;
}

static QVector<QRgb> effectiveColorTable(const QImage &image)
{
    QVector<QRgb> colors;
    switch (image.format()) {
    case QImage::Format_Indexed8:
        colors = image.colorTable();
        break;
    case QImage::Format_Alpha8:
        colors.resize(256);
        for (int i = 0; i < 256; ++i)
            colors[i] = qRgba(0, 0, 0, i);
        break;
    case QImage::Format_Grayscale8:
        colors.resize(256);
        for (int i = 0; i < 256; ++i)
            colors[i] = qRgb(i, i, i);
        break;
    default:
        Q_UNREACHABLE();
    }
    return colors;
}

bool ViTiffHandler::write(const QImage &image)
{
    if (!device()->isWritable())
        return false;

    TIFF *const tiff = TIFFClientOpen("foo",
                                      "wB",
                                      device(),
                                      vitiffReadProc,
                                      vitiffWriteProc,
                                      vitiffSeekProc,
                                      vitiffCloseProc,
                                      vitiffSizeProc,
                                      vitiffMapProc,
                                      vitiffUnmapProc);
    if (!tiff)
        return false;

    const int width = image.width();
    const int height = image.height();
    const int compression = d->compression;

    if (!TIFFSetField(tiff, TIFFTAG_IMAGEWIDTH, width)
        || !TIFFSetField(tiff, TIFFTAG_IMAGELENGTH, height)
        || !TIFFSetField(tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG)) {
        TIFFClose(tiff);
        return false;
    }

    // set the resolution
    bool  resolutionSet = false;
    const int dotPerMeterX = image.dotsPerMeterX();
    const int dotPerMeterY = image.dotsPerMeterY();
    if ((dotPerMeterX % 100) == 0
        && (dotPerMeterY % 100) == 0) {
        resolutionSet = TIFFSetField(tiff, TIFFTAG_RESOLUTIONUNIT, RESUNIT_CENTIMETER)
                        && TIFFSetField(tiff, TIFFTAG_XRESOLUTION, dotPerMeterX/100.0)
                        && TIFFSetField(tiff, TIFFTAG_YRESOLUTION, dotPerMeterY/100.0);
    } else {
        resolutionSet = TIFFSetField(tiff, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH)
                        && TIFFSetField(tiff, TIFFTAG_XRESOLUTION, static_cast<float>(image.logicalDpiX()))
                        && TIFFSetField(tiff, TIFFTAG_YRESOLUTION, static_cast<float>(image.logicalDpiY()));
    }
    if (!resolutionSet) {
        TIFFClose(tiff);
        return false;
    }
    // set the orienataion
    bool orientationSet = false;
    orientationSet = TIFFSetField(tiff, TIFFTAG_ORIENTATION, qt2Exif(d->transformation));
    if (!orientationSet) {
        TIFFClose(tiff);
        return false;
    }

    // configure image depth
    const QImage::Format format = image.format();
    if (format == QImage::Format_Mono || format == QImage::Format_MonoLSB) {
        uint16 photometric = PHOTOMETRIC_MINISBLACK;
        if (image.colorTable().at(0) == 0xffffffff)
            photometric = PHOTOMETRIC_MINISWHITE;
        if (!TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC, photometric)
            || !TIFFSetField(tiff, TIFFTAG_COMPRESSION, compression == NoCompression ? COMPRESSION_NONE : COMPRESSION_LZW)
            || !TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, 1)) {
            TIFFClose(tiff);
            return false;
        }

        // try to do the conversion in chunks no greater than 16 MB
        int chunks = (width * height / (1024 * 1024 * 16)) + 1;
        int chunkHeight = qMax(height / chunks, 1);

        int y = 0;
        while (y < height) {
            QImage chunk = image.copy(0, y, width, qMin(chunkHeight, height - y)).convertToFormat(QImage::Format_Mono);

            int chunkStart = y;
            int chunkEnd = y + chunk.height();
            while (y < chunkEnd) {
                if (TIFFWriteScanline(tiff, reinterpret_cast<uint32 *>(chunk.scanLine(y - chunkStart)), y) != 1) {
                    TIFFClose(tiff);
                    return false;
                }
                ++y;
            }
        }
        TIFFClose(tiff);
    } else if (format == QImage::Format_Indexed8
               || format == QImage::Format_Grayscale8
               || format == QImage::Format_Alpha8) {
        QVector<QRgb> colorTable = effectiveColorTable(image);
        bool isGrayscale = checkGrayscale(colorTable);
        if (isGrayscale) {
            uint16 photometric = PHOTOMETRIC_MINISBLACK;
            if (colorTable.at(0) == 0xffffffff)
                photometric = PHOTOMETRIC_MINISWHITE;
            if (!TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC, photometric)
                    || !TIFFSetField(tiff, TIFFTAG_COMPRESSION, compression == NoCompression ? COMPRESSION_NONE : COMPRESSION_LZW)
                    || !TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, 8)) {
                TIFFClose(tiff);
                return false;
            }
        } else {
            if (!TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_PALETTE)
                    || !TIFFSetField(tiff, TIFFTAG_COMPRESSION, compression == NoCompression ? COMPRESSION_NONE : COMPRESSION_LZW)
                    || !TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, 8)) {
                TIFFClose(tiff);
                return false;
            }
            //// write the color table
            // allocate the color tables
            const int tableSize = colorTable.size();
            Q_ASSERT(tableSize <= 256);
            QVarLengthArray<uint16> redTable(tableSize);
            QVarLengthArray<uint16> greenTable(tableSize);
            QVarLengthArray<uint16> blueTable(tableSize);

            // set the color table
            for (int i = 0; i<tableSize; ++i) {
                const QRgb color = colorTable.at(i);
                redTable[i] = qRed(color) * 257;
                greenTable[i] = qGreen(color) * 257;
                blueTable[i] = qBlue(color) * 257;
            }

            const bool setColorTableSuccess = TIFFSetField(tiff, TIFFTAG_COLORMAP, redTable.data(), greenTable.data(), blueTable.data());

            if (!setColorTableSuccess) {
                TIFFClose(tiff);
                return false;
            }
        }

        //// write the data
        // try to do the conversion in chunks no greater than 16 MB
        int chunks = (width * height/ (1024 * 1024 * 16)) + 1;
        int chunkHeight = qMax(height / chunks, 1);

        int y = 0;
        while (y < height) {
            QImage chunk = image.copy(0, y, width, qMin(chunkHeight, height - y));

            int chunkStart = y;
            int chunkEnd = y + chunk.height();
            while (y < chunkEnd) {
                if (TIFFWriteScanline(tiff, reinterpret_cast<uint32 *>(chunk.scanLine(y - chunkStart)), y) != 1) {
                    TIFFClose(tiff);
                    return false;
                }
                ++y;
            }
        }
        TIFFClose(tiff);
    } else if (!image.hasAlphaChannel()) {
        if (!TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB)
            || !TIFFSetField(tiff, TIFFTAG_COMPRESSION, compression == NoCompression ? COMPRESSION_NONE : COMPRESSION_LZW)
            || !TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, 3)
            || !TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, 8)) {
            TIFFClose(tiff);
            return false;
        }
        // try to do the RGB888 conversion in chunks no greater than 16 MB
        const int chunks = (width * height * 3 / (1024 * 1024 * 16)) + 1;
        const int chunkHeight = qMax(height / chunks, 1);

        int y = 0;
        while (y < height) {
            const QImage chunk = image.copy(0, y, width, qMin(chunkHeight, height - y)).convertToFormat(QImage::Format_RGB888);

            int chunkStart = y;
            int chunkEnd = y + chunk.height();
            while (y < chunkEnd) {
                if (TIFFWriteScanline(tiff, (void*)chunk.scanLine(y - chunkStart), y) != 1) {
                    TIFFClose(tiff);
                    return false;
                }
                ++y;
            }
        }
        TIFFClose(tiff);
    } else {
        const bool premultiplied = image.format() != QImage::Format_ARGB32
                                && image.format() != QImage::Format_RGBA8888;
        const uint16 extrasamples = premultiplied ? EXTRASAMPLE_ASSOCALPHA : EXTRASAMPLE_UNASSALPHA;
        if (!TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB)
            || !TIFFSetField(tiff, TIFFTAG_COMPRESSION, compression == NoCompression ? COMPRESSION_NONE : COMPRESSION_LZW)
            || !TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, 4)
            || !TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, 8)
            || !TIFFSetField(tiff, TIFFTAG_EXTRASAMPLES, 1, &extrasamples)) {
            TIFFClose(tiff);
            return false;
        }
        // try to do the RGBA8888 conversion in chunks no greater than 16 MB
        const int chunks = (width * height * 4 / (1024 * 1024 * 16)) + 1;
        const int chunkHeight = qMax(height / chunks, 1);

        const QImage::Format format = premultiplied ? QImage::Format_RGBA8888_Premultiplied
                                                    : QImage::Format_RGBA8888;
        int y = 0;
        while (y < height) {
            const QImage chunk = image.copy(0, y, width, qMin(chunkHeight, height - y)).convertToFormat(format);

            int chunkStart = y;
            int chunkEnd = y + chunk.height();
            while (y < chunkEnd) {
                if (TIFFWriteScanline(tiff, (void*)chunk.scanLine(y - chunkStart), y) != 1) {
                    TIFFClose(tiff);
                    return false;
                }
                ++y;
            }
        }
        TIFFClose(tiff);
    }

    return true;
}

QByteArray ViTiffHandler::name() const
{
    return "tiff";
}

QVariant ViTiffHandler::option(ImageOption option) const
{
    if (option == Size && canRead()) {
        if (d->readHeaders(device()))
            return d->size;
    } else if (option == CompressionRatio) {
        return d->compression;
    } else if (option == ImageFormat) {
        if (d->readHeaders(device()))
            return d->format;
    } else if (option == ImageTransformation) {
        if (d->readHeaders(device()))
            return int(d->transformation);
    }
    return QVariant();
}

void ViTiffHandler::setOption(ImageOption option, const QVariant &value)
{
    if (option == CompressionRatio && value.type() == QVariant::Int)
        d->compression = value.toInt();
    if (option == ImageTransformation) {
        int transformation = value.toInt();
        if (transformation > 0 && transformation < 8)
            d->transformation = QImageIOHandler::Transformations(transformation);
    }
}

bool ViTiffHandler::supportsOption(ImageOption option) const
{
    return option == CompressionRatio
            || option == Size
            || option == ImageFormat
            || option == ImageTransformation
            || option == TransformedByDefault;
}

bool ViTiffHandler::jumpToNextImage()
{
    if (!ensureHaveDirectoryCount())
        return false;
    if (d->currentDirectory >= d->directoryCount - 1)
        return false;

    d->headersRead = false;
    ++d->currentDirectory;
    return true;
}

bool ViTiffHandler::jumpToImage(int imageNumber)
{
    if (!ensureHaveDirectoryCount())
        return false;
    if (imageNumber < 0 || imageNumber >= d->directoryCount)
        return false;

    if (d->currentDirectory != imageNumber) {
        d->headersRead = false;
        d->currentDirectory = imageNumber;
    }
    return true;
}

int ViTiffHandler::imageCount() const
{
    if (!ensureHaveDirectoryCount())
        return 1;

    return d->directoryCount;
}

int ViTiffHandler::currentImageNumber() const
{
    return d->currentDirectory;
}

void ViTiffHandler::convert32BitOrder(void *buffer, int width)
{
    uint32 *target = reinterpret_cast<uint32 *>(buffer);
    for (int32 x=0; x<width; ++x) {
        uint32 p = target[x];
        // convert between ARGB and ABGR
        target[x] = (p & 0xff000000)
                    | ((p & 0x00ff0000) >> 16)
                    | (p & 0x0000ff00)
                    | ((p & 0x000000ff) << 16);
    }
}

bool ViTiffHandler::ensureHaveDirectoryCount() const
{
    if (d->directoryCount > 0)
        return true;

    TIFF *tiff = TIFFClientOpen("foo",
                                "r",
                                device(),
                                vitiffReadProc,
                                vitiffWriteProc,
                                vitiffSeekProc,
                                vitiffCloseProc,
                                vitiffSizeProc,
                                vitiffMapProc,
                                vitiffUnmapProc);
    if (!tiff) {
        device()->reset();
        return false;
    }

    do {
        ++d->directoryCount;
    } while (TIFFReadDirectory(tiff));
    TIFFClose(tiff);
    device()->reset();
    return true;
}
