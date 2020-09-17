/****************************************************************************
** Meta object code from reading C++ file 'tokenitemmodel.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../qt_native/tokenitemmodel.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tokenitemmodel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_TokenItemModel_t {
    QByteArrayData data[23];
    char stringdata0[273];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TokenItemModel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TokenItemModel_t qt_meta_stringdata_TokenItemModel = {
    {
QT_MOC_LITERAL(0, 0, 14), // "TokenItemModel"
QT_MOC_LITERAL(1, 15, 9), // "execError"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 6), // "errMsg"
QT_MOC_LITERAL(4, 33, 24), // "checkTokenBalanceChanged"
QT_MOC_LITERAL(5, 58, 11), // "updateToken"
QT_MOC_LITERAL(6, 70, 4), // "hash"
QT_MOC_LITERAL(7, 75, 6), // "status"
QT_MOC_LITERAL(8, 82, 9), // "showToken"
QT_MOC_LITERAL(9, 92, 10), // "menuPicked"
QT_MOC_LITERAL(10, 103, 10), // "modelIndex"
QT_MOC_LITERAL(11, 114, 9), // "menuIndex"
QT_MOC_LITERAL(12, 124, 15), // "removeTokenItem"
QT_MOC_LITERAL(13, 140, 16), // "updateReceiveImg"
QT_MOC_LITERAL(14, 157, 9), // "sendToken"
QT_MOC_LITERAL(15, 167, 12), // "payToAddress"
QT_MOC_LITERAL(16, 180, 11), // "gasLimitStr"
QT_MOC_LITERAL(17, 192, 12), // "gasPriceUint"
QT_MOC_LITERAL(18, 205, 11), // "gasPriceStr"
QT_MOC_LITERAL(19, 217, 9), // "amountStr"
QT_MOC_LITERAL(20, 227, 11), // "description"
QT_MOC_LITERAL(21, 239, 14), // "clearSendToken"
QT_MOC_LITERAL(22, 254, 18) // "sendTokenConfirmed"

    },
    "TokenItemModel\0execError\0\0errMsg\0"
    "checkTokenBalanceChanged\0updateToken\0"
    "hash\0status\0showToken\0menuPicked\0"
    "modelIndex\0menuIndex\0removeTokenItem\0"
    "updateReceiveImg\0sendToken\0payToAddress\0"
    "gasLimitStr\0gasPriceUint\0gasPriceStr\0"
    "amountStr\0description\0clearSendToken\0"
    "sendTokenConfirmed"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TokenItemModel[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   59,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   62,    2, 0x0a /* Public */,
       5,    3,   63,    2, 0x08 /* Private */,

 // methods: name, argc, parameters, tag, flags
       9,    2,   70,    2, 0x02 /* Public */,
      12,    1,   75,    2, 0x02 /* Public */,
      13,    1,   78,    2, 0x02 /* Public */,
      14,    6,   81,    2, 0x02 /* Public */,
      21,    0,   94,    2, 0x02 /* Public */,
      22,    0,   95,    2, 0x02 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::Bool,    6,    7,    8,

 // methods: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   10,   11,
    QMetaType::Void, QMetaType::Int,   10,
    QMetaType::QVariantList, QMetaType::Int,   10,
    QMetaType::QVariantList, QMetaType::QString, QMetaType::QString, QMetaType::Int, QMetaType::QString, QMetaType::QString, QMetaType::QString,   15,   16,   17,   18,   19,   20,
    QMetaType::Void,
    QMetaType::Bool,

       0        // eod
};

void TokenItemModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TokenItemModel *_t = static_cast<TokenItemModel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->execError((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->checkTokenBalanceChanged(); break;
        case 2: _t->updateToken((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 3: _t->menuPicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: _t->removeTokenItem((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: { QVariantList _r = _t->updateReceiveImg((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 6: { QVariantList _r = _t->sendToken((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4])),(*reinterpret_cast< const QString(*)>(_a[5])),(*reinterpret_cast< const QString(*)>(_a[6])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 7: _t->clearSendToken(); break;
        case 8: { bool _r = _t->sendTokenConfirmed();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (TokenItemModel::*_t)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TokenItemModel::execError)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject TokenItemModel::staticMetaObject = {
    { &QAbstractItemModel::staticMetaObject, qt_meta_stringdata_TokenItemModel.data,
      qt_meta_data_TokenItemModel,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *TokenItemModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TokenItemModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TokenItemModel.stringdata0))
        return static_cast<void*>(this);
    return QAbstractItemModel::qt_metacast(_clname);
}

int TokenItemModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void TokenItemModel::execError(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
