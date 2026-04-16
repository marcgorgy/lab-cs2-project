#include "ChatRoomScreen.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QSplitter>
#include <QFrame>
#include <QDateTime>
#include <QScrollBar>

ChatRoomScreen::ChatRoomScreen(QWidget *parent)
    : QWidget(parent)
{
    buildUi();
    applyStyles();
}

void ChatRoomScreen::buildUi()
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ── Top bar ──────────────────────────────────────────────────────────
    auto *topBar = new QFrame(this);
    topBar->setObjectName("topBar");
    topBar->setFixedHeight(56);

    auto *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(16, 0, 16, 0);

    m_titleLabel = new QLabel("# general", topBar);
    m_titleLabel->setObjectName("roomTitle");

    m_statusLabel = new QLabel("● Connecting…", topBar);
    m_statusLabel->setObjectName("networkStatus");

    auto *spacer = new QWidget(topBar);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    m_settingsBtn = new QPushButton("⚙  Settings", topBar);
    m_settingsBtn->setObjectName("iconBtn");
    m_settingsBtn->setCursor(Qt::PointingHandCursor);

    m_logoutBtn = new QPushButton("⏻  Logout", topBar);
    m_logoutBtn->setObjectName("dangerBtn");
    m_logoutBtn->setCursor(Qt::PointingHandCursor);

    topLayout->addWidget(m_titleLabel);
    topLayout->addSpacing(16);
    topLayout->addWidget(m_statusLabel);
    topLayout->addWidget(spacer);
    topLayout->addWidget(m_settingsBtn);
    topLayout->addSpacing(8);
    topLayout->addWidget(m_logoutBtn);

    root->addWidget(topBar);

    // ── Middle area: chat + user list ─────────────────────────────────
    auto *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->setObjectName("mainSplitter");
    splitter->setHandleWidth(1);

    // Chat display
    m_chatDisplay = new QTextEdit(splitter);
    m_chatDisplay->setObjectName("chatDisplay");
    m_chatDisplay->setReadOnly(true);

    // User list panel
    auto *userPanel = new QFrame(splitter);
    userPanel->setObjectName("userPanel");
    userPanel->setFixedWidth(180);

    auto *userLayout = new QVBoxLayout(userPanel);
    userLayout->setContentsMargins(0, 0, 0, 0);
    userLayout->setSpacing(0);

    auto *userHeader = new QLabel("  Online", userPanel);
    userHeader->setObjectName("userPanelHeader");
    userHeader->setFixedHeight(36);

    m_userList = new QListWidget(userPanel);
    m_userList->setObjectName("userList");

    userLayout->addWidget(userHeader);
    userLayout->addWidget(m_userList);

    splitter->addWidget(m_chatDisplay);
    splitter->addWidget(userPanel);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 0);

    root->addWidget(splitter, 1);

    // ── Input bar ────────────────────────────────────────────────────────
    auto *inputBar = new QFrame(this);
    inputBar->setObjectName("inputBar");
    inputBar->setFixedHeight(60);

    auto *inputLayout = new QHBoxLayout(inputBar);
    inputLayout->setContentsMargins(16, 8, 16, 8);
    inputLayout->setSpacing(8);

    m_messageInput = new QLineEdit(inputBar);
    m_messageInput->setObjectName("messageInput");
    m_messageInput->setPlaceholderText("Type a message…");

    m_sendBtn = new QPushButton("Send", inputBar);
    m_sendBtn->setObjectName("sendBtn");
    m_sendBtn->setFixedWidth(80);
    m_sendBtn->setCursor(Qt::PointingHandCursor);

    inputLayout->addWidget(m_messageInput);
    inputLayout->addWidget(m_sendBtn);

    root->addWidget(inputBar);

    // Connections
    connect(m_sendBtn,      &QPushButton::clicked,    this, &ChatRoomScreen::onSendClicked);
    connect(m_messageInput, &QLineEdit::returnPressed, this, &ChatRoomScreen::onSendClicked);
    connect(m_settingsBtn,  &QPushButton::clicked, this, [this]() {
        emit settingsRequested(m_username, m_host, m_port);
    });
    connect(m_logoutBtn, &QPushButton::clicked, this, &ChatRoomScreen::logoutRequested);
}

