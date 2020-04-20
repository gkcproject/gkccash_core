#ifndef CURSORPOSPROVIDER_H
#define CURSORPOSPROVIDER_H

#include <QObject>
#include <QCursor>
class CursorPosProvider : public QObject
{
    Q_OBJECT
public:
    explicit CursorPosProvider(QObject *parent = nullptr) : QObject(parent)
    {
    }
    virtual ~CursorPosProvider() = default;



    Q_INVOKABLE QPointF cursorPos()
    {
        return QCursor::pos();
    }

private:

signals:

};


#endif // CURSORPOSPROVIDER_H
