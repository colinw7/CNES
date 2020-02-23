/****************************************************************************
** Meta object code from reading C++ file 'CQNES_PPU.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qinclude/CQNES_PPU.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CQNES_PPU.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CNES__QPPU_t {
    QByteArrayData data[13];
    char stringdata0[134];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CNES__QPPU_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CNES__QPPU_t qt_meta_stringdata_CNES__QPPU = {
    {
QT_MOC_LITERAL(0, 0, 10), // "CNES::QPPU"
QT_MOC_LITERAL(1, 11, 16), // "memChangedSignal"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 4), // "addr"
QT_MOC_LITERAL(4, 34, 3), // "len"
QT_MOC_LITERAL(5, 38, 20), // "spritesChangedSignal"
QT_MOC_LITERAL(6, 59, 12), // "drawLineSlot"
QT_MOC_LITERAL(7, 72, 13), // "showNameTable"
QT_MOC_LITERAL(8, 86, 11), // "showSprites"
QT_MOC_LITERAL(9, 98, 9), // "showDebug"
QT_MOC_LITERAL(10, 108, 5), // "scale"
QT_MOC_LITERAL(11, 114, 6), // "margin"
QT_MOC_LITERAL(12, 121, 12) // "showScanLine"

    },
    "CNES::QPPU\0memChangedSignal\0\0addr\0len\0"
    "spritesChangedSignal\0drawLineSlot\0"
    "showNameTable\0showSprites\0showDebug\0"
    "scale\0margin\0showScanLine"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CNES__QPPU[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       3,   60, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   44,    2, 0x06 /* Public */,
       5,    0,   49,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    0,   50,    2, 0x08 /* Private */,
       7,    1,   51,    2, 0x08 /* Private */,
       8,    1,   54,    2, 0x08 /* Private */,
       9,    1,   57,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::UShort, QMetaType::UShort,    3,    4,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,

 // properties: name, type, flags
      10, QMetaType::Int, 0x00095103,
      11, QMetaType::Int, 0x00095103,
      12, QMetaType::Bool, 0x00095103,

       0        // eod
};

void CNES::QPPU::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QPPU *_t = static_cast<QPPU *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->memChangedSignal((*reinterpret_cast< ushort(*)>(_a[1])),(*reinterpret_cast< ushort(*)>(_a[2]))); break;
        case 1: _t->spritesChangedSignal(); break;
        case 2: _t->drawLineSlot(); break;
        case 3: _t->showNameTable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->showSprites((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->showDebug((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (QPPU::*_t)(ushort , ushort );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QPPU::memChangedSignal)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (QPPU::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QPPU::spritesChangedSignal)) {
                *result = 1;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        QPPU *_t = static_cast<QPPU *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->scale(); break;
        case 1: *reinterpret_cast< int*>(_v) = _t->margin(); break;
        case 2: *reinterpret_cast< bool*>(_v) = _t->isShowScanLine(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        QPPU *_t = static_cast<QPPU *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setScale(*reinterpret_cast< int*>(_v)); break;
        case 1: _t->setMargin(*reinterpret_cast< int*>(_v)); break;
        case 2: _t->setShowScanLine(*reinterpret_cast< bool*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject CNES::QPPU::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CNES__QPPU.data,
      qt_meta_data_CNES__QPPU,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *CNES::QPPU::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CNES::QPPU::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CNES__QPPU.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "PPU"))
        return static_cast< PPU*>(this);
    return QWidget::qt_metacast(_clname);
}

int CNES::QPPU::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 3;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void CNES::QPPU::memChangedSignal(ushort _t1, ushort _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CNES::QPPU::spritesChangedSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