void ChatRoomScreen::applyStyles()
{
    setStyleSheet(R"(
        ChatRoomScreen { background: #1a1a2e; }

        #topBar {
            background: #0f3460;
            border-bottom: 1px solid rgba(255,255,255,0.08);
        }

        #roomTitle {
            font-size: 16px;
            font-weight: 700;
            color: #ffffff;
        }

        #networkStatus {
            font-size: 12px;
            color: #4caf50;
        }

        #iconBtn {
            background: rgba(255,255,255,0.08);
            color: rgba(255,255,255,0.8);
            border: none;
            border-radius: 6px;
            padding: 6px 12px;
            font-size: 12px;
        }
        #iconBtn:hover { background: rgba(255,255,255,0.15); }

        #dangerBtn {
            background: rgba(233,69,96,0.15);
            color: #e94560;
            border: 1px solid rgba(233,69,96,0.3);
            border-radius: 6px;
            padding: 6px 12px;
            font-size: 12px;
        }
        #dangerBtn:hover { background: rgba(233,69,96,0.25); }

        #chatDisplay {
            background: #1a1a2e;
            border: none;
            color: #d0d0d0;
            font-size: 13px;
            padding: 12px;
            selection-background-color: #e94560;
        }

        #userPanel {
            background: #16213e;
            border-left: 1px solid rgba(255,255,255,0.06);
        }

        #userPanelHeader {
            color: rgba(255,255,255,0.4);
            font-size: 11px;
            font-weight: 700;
            letter-spacing: 1px;
            text-transform: uppercase;
            border-bottom: 1px solid rgba(255,255,255,0.06);
            padding-left: 12px;
        }

        #userList {
            background: transparent;
            border: none;
            color: rgba(255,255,255,0.75);
            font-size: 13px;
        }
        #userList::item { padding: 6px 12px; border-radius: 4px; }
        #userList::item:hover { background: rgba(255,255,255,0.06); }

        #inputBar {
            background: #0f3460;
            border-top: 1px solid rgba(255,255,255,0.08);
        }

        #messageInput {
            background: rgba(255,255,255,0.06);
            border: 1px solid rgba(255,255,255,0.12);
            border-radius: 8px;
            color: #ffffff;
            padding: 8px 14px;
            font-size: 13px;
        }
        #messageInput:focus {
            border: 1px solid #e94560;
        }

        #sendBtn {
            background: #e94560;
            color: white;
            border: none;
            border-radius: 8px;
            font-size: 13px;
            font-weight: 600;
        }
        #sendBtn:hover   { background: #c73652; }
        #sendBtn:pressed { background: #a52d44; }
        #sendBtn:disabled {
            background: rgba(233,69,96,0.3);
            color: rgba(255,255,255,0.3);
        }

        QScrollBar:vertical {
            background: transparent; width: 6px;
        }
        QScrollBar::handle:vertical {
            background: rgba(255,255,255,0.2); border-radius: 3px;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
    )");
}

// ── Public interface ──────────────────────────────────────────────────────────

void ChatRoomScreen::configure(const QString &username, const QString &host, int port)
{
    m_username = username;
    m_host     = host;
    m_port     = port;
    m_titleLabel->setText(QString("# general  —  %1").arg(username));
    setNetworkStatus("Connecting…", false);
    m_chatDisplay->clear();
    m_userList->clear();
    m_messageInput->setEnabled(false);
    m_sendBtn->setEnabled(false);
}

void ChatRoomScreen::appendMessage(const QString &sender, const QString &text, bool isOwn)
{
    const QString time  = QDateTime::currentDateTime().toString("hh:mm");
    const QString color = isOwn ? "#e94560" : "#7ec8e3";
    m_chatDisplay->append(
        QString("<span style='color:%1;font-weight:700'>%2</span> "
                "<span style='color:rgba(255,255,255,0.3);font-size:11px'>[%3]</span> "
                "<span style='color:#d0d0d0'>%4</span>")
            .arg(color, sender.toHtmlEscaped(), time, text.toHtmlEscaped())
    );
    // Scroll to bottom
    m_chatDisplay->verticalScrollBar()->setValue(
        m_chatDisplay->verticalScrollBar()->maximum());
}

void ChatRoomScreen::appendSystemMessage(const QString &text)
{
    m_chatDisplay->append(
        QString("<span style='color:rgba(255,255,255,0.35);font-style:italic;font-size:12px'>%1</span>")
            .arg(text.toHtmlEscaped())
    );
}

void ChatRoomScreen::setNetworkStatus(const QString &status, bool isError)
{
    const QString color = isError ? "#ff6b6b" : "#4caf50";
    const QString dot   = isError ? "✕" : "●";
    m_statusLabel->setText(
        QString("<span style='color:%1'>%2 %3</span>").arg(color, dot, status)
    );
    m_statusLabel->setTextFormat(Qt::RichText);
}

void ChatRoomScreen::setConnected(bool connected)
{
    m_messageInput->setEnabled(connected);
    m_sendBtn->setEnabled(connected);
    setNetworkStatus(connected ? "Connected" : "Disconnected", !connected);
    if (connected) {
        m_messageInput->setPlaceholderText("Type a message…");
        m_messageInput->setFocus();
    } else {
        m_messageInput->setPlaceholderText("Not connected");
    }
}

void ChatRoomScreen::addUser(const QString &username)
{
    m_userList->addItem("  🟢  " + username);
}

void ChatRoomScreen::removeUser(const QString &username)
{
    for (int i = 0; i < m_userList->count(); ++i) {
        if (m_userList->item(i)->text().contains(username)) {
            delete m_userList->takeItem(i);
            break;
        }
    }
}

void ChatRoomScreen::clearUsers()
{
    m_userList->clear();
}

// ── Private slots ─────────────────────────────────────────────────────────────

void ChatRoomScreen::onSendClicked()
{
    const QString text = m_messageInput->text().trimmed();
    if (text.isEmpty()) return;
    m_messageInput->clear();
    emit messageSendRequested(text);
}
