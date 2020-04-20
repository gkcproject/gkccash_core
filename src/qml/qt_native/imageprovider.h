#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QQuickImageProvider>
#include <QImage>


#include "walletmodel.h"

class ImageProvider: public QQuickImageProvider
{
public:
    ImageProvider();
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);

    void setImage(QImage &img);
private:
    QImage m_img;
};

#endif // IMAGEPROVIDER_H



