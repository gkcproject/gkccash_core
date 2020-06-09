/****************************************************************************
** Meta object code from reading C++ file 'tokentransactiontablemodel.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../qt_native/tokentransactiontablemodel.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tokentransactiontablemodel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_TokenTransactionTableModel_t {
    QByteArrayData data[11];
    char stringdata0[154];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TokenTransactionTableModel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TokenTransactionTableModel_t qt_meta_stringdata_TokenTransactionTableModel = {
    {
QT_MOC_LITERAL(0, 0, 26), // "TokenTransactionTableModel"
QT_MOC_LITERAL(1, 27, 15), // "emitDataChanged"
QT_MOC_LITERAL(2, 43, 0), // ""
QT_MOC_LITERAL(3, 44, 5), // "index"
QT_MOC_LITERAL(4, 50, 17), // "updateTransaction"
QT_MOC_LITERAL(5, 68, 4), // "hash"
QT_MOC_LITERAL(6, 73, 6), // "status"
QT_MOC_LITERAL(7, 80, 15), // "showTransaction"
QT_MOC_LITERAL(8, 96, 19), // "updateConfirmations"
QT_MOC_LITERAL(9, 116, 31), // "setProcessingQueuedTransactions"
QT_MOC_LITERAL(10, 148, 5) // "value"

    },
    "TokenTransactionTableModel\0emitDataChanged\0"
    "\0index\0updateTransaction\0hash\0status\0"
    "showTransaction\0updateConfirmations\0"
    "setProcessingQueuedTransactions\0value"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TokenTransactionTableModel[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x0a /* Public */,
       4,    3,   37,    2, 0x0a /* Public */,
       8,    0,   44,    2, 0x0a /* Public */,
       9,    1,   45,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::Bool,    5,    6,    7,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   10,

       0        // eod
};

void TokenTransactionTableModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TokenTransactionTableModel *_t = static_cast<TokenTransactionTableModel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->emitDataChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->updateTransaction((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 2: _t->updateConfirmations(); break;
        case 3: _t->setProcessingQueuedTransactions((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject TokenTransactionTableModel::staticMetaObject = {
    { &QAbstractTableModel::staticMetaObject, qt_meta_stringdata_TokenTransactionTableModel.data,
      qt_meta_data_TokenTransactionTableModel,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *TokenTransactionTableModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TokenTransactionTableModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TokenTransactionTableModel.stringdata0))
        return static_cast<void*>(this);
    return QAbstractTableModel::qt_metacast(_clname);
}

int TokenTransactionTableModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractTableModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
