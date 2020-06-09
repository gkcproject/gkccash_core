/****************************************************************************
** Meta object code from reading C++ file 'rpcconsole.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../qt_native/rpcconsole.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'rpcconsole.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_RPCConsole_t {
    QByteArrayData data[69];
    char stringdata0[861];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RPCConsole_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RPCConsole_t qt_meta_stringdata_RPCConsole = {
    {
QT_MOC_LITERAL(0, 0, 10), // "RPCConsole"
QT_MOC_LITERAL(1, 11, 12), // "stopExecutor"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 10), // "cmdRequest"
QT_MOC_LITERAL(4, 36, 7), // "command"
QT_MOC_LITERAL(5, 44, 13), // "handleRestart"
QT_MOC_LITERAL(6, 58, 4), // "args"
QT_MOC_LITERAL(7, 63, 10), // "newMessage"
QT_MOC_LITERAL(8, 74, 7), // "message"
QT_MOC_LITERAL(9, 82, 10), // "infoUpdate"
QT_MOC_LITERAL(10, 93, 4), // "info"
QT_MOC_LITERAL(11, 98, 13), // "trafficUpdate"
QT_MOC_LITERAL(12, 112, 7), // "traffic"
QT_MOC_LITERAL(13, 120, 16), // "requestedRestart"
QT_MOC_LITERAL(14, 137, 27), // "on_tabWidget_currentChanged"
QT_MOC_LITERAL(15, 165, 5), // "index"
QT_MOC_LITERAL(16, 171, 18), // "updateTrafficStats"
QT_MOC_LITERAL(17, 190, 12), // "totalBytesIn"
QT_MOC_LITERAL(18, 203, 13), // "totalBytesOut"
QT_MOC_LITERAL(19, 217, 9), // "showEvent"
QT_MOC_LITERAL(20, 227, 11), // "QShowEvent*"
QT_MOC_LITERAL(21, 239, 5), // "event"
QT_MOC_LITERAL(22, 245, 9), // "hideEvent"
QT_MOC_LITERAL(23, 255, 11), // "QHideEvent*"
QT_MOC_LITERAL(24, 267, 5), // "clear"
QT_MOC_LITERAL(25, 273, 6), // "reject"
QT_MOC_LITERAL(26, 280, 8), // "category"
QT_MOC_LITERAL(27, 289, 4), // "html"
QT_MOC_LITERAL(28, 294, 17), // "setNumConnections"
QT_MOC_LITERAL(29, 312, 5), // "count"
QT_MOC_LITERAL(30, 318, 12), // "setNumBlocks"
QT_MOC_LITERAL(31, 331, 18), // "setMasternodeCount"
QT_MOC_LITERAL(32, 350, 14), // "strMasternodes"
QT_MOC_LITERAL(33, 365, 12), // "peerSelected"
QT_MOC_LITERAL(34, 378, 14), // "QItemSelection"
QT_MOC_LITERAL(35, 393, 8), // "selected"
QT_MOC_LITERAL(36, 402, 10), // "deselected"
QT_MOC_LITERAL(37, 413, 17), // "peerLayoutChanged"
QT_MOC_LITERAL(38, 431, 12), // "hintRowCount"
QT_MOC_LITERAL(39, 444, 8), // "findhint"
QT_MOC_LITERAL(40, 453, 4), // "text"
QT_MOC_LITERAL(41, 458, 25), // "on_lineEdit_returnPressed"
QT_MOC_LITERAL(42, 484, 3), // "cmd"
QT_MOC_LITERAL(43, 488, 12), // "startFromQML"
QT_MOC_LITERAL(44, 501, 13), // "browseHistory"
QT_MOC_LITERAL(45, 515, 6), // "offset"
QT_MOC_LITERAL(46, 522, 14), // "getStartUpInfo"
QT_MOC_LITERAL(47, 537, 16), // "openDebugLogFile"
QT_MOC_LITERAL(48, 554, 20), // "setTrafficGraphRange"
QT_MOC_LITERAL(49, 575, 4), // "mins"
QT_MOC_LITERAL(50, 580, 17), // "getStartUpTraffic"
QT_MOC_LITERAL(51, 598, 14), // "getClientModel"
QT_MOC_LITERAL(52, 613, 12), // "ClientModel*"
QT_MOC_LITERAL(53, 626, 13), // "getWalletPath"
QT_MOC_LITERAL(54, 640, 13), // "changeNetWork"
QT_MOC_LITERAL(55, 654, 13), // "walletSalvage"
QT_MOC_LITERAL(56, 668, 12), // "walletRescan"
QT_MOC_LITERAL(57, 681, 14), // "walletZaptxes1"
QT_MOC_LITERAL(58, 696, 14), // "walletZaptxes2"
QT_MOC_LITERAL(59, 711, 13), // "walletUpgrade"
QT_MOC_LITERAL(60, 725, 13), // "walletReindex"
QT_MOC_LITERAL(61, 739, 12), // "walletResync"
QT_MOC_LITERAL(62, 752, 14), // "showConfEditor"
QT_MOC_LITERAL(63, 767, 16), // "showMNConfEditor"
QT_MOC_LITERAL(64, 784, 11), // "showBackups"
QT_MOC_LITERAL(65, 796, 9), // "hintModel"
QT_MOC_LITERAL(66, 806, 16), // "SingleItemModel*"
QT_MOC_LITERAL(67, 823, 21), // "peerTableModel_sorted"
QT_MOC_LITERAL(68, 845, 15) // "PeerTableProxy*"

    },
    "RPCConsole\0stopExecutor\0\0cmdRequest\0"
    "command\0handleRestart\0args\0newMessage\0"
    "message\0infoUpdate\0info\0trafficUpdate\0"
    "traffic\0requestedRestart\0"
    "on_tabWidget_currentChanged\0index\0"
    "updateTrafficStats\0totalBytesIn\0"
    "totalBytesOut\0showEvent\0QShowEvent*\0"
    "event\0hideEvent\0QHideEvent*\0clear\0"
    "reject\0category\0html\0setNumConnections\0"
    "count\0setNumBlocks\0setMasternodeCount\0"
    "strMasternodes\0peerSelected\0QItemSelection\0"
    "selected\0deselected\0peerLayoutChanged\0"
    "hintRowCount\0findhint\0text\0"
    "on_lineEdit_returnPressed\0cmd\0"
    "startFromQML\0browseHistory\0offset\0"
    "getStartUpInfo\0openDebugLogFile\0"
    "setTrafficGraphRange\0mins\0getStartUpTraffic\0"
    "getClientModel\0ClientModel*\0getWalletPath\0"
    "changeNetWork\0walletSalvage\0walletRescan\0"
    "walletZaptxes1\0walletZaptxes2\0"
    "walletUpgrade\0walletReindex\0walletResync\0"
    "showConfEditor\0showMNConfEditor\0"
    "showBackups\0hintModel\0SingleItemModel*\0"
    "peerTableModel_sorted\0PeerTableProxy*"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RPCConsole[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      42,   14, // methods
       2,  316, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,  224,    2, 0x06 /* Public */,
       3,    1,  225,    2, 0x06 /* Public */,
       5,    1,  228,    2, 0x06 /* Public */,
       7,    1,  231,    2, 0x06 /* Public */,
       9,    1,  234,    2, 0x06 /* Public */,
      11,    1,  237,    2, 0x06 /* Public */,
      13,    1,  240,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      14,    1,  243,    2, 0x08 /* Private */,
      16,    2,  246,    2, 0x08 /* Private */,
      19,    1,  251,    2, 0x08 /* Private */,
      22,    1,  254,    2, 0x08 /* Private */,
      24,    0,  257,    2, 0x0a /* Public */,
      25,    0,  258,    2, 0x0a /* Public */,
       8,    3,  259,    2, 0x0a /* Public */,
       8,    2,  266,    2, 0x2a /* Public | MethodCloned */,
      28,    1,  271,    2, 0x0a /* Public */,
      30,    1,  274,    2, 0x0a /* Public */,
      31,    1,  277,    2, 0x0a /* Public */,
      33,    2,  280,    2, 0x0a /* Public */,
      37,    0,  285,    2, 0x0a /* Public */,

 // methods: name, argc, parameters, tag, flags
      38,    0,  286,    2, 0x02 /* Public */,
      39,    1,  287,    2, 0x02 /* Public */,
      41,    1,  290,    2, 0x02 /* Public */,
      43,    0,  293,    2, 0x02 /* Public */,
      44,    1,  294,    2, 0x02 /* Public */,
      46,    0,  297,    2, 0x02 /* Public */,
      47,    0,  298,    2, 0x02 /* Public */,
      48,    1,  299,    2, 0x02 /* Public */,
      50,    0,  302,    2, 0x02 /* Public */,
      51,    0,  303,    2, 0x02 /* Public */,
      53,    0,  304,    2, 0x02 /* Public */,
      54,    0,  305,    2, 0x02 /* Public */,
      55,    0,  306,    2, 0x02 /* Public */,
      56,    0,  307,    2, 0x02 /* Public */,
      57,    0,  308,    2, 0x02 /* Public */,
      58,    0,  309,    2, 0x02 /* Public */,
      59,    0,  310,    2, 0x02 /* Public */,
      60,    0,  311,    2, 0x02 /* Public */,
      61,    0,  312,    2, 0x02 /* Public */,
      62,    0,  313,    2, 0x02 /* Public */,
      63,    0,  314,    2, 0x02 /* Public */,
      64,    0,  315,    2, 0x02 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void, QMetaType::QStringList,    6,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void, QMetaType::QString,   10,
    QMetaType::Void, QMetaType::QString,   12,
    QMetaType::Void, QMetaType::QStringList,    6,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,   15,
    QMetaType::Void, QMetaType::ULongLong, QMetaType::ULongLong,   17,   18,
    QMetaType::Void, 0x80000000 | 20,   21,
    QMetaType::Void, 0x80000000 | 23,   21,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::QString, QMetaType::Bool,   26,    8,   27,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,   26,    8,
    QMetaType::Void, QMetaType::Int,   29,
    QMetaType::Void, QMetaType::Int,   29,
    QMetaType::Void, QMetaType::QString,   32,
    QMetaType::Void, 0x80000000 | 34, 0x80000000 | 34,   35,   36,
    QMetaType::Void,

 // methods: parameters
    QMetaType::Int,
    QMetaType::Void, QMetaType::QString,   40,
    QMetaType::Void, QMetaType::QString,   42,
    QMetaType::Void,
    QMetaType::QString, QMetaType::Int,   45,
    QMetaType::QString,
    QMetaType::Void,
    QMetaType::QString, QMetaType::Int,   49,
    QMetaType::QString,
    0x80000000 | 52,
    QMetaType::QString,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
      65, 0x80000000 | 66, 0x00095409,
      67, 0x80000000 | 68, 0x00095409,

       0        // eod
};

