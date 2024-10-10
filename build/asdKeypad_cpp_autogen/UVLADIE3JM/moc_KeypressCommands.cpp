/****************************************************************************
** Meta object code from reading C++ file 'KeypressCommands.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/KeypressCommands.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KeypressCommands.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KeypressCommands_t {
    QByteArrayData data[14];
    char stringdata0[192];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KeypressCommands_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KeypressCommands_t qt_meta_stringdata_KeypressCommands = {
    {
QT_MOC_LITERAL(0, 0, 16), // "KeypressCommands"
QT_MOC_LITERAL(1, 17, 13), // "sendKeypress1"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 13), // "sendKeypress2"
QT_MOC_LITERAL(4, 46, 13), // "sendKeypress3"
QT_MOC_LITERAL(5, 60, 13), // "sendKeypress4"
QT_MOC_LITERAL(6, 74, 13), // "sendKeypress5"
QT_MOC_LITERAL(7, 88, 13), // "sendKeypress6"
QT_MOC_LITERAL(8, 102, 13), // "sendKeypress7"
QT_MOC_LITERAL(9, 116, 13), // "sendKeypress8"
QT_MOC_LITERAL(10, 130, 13), // "sendKeypress9"
QT_MOC_LITERAL(11, 144, 13), // "sendKeypress0"
QT_MOC_LITERAL(12, 158, 16), // "sendKeypressStar"
QT_MOC_LITERAL(13, 175, 16) // "sendKeypressHash"

    },
    "KeypressCommands\0sendKeypress1\0\0"
    "sendKeypress2\0sendKeypress3\0sendKeypress4\0"
    "sendKeypress5\0sendKeypress6\0sendKeypress7\0"
    "sendKeypress8\0sendKeypress9\0sendKeypress0\0"
    "sendKeypressStar\0sendKeypressHash"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KeypressCommands[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   74,    2, 0x0a /* Public */,
       3,    0,   75,    2, 0x0a /* Public */,
       4,    0,   76,    2, 0x0a /* Public */,
       5,    0,   77,    2, 0x0a /* Public */,
       6,    0,   78,    2, 0x0a /* Public */,
       7,    0,   79,    2, 0x0a /* Public */,
       8,    0,   80,    2, 0x0a /* Public */,
       9,    0,   81,    2, 0x0a /* Public */,
      10,    0,   82,    2, 0x0a /* Public */,
      11,    0,   83,    2, 0x0a /* Public */,
      12,    0,   84,    2, 0x0a /* Public */,
      13,    0,   85,    2, 0x0a /* Public */,

 // slots: parameters
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
    QMetaType::Void,

       0        // eod
};

void KeypressCommands::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<KeypressCommands *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->sendKeypress1(); break;
        case 1: _t->sendKeypress2(); break;
        case 2: _t->sendKeypress3(); break;
        case 3: _t->sendKeypress4(); break;
        case 4: _t->sendKeypress5(); break;
        case 5: _t->sendKeypress6(); break;
        case 6: _t->sendKeypress7(); break;
        case 7: _t->sendKeypress8(); break;
        case 8: _t->sendKeypress9(); break;
        case 9: _t->sendKeypress0(); break;
        case 10: _t->sendKeypressStar(); break;
        case 11: _t->sendKeypressHash(); break;
        default: ;
        }
    }
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject KeypressCommands::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_KeypressCommands.data,
    qt_meta_data_KeypressCommands,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *KeypressCommands::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KeypressCommands::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KeypressCommands.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int KeypressCommands::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
