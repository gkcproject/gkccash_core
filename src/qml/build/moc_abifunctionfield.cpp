/****************************************************************************
** Meta object code from reading C++ file 'abifunctionfield.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../qt_native/abifunctionfield.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'abifunctionfield.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ABIFunctionField_t {
    QByteArrayData data[8];
    char stringdata0[89];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ABIFunctionField_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ABIFunctionField_t qt_meta_stringdata_ABIFunctionField = {
    {
QT_MOC_LITERAL(0, 0, 16), // "ABIFunctionField"
QT_MOC_LITERAL(1, 17, 15), // "functionChanged"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 22), // "on_currentIndexChanged"
QT_MOC_LITERAL(4, 57, 12), // "FunctionType"
QT_MOC_LITERAL(5, 70, 6), // "Create"
QT_MOC_LITERAL(6, 77, 6), // "SendTo"
QT_MOC_LITERAL(7, 84, 4) // "Call"

    },
    "ABIFunctionField\0functionChanged\0\0"
    "on_currentIndexChanged\0FunctionType\0"
    "Create\0SendTo\0Call"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ABIFunctionField[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       1,   26, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   24,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   25,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,

 // enums: name, flags, count, data
       4, 0x0,    3,   30,

 // enum data: key, value
       5, uint(ABIFunctionField::Create),
       6, uint(ABIFunctionField::SendTo),
       7, uint(ABIFunctionField::Call),

       0        // eod
};

void ABIFunctionField::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ABIFunctionField *_t = static_cast<ABIFunctionField *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->functionChanged(); break;
        case 1: _t->on_currentIndexChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (ABIFunctionField::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ABIFunctionField::functionChanged)) {
                *result = 0;
                return;
            }
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject ABIFunctionField::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ABIFunctionField.data,
      qt_meta_data_ABIFunctionField,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ABIFunctionField::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ABIFunctionField::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ABIFunctionField.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ABIFunctionField::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void ABIFunctionField::functionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
