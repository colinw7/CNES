/****************************************************************************
** Meta object code from reading C++ file 'CQNES_Machine.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qinclude/CQNES_Machine.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CQNES_Machine.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CNES__QMachine_t {
    QByteArrayData data[11];
    char stringdata0[149];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CNES__QMachine_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CNES__QMachine_t qt_meta_stringdata_CNES__QMachine = {
    {
QT_MOC_LITERAL(0, 0, 14), // "CNES::QMachine"
QT_MOC_LITERAL(1, 15, 15), // "pcChangedSignal"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 16), // "memChangedSignal"
QT_MOC_LITERAL(4, 49, 4), // "addr"
QT_MOC_LITERAL(5, 54, 3), // "len"
QT_MOC_LITERAL(6, 58, 17), // "handleBreakSignal"
QT_MOC_LITERAL(7, 76, 19), // "breakpointHitSignal"
QT_MOC_LITERAL(8, 96, 17), // "illegalJumpSignal"
QT_MOC_LITERAL(9, 114, 9), // "nmiSignal"
QT_MOC_LITERAL(10, 124, 24) // "breakpointsChangedSignal"

    },
    "CNES::QMachine\0pcChangedSignal\0\0"
    "memChangedSignal\0addr\0len\0handleBreakSignal\0"
    "breakpointHitSignal\0illegalJumpSignal\0"
    "nmiSignal\0breakpointsChangedSignal"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CNES__QMachine[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x06 /* Public */,
       3,    2,   50,    2, 0x06 /* Public */,
       6,    0,   55,    2, 0x06 /* Public */,
       7,    0,   56,    2, 0x06 /* Public */,
       8,    0,   57,    2, 0x06 /* Public */,
       9,    0,   58,    2, 0x06 /* Public */,
      10,    0,   59,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::UShort, QMetaType::UShort,    4,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CNES::QMachine::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QMachine *_t = static_cast<QMachine *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->pcChangedSignal(); break;
        case 1: _t->memChangedSignal((*reinterpret_cast< ushort(*)>(_a[1])),(*reinterpret_cast< ushort(*)>(_a[2]))); break;
        case 2: _t->handleBreakSignal(); break;
        case 3: _t->breakpointHitSignal(); break;
        case 4: _t->illegalJumpSignal(); break;
        case 5: _t->nmiSignal(); break;
        case 6: _t->breakpointsChangedSignal(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (QMachine::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QMachine::pcChangedSignal)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (QMachine::*_t)(ushort , ushort );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QMachine::memChangedSignal)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (QMachine::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QMachine::handleBreakSignal)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (QMachine::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QMachine::breakpointHitSignal)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (QMachine::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QMachine::illegalJumpSignal)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (QMachine::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QMachine::nmiSignal)) {
                *result = 5;
                return;
            }
        }
        {
            typedef void (QMachine::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QMachine::breakpointsChangedSignal)) {
                *result = 6;
                return;
            }
        }
    }
}

const QMetaObject CNES::QMachine::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_CNES__QMachine.data,
      qt_meta_data_CNES__QMachine,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *CNES::QMachine::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CNES::QMachine::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CNES__QMachine.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "Machine"))
        return static_cast< Machine*>(this);
    return QObject::qt_metacast(_clname);
}

int CNES::QMachine::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void CNES::QMachine::pcChangedSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void CNES::QMachine::memChangedSignal(ushort _t1, ushort _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void CNES::QMachine::handleBreakSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void CNES::QMachine::breakpointHitSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void CNES::QMachine::illegalJumpSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void CNES::QMachine::nmiSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void CNES::QMachine::breakpointsChangedSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
