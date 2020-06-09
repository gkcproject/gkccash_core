/****************************************************************************
** Meta object code from reading C++ file 'peertableproxy.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../qt_native/peertableproxy.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'peertableproxy.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_PeerTableProxy_t {
    QByteArrayData data[11];
    char stringdata0[130];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PeerTableProxy_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PeerTableProxy_t qt_meta_stringdata_PeerTableProxy = {
    {
QT_MOC_LITERAL(0, 0, 14), // "PeerTableProxy"
QT_MOC_LITERAL(1, 15, 10), // "sortColumn"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 8), // "roleName"
QT_MOC_LITERAL(4, 36, 13), // "Qt::SortOrder"
QT_MOC_LITERAL(5, 50, 5), // "order"
QT_MOC_LITERAL(6, 56, 16), // "getPeerTitleInfo"
QT_MOC_LITERAL(7, 73, 3), // "row"
QT_MOC_LITERAL(8, 77, 19), // "getPeerDetailedInfo"
QT_MOC_LITERAL(9, 97, 16), // "startAutoRefresh"
QT_MOC_LITERAL(10, 114, 15) // "stopAutoRefresh"

    },
    "PeerTableProxy\0sortColumn\0\0roleName\0"
    "Qt::SortOrder\0order\0getPeerTitleInfo\0"
    "row\0getPeerDetailedInfo\0startAutoRefresh\0"
    "stopAutoRefresh"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PeerTableProxy[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // methods: name, argc, parameters, tag, flags
       1,    2,   39,    2, 0x02 /* Public */,
       6,    1,   44,    2, 0x02 /* Public */,
       8,    1,   47,    2, 0x02 /* Public */,
       9,    0,   50,    2, 0x02 /* Public */,
      10,    0,   51,    2, 0x02 /* Public */,

 // methods: parameters
    QMetaType::Void, QMetaType::QString, 0x80000000 | 4,    3,    5,
    QMetaType::QString, QMetaType::Int,    7,
    QMetaType::QString, QMetaType::Int,    7,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void PeerTableProxy::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PeerTableProxy *_t = static_cast<PeerTableProxy *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sortColumn((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< Qt::SortOrder(*)>(_a[2]))); break;
        case 1: { QString _r = _t->getPeerTitleInfo((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 2: { QString _r = _t->getPeerDetailedInfo((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 3: _t->startAutoRefresh(); break;
        case 4: _t->stopAutoRefresh(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject PeerTableProxy::staticMetaObject = {
    { &QSortFilterProxyModel::staticMetaObject, qt_meta_stringdata_PeerTableProxy.data,
      qt_meta_data_PeerTableProxy,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *PeerTableProxy::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PeerTableProxy::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PeerTableProxy.stringdata0))
        return static_cast<void*>(this);
    return QSortFilterProxyModel::qt_metacast(_clname);
}

int PeerTableProxy::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QSortFilterProxyModel::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
