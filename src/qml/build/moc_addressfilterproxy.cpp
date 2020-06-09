/****************************************************************************
** Meta object code from reading C++ file 'addressfilterproxy.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../qt_native/addressfilterproxy.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'addressfilterproxy.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_AddressFilterProxy_t {
    QByteArrayData data[21];
    char stringdata0[187];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_AddressFilterProxy_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_AddressFilterProxy_t qt_meta_stringdata_AddressFilterProxy = {
    {
QT_MOC_LITERAL(0, 0, 18), // "AddressFilterProxy"
QT_MOC_LITERAL(1, 19, 7), // "message"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 5), // "title"
QT_MOC_LITERAL(4, 34, 5), // "style"
QT_MOC_LITERAL(5, 40, 10), // "sortColumn"
QT_MOC_LITERAL(6, 51, 8), // "roleName"
QT_MOC_LITERAL(7, 60, 13), // "Qt::SortOrder"
QT_MOC_LITERAL(8, 74, 5), // "order"
QT_MOC_LITERAL(9, 80, 11), // "saveAddress"
QT_MOC_LITERAL(10, 92, 4), // "mode"
QT_MOC_LITERAL(11, 97, 5), // "label"
QT_MOC_LITERAL(12, 103, 8), // "address_"
QT_MOC_LITERAL(13, 112, 3), // "row"
QT_MOC_LITERAL(14, 116, 13), // "deleteAddress"
QT_MOC_LITERAL(15, 130, 4), // "rows"
QT_MOC_LITERAL(16, 135, 7), // "getData"
QT_MOC_LITERAL(17, 143, 9), // "sourceRow"
QT_MOC_LITERAL(18, 153, 11), // "updateLabel"
QT_MOC_LITERAL(19, 165, 7), // "address"
QT_MOC_LITERAL(20, 173, 13) // "exportClicked"

    },
    "AddressFilterProxy\0message\0\0title\0"
    "style\0sortColumn\0roleName\0Qt::SortOrder\0"
    "order\0saveAddress\0mode\0label\0address_\0"
    "row\0deleteAddress\0rows\0getData\0sourceRow\0"
    "updateLabel\0address\0exportClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AddressFilterProxy[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   54,    2, 0x06 /* Public */,

 // methods: name, argc, parameters, tag, flags
       5,    2,   61,    2, 0x02 /* Public */,
       9,    4,   66,    2, 0x02 /* Public */,
       9,    3,   75,    2, 0x22 /* Public | MethodCloned */,
      14,    1,   82,    2, 0x02 /* Public */,
      16,    2,   85,    2, 0x02 /* Public */,
      18,    1,   90,    2, 0x02 /* Public */,
      20,    0,   93,    2, 0x02 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::UInt,    3,    1,    4,

 // methods: parameters
    QMetaType::Void, QMetaType::QString, 0x80000000 | 7,    6,    8,
    QMetaType::QString, QMetaType::Int, QMetaType::QString, QMetaType::QString, QMetaType::Int,   10,   11,   12,   13,
    QMetaType::QString, QMetaType::Int, QMetaType::QString, QMetaType::QString,   10,   11,   12,
    QMetaType::Void, QMetaType::QVariantList,   15,
    QMetaType::QVariant, QMetaType::QString, QMetaType::Int,    6,   17,
    QMetaType::QString, QMetaType::QString,   19,
    QMetaType::Void,

       0        // eod
};

void AddressFilterProxy::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AddressFilterProxy *_t = static_cast<AddressFilterProxy *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->message((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< uint(*)>(_a[3]))); break;
        case 1: _t->sortColumn((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< Qt::SortOrder(*)>(_a[2]))); break;
        case 2: { QString _r = _t->saveAddress((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 3: { QString _r = _t->saveAddress((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 4: _t->deleteAddress((*reinterpret_cast< QVariantList(*)>(_a[1]))); break;
        case 5: { QVariant _r = _t->getData((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QVariant*>(_a[0]) = std::move(_r); }  break;
        case 6: { QString _r = _t->updateLabel((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 7: _t->exportClicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (AddressFilterProxy::*_t)(const QString & , const QString & , unsigned int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AddressFilterProxy::message)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject AddressFilterProxy::staticMetaObject = {
    { &QSortFilterProxyModel::staticMetaObject, qt_meta_stringdata_AddressFilterProxy.data,
      qt_meta_data_AddressFilterProxy,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *AddressFilterProxy::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AddressFilterProxy::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_AddressFilterProxy.stringdata0))
        return static_cast<void*>(this);
    return QSortFilterProxyModel::qt_metacast(_clname);
}

int AddressFilterProxy::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QSortFilterProxyModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void AddressFilterProxy::message(const QString & _t1, const QString & _t2, unsigned int _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
