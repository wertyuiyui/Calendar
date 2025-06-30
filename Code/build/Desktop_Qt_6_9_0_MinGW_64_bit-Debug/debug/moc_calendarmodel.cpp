/****************************************************************************
** Meta object code from reading C++ file 'calendarmodel.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../calendarmodel.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'calendarmodel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN13CalendarModelE_t {};
} // unnamed namespace

template <> constexpr inline auto CalendarModel::qt_create_metaobjectdata<qt_meta_tag_ZN13CalendarModelE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "CalendarModel",
        "modelReset",
        ""
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'modelReset'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<CalendarModel, qt_meta_tag_ZN13CalendarModelE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject CalendarModel::staticMetaObject = { {
    QMetaObject::SuperData::link<QAbstractTableModel::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13CalendarModelE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13CalendarModelE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN13CalendarModelE_t>.metaTypes,
    nullptr
} };

void CalendarModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<CalendarModel *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->modelReset(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (CalendarModel::*)()>(_a, &CalendarModel::modelReset, 0))
            return;
    }
}

const QMetaObject *CalendarModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CalendarModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13CalendarModelE_t>.strings))
        return static_cast<void*>(this);
    return QAbstractTableModel::qt_metacast(_clname);
}

int CalendarModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractTableModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void CalendarModel::modelReset()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
