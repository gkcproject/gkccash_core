/****************************************************************************
** Meta object code from reading C++ file 'recentrequestsfilterproxy.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../qt_native/recentrequestsfilterproxy.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'recentrequestsfilterproxy.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_RecentRequestsFilterProxy_t {
    QByteArrayData data[24];
    char stringdata0[237];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RecentRequestsFilterProxy_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RecentRequestsFilterProxy_t qt_meta_stringdata_RecentRequestsFilterProxy = {
    {
QT_MOC_LITERAL(0, 0, 25), // "RecentRequestsFilterProxy"
QT_MOC_LITERAL(1, 26, 23), // "sendCoinsRecipientReady"
QT_MOC_LITERAL(2, 50, 0), // ""
QT_MOC_LITERAL(3, 51, 4), // "info"
QT_MOC_LITERAL(4, 56, 7), // "address"
QT_MOC_LITERAL(5, 64, 5), // "label"
QT_MOC_LITERAL(6, 70, 10), // "sortColumn"
QT_MOC_LITERAL(7, 81, 8), // "roleName"
QT_MOC_LITERAL(8, 90, 13), // "Qt::SortOrder"
QT_MOC_LITERAL(9, 104, 5), // "order"
QT_MOC_LITERAL(10, 110, 7), // "getData"
QT_MOC_LITERAL(11, 118, 9), // "sourceRow"
QT_MOC_LITERAL(12, 128, 10), // "newRequest"
QT_MOC_LITERAL(13, 139, 7), // "message"
QT_MOC_LITERAL(14, 147, 9), // "amountStr"
QT_MOC_LITERAL(15, 157, 4), // "unit"
QT_MOC_LITERAL(16, 162, 9), // "saveImage"
QT_MOC_LITERAL(17, 172, 9), // "copyImage"
QT_MOC_LITERAL(18, 182, 7), // "copyURI"
QT_MOC_LITERAL(19, 190, 11), // "copyAddress"
QT_MOC_LITERAL(20, 202, 11), // "showAddress"
QT_MOC_LITERAL(21, 214, 3), // "row"
QT_MOC_LITERAL(22, 218, 13), // "removeAddress"
QT_MOC_LITERAL(23, 232, 4) // "rows"

    },
    "RecentRequestsFilterProxy\0"
    "sendCoinsRecipientReady\0\0info\0address\0"
    "label\0sortColumn\0roleName\0Qt::SortOrder\0"
    "order\0getData\0sourceRow\0newRequest\0"
    "message\0amountStr\0unit\0saveImage\0"
    "copyImage\0copyURI\0copyAddress\0showAddress\0"
    "row\0removeAddress\0rows"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RecentRequestsFilterProxy[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   64,    2, 0x06 /* Public */,

 // methods: name, argc, parameters, tag, flags
       6,    2,   71,    2, 0x02 /* Public */,
      10,    2,   76,    2, 0x02 /* Public */,
      12,    5,   81,    2, 0x02 /* Public */,
      16,    0,   92,    2, 0x02 /* Public */,
      17,    0,   93,    2, 0x02 /* Public */,
      18,    0,   94,    2, 0x02 /* Public */,
      19,    0,   95,    2, 0x02 /* Public */,
      20,    1,   96,    2, 0x02 /* Public */,
      22,    1,   99,    2, 0x02 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString,    3,    4,    5,

 // methods: parameters
    QMetaType::Void, QMetaType::QString, 0x80000000 | 8,    7,    9,
    QMetaType::QVariant, QMetaType::QString, QMetaType::Int,    7,   11,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::Int,    5,    4,   13,   14,   15,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   21,
    QMetaType::Void, QMetaType::QVariantList,   23,

       0        // eod
};

void RecentRequestsFilterProxy::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RecentRequestsFilterProxy *_t = static_cast<RecentRequestsFilterProxy *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sendCoinsRecipientReady((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 1: _t->sortColumn((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< Qt::SortOrder(*)>(_a[2]))); break;
        case 2: { QVariant _r = _t->getData((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QVariant*>(_a[0]) = std::move(_r); }  break;
        case 3: _t->newRequest((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4])),(*reinterpret_cast< int(*)>(_a[5]))); break;
        case 4: _t->saveImage(); break;
        case 5: _t->copyImage(); break;
        case 6: _t->copyURI(); break;
        case 7: _t->copyAddress(); break;
        case 8: _t->showAddress((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->removeAddress((*reinterpret_cast< QVariantList(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (RecentRequestsFilterProxy::*_t)(QString , QString , QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RecentRequestsFilterProxy::sendCoinsRecipientReady)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject RecentRequestsFilterProxy::staticMetaObject = {
    { &QSortFilterProxyModel::staticMetaObject, qt_meta_stringdata_RecentRequestsFilterProxy.data,
      qt_meta_data_RecentRequestsFilterProxy,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *RecentRequestsFilterProxy::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RecentRequestsFilterProxy::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_RecentRequestsFilterProxy.stringdata0))
        return static_cast<void*>(this);
    return QSortFilterProxyModel::qt_metacast(_clname);
}

int RecentRequestsFilterProxy::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QSortFilterProxyModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void RecentRequestsFilterProxy::sendCoinsRecipientReady(QString _t1, QString _t2, QString _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
