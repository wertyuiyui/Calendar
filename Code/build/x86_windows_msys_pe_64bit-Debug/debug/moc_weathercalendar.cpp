/****************************************************************************
** Meta object code from reading C++ file 'weathercalendar.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../weathercalendar.h"
#include <QtNetwork/QSslError>
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'weathercalendar.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN14BirthdayDialogE_t {};
} // unnamed namespace

template <> constexpr inline auto BirthdayDialog::qt_create_metaobjectdata<qt_meta_tag_ZN14BirthdayDialogE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "BirthdayDialog",
        "addBirthday",
        "",
        "editBirthday",
        "deleteBirthday"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'addBirthday'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'editBirthday'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'deleteBirthday'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<BirthdayDialog, qt_meta_tag_ZN14BirthdayDialogE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject BirthdayDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14BirthdayDialogE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14BirthdayDialogE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN14BirthdayDialogE_t>.metaTypes,
    nullptr
} };

void BirthdayDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<BirthdayDialog *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->addBirthday(); break;
        case 1: _t->editBirthday(); break;
        case 2: _t->deleteBirthday(); break;
        default: ;
        }
    }
    (void)_a;
}

const QMetaObject *BirthdayDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BirthdayDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14BirthdayDialogE_t>.strings))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int BirthdayDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 3;
    }
    return _id;
}
namespace {
struct qt_meta_tag_ZN13FortuneDialogE_t {};
} // unnamed namespace

template <> constexpr inline auto FortuneDialog::qt_create_metaobjectdata<qt_meta_tag_ZN13FortuneDialogE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "FortuneDialog"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<FortuneDialog, qt_meta_tag_ZN13FortuneDialogE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject FortuneDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13FortuneDialogE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13FortuneDialogE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN13FortuneDialogE_t>.metaTypes,
    nullptr
} };

void FortuneDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<FortuneDialog *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *FortuneDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FortuneDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13FortuneDialogE_t>.strings))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int FortuneDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN17InspirationDialogE_t {};
} // unnamed namespace

template <> constexpr inline auto InspirationDialog::qt_create_metaobjectdata<qt_meta_tag_ZN17InspirationDialogE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "InspirationDialog"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<InspirationDialog, qt_meta_tag_ZN17InspirationDialogE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject InspirationDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17InspirationDialogE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17InspirationDialogE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN17InspirationDialogE_t>.metaTypes,
    nullptr
} };

void InspirationDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<InspirationDialog *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *InspirationDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *InspirationDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17InspirationDialogE_t>.strings))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int InspirationDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN14ScheduleDialogE_t {};
} // unnamed namespace

template <> constexpr inline auto ScheduleDialog::qt_create_metaobjectdata<qt_meta_tag_ZN14ScheduleDialogE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "ScheduleDialog",
        "addSchedule",
        "",
        "deleteSchedule",
        "clearSchedules"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'addSchedule'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'deleteSchedule'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'clearSchedules'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ScheduleDialog, qt_meta_tag_ZN14ScheduleDialogE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject ScheduleDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14ScheduleDialogE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14ScheduleDialogE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN14ScheduleDialogE_t>.metaTypes,
    nullptr
} };

void ScheduleDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ScheduleDialog *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->addSchedule(); break;
        case 1: _t->deleteSchedule(); break;
        case 2: _t->clearSchedules(); break;
        default: ;
        }
    }
    (void)_a;
}

const QMetaObject *ScheduleDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScheduleDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14ScheduleDialogE_t>.strings))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int ScheduleDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 3;
    }
    return _id;
}
namespace {
struct qt_meta_tag_ZN15WeatherCalendarE_t {};
} // unnamed namespace

template <> constexpr inline auto WeatherCalendar::qt_create_metaobjectdata<qt_meta_tag_ZN15WeatherCalendarE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "WeatherCalendar",
        "handleWeatherReply",
        "",
        "QNetworkReply*",
        "reply",
        "updateWeather",
        "prevMonth",
        "nextMonth",
        "showFortune",
        "handleFortuneReply",
        "retryFortune",
        "showInspiration",
        "showBirthdayDialog"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'handleWeatherReply'
        QtMocHelpers::SlotData<void(QNetworkReply *)>(1, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Slot 'updateWeather'
        QtMocHelpers::SlotData<void()>(5, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'prevMonth'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'nextMonth'
        QtMocHelpers::SlotData<void()>(7, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'showFortune'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'handleFortuneReply'
        QtMocHelpers::SlotData<void(QNetworkReply *)>(9, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Slot 'retryFortune'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'showInspiration'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'showBirthdayDialog'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<WeatherCalendar, qt_meta_tag_ZN15WeatherCalendarE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject WeatherCalendar::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15WeatherCalendarE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15WeatherCalendarE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN15WeatherCalendarE_t>.metaTypes,
    nullptr
} };

void WeatherCalendar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<WeatherCalendar *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->handleWeatherReply((*reinterpret_cast< std::add_pointer_t<QNetworkReply*>>(_a[1]))); break;
        case 1: _t->updateWeather(); break;
        case 2: _t->prevMonth(); break;
        case 3: _t->nextMonth(); break;
        case 4: _t->showFortune(); break;
        case 5: _t->handleFortuneReply((*reinterpret_cast< std::add_pointer_t<QNetworkReply*>>(_a[1]))); break;
        case 6: _t->retryFortune(); break;
        case 7: _t->showInspiration(); break;
        case 8: _t->showBirthdayDialog(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QNetworkReply* >(); break;
            }
            break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QNetworkReply* >(); break;
            }
            break;
        }
    }
}

const QMetaObject *WeatherCalendar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WeatherCalendar::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15WeatherCalendarE_t>.strings))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int WeatherCalendar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}
QT_WARNING_POP
