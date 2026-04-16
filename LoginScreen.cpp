#include "LoginScreen.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QIntValidator>
#include <QFrame>
#include <QFont>
#include <QGraphicsDropShadowEffect>

LoginScreen::LoginScreen(QWidget *parent)
    : QWidget(parent)
{
    buildUi();
    applyStyles();
}

void LoginScreen::buildUi()
{
    // ── Outer layout ─────────────────────────────────────────────
    auto *outerLayout = new QVBoxLayout(this);
    outerLayout->setAlignment(Qt::AlignCenter);

    // ── Card frame ───────────────────────────────────────────────
    auto *card = new QFrame(this);
    card->setObjectName("loginCard");
    card->setFixedWidth(400);

    auto *shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(32);
    shadow->setOffset(0, 8);
    shadow->setColor(QColor(0, 0, 0, 80));
    card->setGraphicsEffect(shadow);

    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(40, 40, 40, 40);
    cardLayout->setSpacing(20);

    // Title
    auto *title = new QLabel("QtChat", card);
    title->setObjectName("loginTitle");
    title->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(title);

    auto *subtitle = new QLabel("Connect to a server to start chatting", card);
    subtitle->setObjectName("loginSubtitle");
    subtitle->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(subtitle);

    cardLayout->addSpacing(8);

    // Form
    auto *form = new QFormLayout();
    form->setSpacing(12);
    form->setLabelAlignment(Qt::AlignRight);

    m_usernameEdit = new QLineEdit(card);
    m_usernameEdit->setObjectName("formInput");
    m_usernameEdit->setPlaceholderText("e.g. alice");
    m_usernameEdit->setMaxLength(32);
    form->addRow("Username", m_usernameEdit);

    m_hostEdit = new QLineEdit(card);
    m_hostEdit->setObjectName("formInput");
    m_hostEdit->setPlaceholderText("e.g. 127.0.0.1");
    m_hostEdit->setText("127.0.0.1");
    form->addRow("Host", m_hostEdit);

    m_portEdit = new QLineEdit(card);
    m_portEdit->setObjectName("formInput");
    m_portEdit->setPlaceholderText("e.g. 12345");
    m_portEdit->setText("12345");
    m_portEdit->setValidator(new QIntValidator(1, 65535, this));
    form->addRow("Port", m_portEdit);

    cardLayout->addLayout(form);

    // Error label (hidden by default)
    m_errorLabel = new QLabel(card);
    m_errorLabel->setObjectName("errorLabel");
    m_errorLabel->setWordWrap(true);
    m_errorLabel->setAlignment(Qt::AlignCenter);
    m_errorLabel->hide();
    cardLayout->addWidget(m_errorLabel);

    // Progress bar (hidden by default)
    m_progressBar = new QProgressBar(card);
    m_progressBar->setObjectName("connectProgress");
    m_progressBar->setRange(0, 0); // indeterminate
    m_progressBar->setFixedHeight(4);
    m_progressBar->setTextVisible(false);
    m_progressBar->hide();
    cardLayout->addWidget(m_progressBar);

    // Connect button
    m_connectBtn = new QPushButton("Connect", card);
    m_connectBtn->setObjectName("primaryBtn");
    m_connectBtn->setFixedHeight(44);
    m_connectBtn->setCursor(Qt::PointingHandCursor);
    cardLayout->addWidget(m_connectBtn);

    outerLayout->addWidget(card);

    // Allow Enter key
    connect(m_usernameEdit, &QLineEdit::returnPressed, this, &LoginScreen::onConnectClicked);
    connect(m_hostEdit,     &QLineEdit::returnPressed, this, &LoginScreen::onConnectClicked);
    connect(m_portEdit,     &QLineEdit::returnPressed, this, &LoginScreen::onConnectClicked);
    connect(m_connectBtn,   &QPushButton::clicked,     this, &LoginScreen::onConnectClicked);
}

void LoginScreen::applyStyles()
{
    setStyleSheet(R"(
        LoginScreen {
            background: qlineargradient(x1:0,y1:0,x2:1,y2:1,
                stop:0 #1a1a2e, stop:1 #16213e);
        }

        #loginCard {
            background: #0f3460;
            border-radius: 16px;
            border: 1px solid rgba(255,255,255,0.08);
        }

        #loginTitle {
            font-size: 28px;
            font-weight: 700;
            color: #e94560;
            letter-spacing: 2px;
        }

        #loginSubtitle {
            font-size: 12px;
            color: rgba(255,255,255,0.5);
        }

        QFormLayout QLabel {
            color: rgba(255,255,255,0.75);
            font-size: 13px;
        }

        #formInput {
            background: rgba(255,255,255,0.06);
            border: 1px solid rgba(255,255,255,0.15);
            border-radius: 8px;
            color: #ffffff;
            padding: 8px 12px;
            font-size: 13px;
        }

        #formInput:focus {
            border: 1px solid #e94560;
            background: rgba(233,69,96,0.08);
        }

        #primaryBtn {
            background: #e94560;
            color: white;
            border: none;
            border-radius: 8px;
            font-size: 14px;
            font-weight: 600;
            letter-spacing: 1px;
        }

        #primaryBtn:hover   { background: #c73652; }
        #primaryBtn:pressed { background: #a52d44; }
        #primaryBtn:disabled {
            background: rgba(233,69,96,0.4);
            color: rgba(255,255,255,0.4);
        }

        #errorLabel {
            color: #ff6b6b;
            font-size: 12px;
            background: rgba(255,107,107,0.1);
            border: 1px solid rgba(255,107,107,0.3);
            border-radius: 6px;
            padding: 6px 10px;
        }

        #connectProgress {
            background: rgba(255,255,255,0.1);
            border: none;
            border-radius: 2px;
        }
        #connectProgress::chunk {
            background: #e94560;
            border-radius: 2px;
        }
    )");
}

// ── Public interface ──────────────────────────────────────────────────────────

void LoginScreen::reset()
{
    m_usernameEdit->clear();
    m_errorLabel->hide();
    m_progressBar->hide();
    m_connectBtn->setEnabled(true);
    m_connectBtn->setText("Connect");
}

void LoginScreen::setConnecting(bool connecting)
{
    m_connectBtn->setEnabled(!connecting);
    m_connectBtn->setText(connecting ? "Connecting…" : "Connect");
    m_progressBar->setVisible(connecting);
    if (connecting) m_errorLabel->hide();
}

void LoginScreen::setErrorMessage(const QString &msg)
{
    m_errorLabel->setText(msg);
    m_errorLabel->show();
    setConnecting(false);
}

// ── Private slots ─────────────────────────────────────────────────────────────

void LoginScreen::onConnectClicked()
{
    if (!validateInputs()) return;

    const QString username = m_usernameEdit->text().trimmed();
    const QString host     = m_hostEdit->text().trimmed();
    const int     port     = m_portEdit->text().toInt();

    emit loginRequested(username, host, port);
}

bool LoginScreen::validateInputs()
{
    if (m_usernameEdit->text().trimmed().isEmpty()) {
        setErrorMessage("⚠  Username cannot be empty.");
        m_usernameEdit->setFocus();
        return false;
    }
    if (m_hostEdit->text().trimmed().isEmpty()) {
        setErrorMessage("⚠  Host cannot be empty.");
        m_hostEdit->setFocus();
        return false;
    }
    if (m_portEdit->text().trimmed().isEmpty()) {
        setErrorMessage("⚠  Port cannot be empty.");
        m_portEdit->setFocus();
        return false;
    }

    m_errorLabel->hide();
    return true;
}