void RPCConsole::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RPCConsole *_t = static_cast<RPCConsole *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->stopExecutor(); break;
        case 1: _t->cmdRequest((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->handleRestart((*reinterpret_cast< QStringList(*)>(_a[1]))); break;
        case 3: _t->newMessage((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: _t->infoUpdate((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: _t->trafficUpdate((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 6: _t->requestedRestart((*reinterpret_cast< QStringList(*)>(_a[1]))); break;
        case 7: _t->on_tabWidget_currentChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->updateTrafficStats((*reinterpret_cast< quint64(*)>(_a[1])),(*reinterpret_cast< quint64(*)>(_a[2]))); break;
        case 9: _t->showEvent((*reinterpret_cast< QShowEvent*(*)>(_a[1]))); break;
        case 10: _t->hideEvent((*reinterpret_cast< QHideEvent*(*)>(_a[1]))); break;
        case 11: _t->clear(); break;
        case 12: _t->reject(); break;
        case 13: _t->message((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 14: _t->message((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 15: _t->setNumConnections((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 16: _t->setNumBlocks((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 17: _t->setMasternodeCount((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 18: _t->peerSelected((*reinterpret_cast< const QItemSelection(*)>(_a[1])),(*reinterpret_cast< const QItemSelection(*)>(_a[2]))); break;
        case 19: _t->peerLayoutChanged(); break;
        case 20: { int _r = _t->hintRowCount();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 21: _t->findhint((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 22: _t->on_lineEdit_returnPressed((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 23: _t->startFromQML(); break;
        case 24: { QString _r = _t->browseHistory((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 25: { QString _r = _t->getStartUpInfo();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 26: _t->openDebugLogFile(); break;
        case 27: { QString _r = _t->setTrafficGraphRange((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 28: { QString _r = _t->getStartUpTraffic();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 29: { ClientModel* _r = _t->getClientModel();
            if (_a[0]) *reinterpret_cast< ClientModel**>(_a[0]) = std::move(_r); }  break;
        case 30: { QString _r = _t->getWalletPath();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 31: _t->changeNetWork(); break;
        case 32: _t->walletSalvage(); break;
        case 33: _t->walletRescan(); break;
        case 34: _t->walletZaptxes1(); break;
        case 35: _t->walletZaptxes2(); break;
        case 36: _t->walletUpgrade(); break;
        case 37: _t->walletReindex(); break;
        case 38: _t->walletResync(); break;
        case 39: _t->showConfEditor(); break;
        case 40: _t->showMNConfEditor(); break;
        case 41: _t->showBackups(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 18:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QItemSelection >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (RPCConsole::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RPCConsole::stopExecutor)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (RPCConsole::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RPCConsole::cmdRequest)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (RPCConsole::*_t)(QStringList );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RPCConsole::handleRestart)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (RPCConsole::*_t)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RPCConsole::newMessage)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (RPCConsole::*_t)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RPCConsole::infoUpdate)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (RPCConsole::*_t)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RPCConsole::trafficUpdate)) {
                *result = 5;
                return;
            }
        }
        {
            typedef void (RPCConsole::*_t)(QStringList );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RPCConsole::requestedRestart)) {
                *result = 6;
                return;
            }
        }
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< PeerTableProxy* >(); break;
        case 0:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< SingleItemModel* >(); break;
        }
    }

#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        RPCConsole *_t = static_cast<RPCConsole *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< SingleItemModel**>(_v) = _t->m_hintModel; break;
        case 1: *reinterpret_cast< PeerTableProxy**>(_v) = _t->peerTableproxy; break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject RPCConsole::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_RPCConsole.data,
      qt_meta_data_RPCConsole,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *RPCConsole::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RPCConsole::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_RPCConsole.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int RPCConsole::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 42)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 42;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 42)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 42;
    }
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void RPCConsole::stopExecutor()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void RPCConsole::cmdRequest(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void RPCConsole::handleRestart(QStringList _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void RPCConsole::newMessage(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void RPCConsole::infoUpdate(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void RPCConsole::trafficUpdate(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void RPCConsole::requestedRestart(QStringList _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
