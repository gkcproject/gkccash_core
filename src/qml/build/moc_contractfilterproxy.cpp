/****************************************************************************
** Meta object code from reading C++ file 'contractfilterproxy.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../qt_native/contractfilterproxy.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'contractfilterproxy.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ContractFilterProxy_t {
    QByteArrayData data[25];
    char stringdata0[246];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ContractFilterProxy_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ContractFilterProxy_t qt_meta_stringdata_ContractFilterProxy = {
    {
QT_MOC_LITERAL(0, 0, 19), // "ContractFilterProxy"
QT_MOC_LITERAL(1, 20, 7), // "message"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 5), // "title"
QT_MOC_LITERAL(4, 35, 5), // "style"
QT_MOC_LITERAL(5, 41, 18), // "addressListChanged"
QT_MOC_LITERAL(6, 60, 10), // "on_refresh"
QT_MOC_LITERAL(7, 71, 10), // "sortColumn"
QT_MOC_LITERAL(8, 82, 8), // "roleName"
QT_MOC_LITERAL(9, 91, 13), // "Qt::SortOrder"
QT_MOC_LITERAL(10, 105, 5), // "order"
QT_MOC_LITERAL(11, 111, 12), // "saveContract"
QT_MOC_LITERAL(12, 124, 4), // "mode"
QT_MOC_LITERAL(13, 129, 5), // "label"
QT_MOC_LITERAL(14, 135, 8), // "contract"
QT_MOC_LITERAL(15, 144, 6), // "abiStr"
QT_MOC_LITERAL(16, 151, 3), // "row"
QT_MOC_LITERAL(17, 155, 14), // "deleteContract"
QT_MOC_LITERAL(18, 170, 7), // "getData"
QT_MOC_LITERAL(19, 178, 9), // "sourceRow"
QT_MOC_LITERAL(20, 188, 13), // "exportClicked"
QT_MOC_LITERAL(21, 202, 9), // "updateABI"
QT_MOC_LITERAL(22, 212, 7), // "address"
QT_MOC_LITERAL(23, 220, 13), // "lookupAddress"
QT_MOC_LITERAL(24, 234, 11) // "addressList"

    },
    "ContractFilterProxy\0message\0\0title\0"
    "style\0addressListChanged\0on_refresh\0"
    "sortColumn\0roleName\0Qt::SortOrder\0"
    "order\0saveContract\0mode\0label\0contract\0"
    "abiStr\0row\0deleteContract\0getData\0"
    "sourceRow\0exportClicked\0updateABI\0"
    "address\0lookupAddress\0addressList"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ContractFilterProxy[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       1,  118, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   69,    2, 0x06 /* Public */,
       5,    0,   76,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    0,   77,    2, 0x08 /* Private */,

 // methods: name, argc, parameters, tag, flags
       7,    2,   78,    2, 0x02 /* Public */,
      11,    5,   83,    2, 0x02 /* Public */,
      11,    4,   94,    2, 0x22 /* Public | MethodCloned */,
      17,    1,  103,    2, 0x02 /* Public */,
      18,    2,  106,    2, 0x02 /* Public */,
      20,    0,  111,    2, 0x02 /* Public */,
      21,    1,  112,    2, 0x02 /* Public */,
      23,    1,  115,    2, 0x02 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::UInt,    3,    1,    4,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,

 // methods: parameters
    QMetaType::Void, QMetaType::QString, 0x80000000 | 9,    8,   10,
    QMetaType::QString, QMetaType::Int, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::Int,   12,   13,   14,   15,   16,
    QMetaType::QString, QMetaType::Int, QMetaType::QString, QMetaType::QString, QMetaType::QString,   12,   13,   14,   15,
    QMetaType::Void, QMetaType::Int,   16,
    QMetaType::QVariant, QMetaType::QString, QMetaType::Int,    8,   19,
    QMetaType::Void,
    QMetaType::QString, QMetaType::QString,   22,
    QMetaType::Int, QMetaType::QString,   22,

 // properties: name, type, flags
      24, QMetaType::QStringList, 0x00495003,

 // properties: notify_signal_id
       1,

       0        // eod
};

void ContractFilterProxy::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ContractFilterProxy *_t = static_cast<ContractFilterProxy *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->message((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< uint(*)>(_a[3]))); break;
        case 1: _t->addressListChanged(); break;
        case 2: _t->on_refresh(); break;
        case 3: _t->sortColumn((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< Qt::SortOrder(*)>(_a[2]))); break;
        case 4: { QString _r = _t->saveContract((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4])),(*reinterpret_cast< int(*)>(_a[5])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 5: { QString _r = _t->saveContract((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 6: _t->deleteContract((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: { QVariant _r = _t->getData((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QVariant*>(_a[0]) = std::move(_r); }  break;
        case 8: _t->exportClicked(); break;
        case 9: { QString _r = _t->updateABI((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 10: { int _r = _t->lookupAddress((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (ContractFilterProxy::*_t)(const QString & , const QString & , unsigned int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ContractFilterProxy::message)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (ContractFilterProxy::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ContractFilterProxy::addressListChanged)) {
                *result = 1;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        ContractFilterProxy *_t = static_cast<ContractFilterProxy *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QStringList*>(_v) = _t->m_stringList; break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        ContractFilterProxy *_t = static_cast<ContractFilterProxy *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0:
            if (_t->m_stringList != *reinterpret_cast< QStringList*>(_v)) {
                _t->m_stringList = *reinterpret_cast< QStringList*>(_v);
                Q_EMIT _t->addressListChanged();
            }
            break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject ContractFilterProxy::staticMetaObject = {
    { &QSortFilterProxyModel::staticMetaObject, qt_meta_stringdata_ContractFilterProxy.data,
      qt_meta_data_ContractFilterProxy,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ContractFilterProxy::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ContractFilterProxy::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ContractFilterProxy.stringdata0))
        return static_cast<void*>(this);
    return QSortFilterProxyModel::qt_metacast(_clname);
}

int ContractFilterProxy::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QSortFilterProxyModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void ContractFilterProxy::message(const QString & _t1, const QString & _t2, unsigned int _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ContractFilterProxy::addressListChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
