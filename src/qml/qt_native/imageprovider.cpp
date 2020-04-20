#include "imageprovider.h"

ImageProvider::ImageProvider(): QQuickImageProvider(QQuickImageProvider::Image)
{
}

//See doc QQuickImageProvider  :
//    "Note: The destructor of your derived class need to be thread safe."




QImage ImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    return m_img;
}

void ImageProvider::setImage(QImage &img)
{
    m_img = img;
}
