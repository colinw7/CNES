/****************************************************************************
** Meta object code from reading C++ file 'CQNES_CPU.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qinclude/CQNES_CPU.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CQNES_CPU.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CNES__QCPU_t {
    QByteArrayData data[12];
    char stringdata0[162];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CNES__QCPU_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CNES__QCPU_t qt_meta_stringdata_CNES__QCPU = {
    {
QT_MOC_LITERAL(0, 0, 10), // "CNES::QCPU"
QT_MOC_LITERAL(1, 11, 15), // "pcChangedSignal"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 16), // "memChangedSignal"
QT_MOC_LITERAL(4, 45, 4), // "addr"
QT_MOC_LITERAL(5, 50, 3), // "len"
QT_MOC_LITERAL(6, 54, 17), // "handleBreakSignal"
QT_MOC_LITERAL(7, 72, 19), // "breakpointHitSignal"
QT_MOC_LITERAL(8, 92, 17), // "illegalJumpSignal"
QT_MOC_LITERAL(9, 110, 9), // "nmiSignal"
QT_MOC_LITERAL(10, 120, 24), // "breakpointsChangedSignal"
QT_MOC_LITERAL(11, 145, 16) // "nesChangedSignal"

    },
    "CNES::QCPU\0pcChangedSignal\0\0"
    "memChangedSignal\0addr\0len\0handleBreakSignal\0"
    "breakpointHitSignal\0illegalJumpSignal\0"
    "nmiSignal\0breakpointsChangedSignal\0"
    "nesChangedSignal"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CNES__QCPU[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x06 /* Public */,
       3,    2,   55,    2, 0x06 /* Public */,
       6,    0,   60,    2, 0x06 /* Public */,
       7,    0,   61,    2, 0x06 /* Public */,
       8,    0,   62,    2, 0x06 /* Public */,
       9,    0,   63,    2, 0x06 /* Public */,
      10,    0,   64,    2, 0x06 /* Public */,
      11,    0,   65,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::UShort, QMetaType::UShort,    4,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CNES::QCPU::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<QCPU *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->pcChangedSignal(); break;
        case 1: _t->memChangedSignal((*reinterpret_cast< ushort(*)>(_a[1])),(*reinterpret_cast< ushort(*)>(_a[2]))); break;
        case 2: _t->handleBreakSignal(); break;
        case 3: _t->breakpointHitSignal(); break;
        case 4: _t->illegalJumpSignal(); break;
        case 5: _t->nmiSignal(); break;
        case 6: _t->breakpointsChangedSignal(); break;
        case 7: _t->nesChangedSignal(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (QCPU::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QCPU::pcChangedSignal)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (QCPU::*)(ushort , ushort );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QCPU::memChangedSignal)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (QCPU::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QCPU::handleBreakSignal)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (QCPU::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QCPU::breakpointHitSignal)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (QCPU::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QCPU::illegalJumpSignal)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (QCPU::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QCPU::nmiSignal)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (QCPU::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QCPU::breakpointsChangedSignal)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (QCPU::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QCPU::nesChangedSignal)) {
                *result = 7;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CNES::QCPU::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_CNES__QCPU.data,
    qt_meta_data_CNES__QCPU,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CNES::QCPU::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CNES::QCPU::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CNES__QCPU.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "CPU"))
        return static_cast< CPU*>(this);
    return QObject::qt_metacast(_clname);
}

int CNES::QCPU::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
void CNES::QCPU::pcChangedSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void CNES::QCPU::memChangedSignal(ushort _t1, ushort _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void CNES::QCPU::handleBreakSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void CNES::QCPU::breakpointHitSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void CNES::QCPU::illegalJumpSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void CNES::QCPU::nmiSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void CNES::QCPU::breakpointsChangedSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void CNES::QCPU::nesChangedSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
