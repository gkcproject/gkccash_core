/****************************************************************************
** Meta object code from reading C++ file 'bitcoincore.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../bitcoincore.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'bitcoincore.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_BitcoinCore_t {
    QByteArrayData data[11];
    char stringdata0[110];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_BitcoinCore_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_BitcoinCore_t qt_meta_stringdata_BitcoinCore = {
    {
QT_MOC_LITERAL(0, 0, 11), // "BitcoinCore"
QT_MOC_LITERAL(1, 12, 16), // "initializeResult"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 6), // "retval"
QT_MOC_LITERAL(4, 37, 14), // "shutdownResult"
QT_MOC_LITERAL(5, 52, 16), // "runawayException"
QT_MOC_LITERAL(6, 69, 7), // "message"
QT_MOC_LITERAL(7, 77, 10), // "initialize"
QT_MOC_LITERAL(8, 88, 8), // "shutdown"
QT_MOC_LITERAL(9, 97, 7), // "restart"
QT_MOC_LITERAL(10, 105, 4) // "args"

    },
    "BitcoinCore\0initializeResult\0\0retval\0"
    "shutdownResult\0runawayException\0message\0"
    "initialize\0shutdown\0restart\0args"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_BitcoinCore[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x06 /* Public */,
       4,    1,   47,    2, 0x06 /* Public */,
       5,    1,   50,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    0,   53,    2, 0x0a /* Public */,
       8,    0,   54,    2, 0x0a /* Public */,
       9,    1,   55,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::QString,    6,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QStringList,   10,

       0        // eod
};

void BitcoinCore::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        BitcoinCore *_t = static_cast<BitcoinCore *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->initializeResult((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->shutdownResult((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->runawayException((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->initialize(); break;
        case 4: _t->shutdown(); break;
        case 5: _t->restart((*reinterpret_cast< QStringList(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (BitcoinCore::*_t)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BitcoinCore::initializeResult)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (BitcoinCore::*_t)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BitcoinCore::shutdownResult)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (BitcoinCore::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BitcoinCore::runawayException)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject BitcoinCore::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_BitcoinCore.data,
      qt_meta_data_BitcoinCore,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *BitcoinCore::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BitcoinCore::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_BitcoinCore.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int BitcoinCore::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void BitcoinCore::initializeResult(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void BitcoinCore::shutdownResult(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void BitcoinCore::runawayException(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
