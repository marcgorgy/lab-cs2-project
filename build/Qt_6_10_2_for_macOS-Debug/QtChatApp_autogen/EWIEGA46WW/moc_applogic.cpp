/****************************************************************************
** Meta object code from reading C++ file 'applogic.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../applogic.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'applogic.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.10.2. It"
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
struct qt_meta_tag_ZN8AppLogicE_t {};
} // unnamed namespace

template <> constexpr inline auto AppLogic::qt_create_metaobjectdata<qt_meta_tag_ZN8AppLogicE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "AppLogic",
        "showLoginError",
        "",
        "msg",
        "setLoginConnectingState",
        "connecting",
        "navigateToChatScreen",
        "username",
        "host",
        "port",
        "navigateToLoginScreen",
        "appendChatMessage",
        "sender",
        "text",
        "isOwn",
        "appendSystemMessage",
        "updateNetworkStatus",
        "status",
        "isError",
        "chatScreenAddUser",
        "chatScreenRemoveUser",
        "chatScreenClearUsers",
        "requestNetworkConnect",
        "requestNetworkSendChat",
        "requestNetworkDisconnect",
        "onGuiLoginRequested",
        "onGuiMessageSendRequested",
        "onGuiLogoutRequested",
        "onNetworkConnected",
        "onNetworkConnectionFailed",
        "errorMsg",
        "onNetworkDisconnected",
        "onNetworkIncomingMessage",
        "onNetworkUserJoined",
        "onNetworkUserLeft",
        "onNetworkUserListReceived",
        "users"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'showLoginError'
        QtMocHelpers::SignalData<void(const QString &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Signal 'setLoginConnectingState'
        QtMocHelpers::SignalData<void(bool)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 5 },
        }}),
        // Signal 'navigateToChatScreen'
        QtMocHelpers::SignalData<void(const QString &, const QString &, int)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 }, { QMetaType::QString, 8 }, { QMetaType::Int, 9 },
        }}),
        // Signal 'navigateToLoginScreen'
        QtMocHelpers::SignalData<void()>(10, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'appendChatMessage'
        QtMocHelpers::SignalData<void(const QString &, const QString &, bool)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 12 }, { QMetaType::QString, 13 }, { QMetaType::Bool, 14 },
        }}),
        // Signal 'appendSystemMessage'
        QtMocHelpers::SignalData<void(const QString &)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 13 },
        }}),
        // Signal 'updateNetworkStatus'
        QtMocHelpers::SignalData<void(const QString &, bool)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 17 }, { QMetaType::Bool, 18 },
        }}),
        // Signal 'chatScreenAddUser'
        QtMocHelpers::SignalData<void(const QString &)>(19, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 },
        }}),
        // Signal 'chatScreenRemoveUser'
        QtMocHelpers::SignalData<void(const QString &)>(20, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 },
        }}),
        // Signal 'chatScreenClearUsers'
        QtMocHelpers::SignalData<void()>(21, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'requestNetworkConnect'
        QtMocHelpers::SignalData<void(const QString &, const QString &, int)>(22, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 }, { QMetaType::QString, 8 }, { QMetaType::Int, 9 },
        }}),
        // Signal 'requestNetworkSendChat'
        QtMocHelpers::SignalData<void(const QString &)>(23, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 13 },
        }}),
        // Signal 'requestNetworkDisconnect'
        QtMocHelpers::SignalData<void()>(24, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onGuiLoginRequested'
        QtMocHelpers::SlotData<void(const QString &, const QString &, int)>(25, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 }, { QMetaType::QString, 8 }, { QMetaType::Int, 9 },
        }}),
        // Slot 'onGuiMessageSendRequested'
        QtMocHelpers::SlotData<void(const QString &)>(26, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 13 },
        }}),
        // Slot 'onGuiLogoutRequested'
        QtMocHelpers::SlotData<void()>(27, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onNetworkConnected'
        QtMocHelpers::SlotData<void()>(28, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onNetworkConnectionFailed'
        QtMocHelpers::SlotData<void(const QString &)>(29, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 30 },
        }}),
        // Slot 'onNetworkDisconnected'
        QtMocHelpers::SlotData<void()>(31, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onNetworkIncomingMessage'
        QtMocHelpers::SlotData<void(const QString &, const QString &)>(32, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 12 }, { QMetaType::QString, 13 },
        }}),
        // Slot 'onNetworkUserJoined'
        QtMocHelpers::SlotData<void(const QString &)>(33, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 },
        }}),
        // Slot 'onNetworkUserLeft'
        QtMocHelpers::SlotData<void(const QString &)>(34, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 },
        }}),
        // Slot 'onNetworkUserListReceived'
        QtMocHelpers::SlotData<void(const QStringList &)>(35, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QStringList, 36 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<AppLogic, qt_meta_tag_ZN8AppLogicE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject AppLogic::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8AppLogicE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8AppLogicE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN8AppLogicE_t>.metaTypes,
    nullptr
} };

void AppLogic::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<AppLogic *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->showLoginError((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->setLoginConnectingState((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 2: _t->navigateToChatScreen((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[3]))); break;
        case 3: _t->navigateToLoginScreen(); break;
        case 4: _t->appendChatMessage((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[3]))); break;
        case 5: _t->appendSystemMessage((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->updateNetworkStatus((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[2]))); break;
        case 7: _t->chatScreenAddUser((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 8: _t->chatScreenRemoveUser((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 9: _t->chatScreenClearUsers(); break;
        case 10: _t->requestNetworkConnect((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[3]))); break;
        case 11: _t->requestNetworkSendChat((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 12: _t->requestNetworkDisconnect(); break;
        case 13: _t->onGuiLoginRequested((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[3]))); break;
        case 14: _t->onGuiMessageSendRequested((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 15: _t->onGuiLogoutRequested(); break;
        case 16: _t->onNetworkConnected(); break;
        case 17: _t->onNetworkConnectionFailed((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 18: _t->onNetworkDisconnected(); break;
        case 19: _t->onNetworkIncomingMessage((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 20: _t->onNetworkUserJoined((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 21: _t->onNetworkUserLeft((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 22: _t->onNetworkUserListReceived((*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (AppLogic::*)(const QString & )>(_a, &AppLogic::showLoginError, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppLogic::*)(bool )>(_a, &AppLogic::setLoginConnectingState, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppLogic::*)(const QString & , const QString & , int )>(_a, &AppLogic::navigateToChatScreen, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppLogic::*)()>(_a, &AppLogic::navigateToLoginScreen, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppLogic::*)(const QString & , const QString & , bool )>(_a, &AppLogic::appendChatMessage, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppLogic::*)(const QString & )>(_a, &AppLogic::appendSystemMessage, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppLogic::*)(const QString & , bool )>(_a, &AppLogic::updateNetworkStatus, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppLogic::*)(const QString & )>(_a, &AppLogic::chatScreenAddUser, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppLogic::*)(const QString & )>(_a, &AppLogic::chatScreenRemoveUser, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppLogic::*)()>(_a, &AppLogic::chatScreenClearUsers, 9))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppLogic::*)(const QString & , const QString & , int )>(_a, &AppLogic::requestNetworkConnect, 10))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppLogic::*)(const QString & )>(_a, &AppLogic::requestNetworkSendChat, 11))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppLogic::*)()>(_a, &AppLogic::requestNetworkDisconnect, 12))
            return;
    }
}

const QMetaObject *AppLogic::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AppLogic::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8AppLogicE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int AppLogic::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 23)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 23;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 23)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 23;
    }
    return _id;
}

// SIGNAL 0
void AppLogic::showLoginError(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void AppLogic::setLoginConnectingState(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void AppLogic::navigateToChatScreen(const QString & _t1, const QString & _t2, int _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1, _t2, _t3);
}

// SIGNAL 3
void AppLogic::navigateToLoginScreen()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void AppLogic::appendChatMessage(const QString & _t1, const QString & _t2, bool _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1, _t2, _t3);
}

// SIGNAL 5
void AppLogic::appendSystemMessage(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void AppLogic::updateNetworkStatus(const QString & _t1, bool _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1, _t2);
}

// SIGNAL 7
void AppLogic::chatScreenAddUser(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1);
}

// SIGNAL 8
void AppLogic::chatScreenRemoveUser(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 8, nullptr, _t1);
}

// SIGNAL 9
void AppLogic::chatScreenClearUsers()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}

// SIGNAL 10
void AppLogic::requestNetworkConnect(const QString & _t1, const QString & _t2, int _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 10, nullptr, _t1, _t2, _t3);
}

// SIGNAL 11
void AppLogic::requestNetworkSendChat(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 11, nullptr, _t1);
}

// SIGNAL 12
void AppLogic::requestNetworkDisconnect()
{
    QMetaObject::activate(this, &staticMetaObject, 12, nullptr);
}
QT_WARNING_POP
