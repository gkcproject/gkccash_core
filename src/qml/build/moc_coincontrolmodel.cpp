/****************************************************************************
** Meta object code from reading C++ file 'coincontrolmodel.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../qt_native/coincontrolmodel.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'coincontrolmodel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CoinControlModel_t {
    QByteArrayData data[12];
    char stringdata0[158];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CoinControlModel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CoinControlModel_t qt_meta_stringdata_CoinControlModel = {
    {
QT_MOC_LITERAL(0, 0, 16), // "CoinControlModel"
QT_MOC_LITERAL(1, 17, 20), // "updateLabelBlockSize"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 4), // "size"
QT_MOC_LITERAL(4, 44, 26), // "updateCoinControlLabelsSig"
QT_MOC_LITERAL(5, 71, 20), // "updateSmartFeeLabels"
QT_MOC_LITERAL(6, 92, 10), // "returnList"
QT_MOC_LITERAL(7, 103, 19), // "notifySendingResult"
QT_MOC_LITERAL(8, 123, 4), // "type"
QT_MOC_LITERAL(9, 128, 5), // "title"
QT_MOC_LITERAL(10, 134, 3), // "msg"
QT_MOC_LITERAL(11, 138, 19) // "updateSmartFeeLabel"

    },
    "CoinControlModel\0updateLabelBlockSize\0"
    "\0size\0updateCoinControlLabelsSig\0"
    "updateSmartFeeLabels\0returnList\0"
    "notifySendingResult\0type\0title\0msg\0"
    "updateSmartFeeLabel"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CoinControlModel[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,
       4,    0,   42,    2, 0x06 /* Public */,
       5,    1,   43,    2, 0x06 /* Public */,
       7,    3,   46,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      11,    0,   53,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QVariantList,    6,
    QMetaType::Void, QMetaType::Int, QMetaType::QString, QMetaType::QString,    8,    9,   10,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void CoinControlModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CoinControlModel *_t = static_cast<CoinControlModel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updateLabelBlockSize((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->updateCoinControlLabelsSig(); break;
        case 2: _t->updateSmartFeeLabels((*reinterpret_cast< QVariantList(*)>(_a[1]))); break;
        case 3: _t->notifySendingResult((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 4: _t->updateSmartFeeLabel(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (CoinControlModel::*_t)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CoinControlModel::updateLabelBlockSize)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (CoinControlModel::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CoinControlModel::updateCoinControlLabelsSig)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (CoinControlModel::*_t)(QVariantList );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CoinControlModel::updateSmartFeeLabels)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (CoinControlModel::*_t)(int , QString , QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CoinControlModel::notifySendingResult)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CoinControlModel::staticMetaObject = {
    { &QAbstractListModel::staticMetaObject, qt_meta_stringdata_CoinControlModel.data,
      qt_meta_data_CoinControlModel,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *CoinControlModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CoinControlModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CoinControlModel.stringdata0))
        return static_cast<void*>(this);
    return QAbstractListModel::qt_metacast(_clname);
}

int CoinControlModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractListModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void CoinControlModel::updateLabelBlockSize(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CoinControlModel::updateCoinControlLabelsSig()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void CoinControlModel::updateSmartFeeLabels(QVariantList _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void CoinControlModel::notifySendingResult(int _t1, QString _t2, QString _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
