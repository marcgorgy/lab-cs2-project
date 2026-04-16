#include "SettingsScreen.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QIntValidator>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QScrollArea>

SettingsScreen::SettingsScreen(QWidget *parent)
    : QWidget(parent)
{
    buildUi();
    applyStyles();
}

void SettingsScreen::buildUi()
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ── Top bar ──────────────────────────────────────────────────────────
    auto *topBar = new QFrame(this);
    topBar->setObjectName("settingsTopBar");
    topBar->setFixedHeight(56);

    auto *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(24, 0, 24, 0);

    auto *titleLabel = new QLabel("⚙  Settings", topBar);
    titleLabel->setObjectName("settingsTitle");

    topLayout->addWidget(titleLabel);
    topLayout->addStretch();

    root->addWidget(topBar);

    // ── Scrollable content ───────────────────────────────────────────────
    auto *scroll = new QScrollArea(this);
    scroll->setObjectName("settingsScroll");
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);

    auto *content = new QWidget(scroll);
    auto *contentLayout = new QVBoxLayout(content);
    contentLayout->setContentsMargins(40, 32, 40, 32);
    contentLayout->setSpacing(24);

    auto makeGroup = [&](const QString &title) -> QGroupBox* {
        auto *grp = new QGroupBox(title, content);
        grp->setObjectName("settingsGroup");
        return grp;
    };

    // ── Group: Connection ────────────────────────────────────────────────
    auto *connGroup = makeGroup("Connection");
    auto *connForm  = new QFormLayout(connGroup);
    connForm->setSpacing(12);
    connForm->setLabelAlignment(Qt::AlignRight);

    m_usernameEdit = new QLineEdit(connGroup);
    m_usernameEdit->setObjectName("settingsInput");
    m_usernameEdit->setMaxLength(32);
    connForm->addRow("Username", m_usernameEdit);

    m_hostEdit = new QLineEdit(connGroup);
    m_hostEdit->setObjectName("settingsInput");
    connForm->addRow("Host", m_hostEdit);

    m_portEdit = new QLineEdit(connGroup);
    m_portEdit->setObjectName("settingsInput");
    m_portEdit->setValidator(new QIntValidator(1, 65535, this));
    connForm->addRow("Port", m_portEdit);

    contentLayout->addWidget(connGroup);

    // ── Group: Preferences ───────────────────────────────────────────────
    auto *prefGroup = makeGroup("Preferences");
    auto *prefLayout = new QVBoxLayout(prefGroup);
    prefLayout->setSpacing(12);

    m_timestampsCheck = new QCheckBox("Show message timestamps", prefGroup);
    m_timestampsCheck->setObjectName("settingsCheck");
    m_timestampsCheck->setChecked(true);

    m_soundCheck = new QCheckBox("Enable notification sounds", prefGroup);
    m_soundCheck->setObjectName("settingsCheck");
    m_soundCheck->setChecked(true);

    auto *themeRow = new QHBoxLayout();
    auto *themeLabel = new QLabel("Theme:", prefGroup);
    themeLabel->setObjectName("prefLabel");
    m_themeCombo = new QComboBox(prefGroup);
    m_themeCombo->setObjectName("settingsCombo");
    m_themeCombo->addItems({"Dark (default)", "Light", "High Contrast"});
    themeRow->addWidget(themeLabel);
    themeRow->addWidget(m_themeCombo);
    themeRow->addStretch();

    prefLayout->addWidget(m_timestampsCheck);
    prefLayout->addWidget(m_soundCheck);
    prefLayout->addLayout(themeRow);

    contentLayout->addWidget(prefGroup);

    // ── Error label ───────────────────────────────────────────────────────
    m_errorLabel = new QLabel(content);
    m_errorLabel->setObjectName("settingsError");
    m_errorLabel->setWordWrap(true);
    m_errorLabel->hide();
    contentLayout->addWidget(m_errorLabel);

    contentLayout->addStretch();

    scroll->setWidget(content);
    root->addWidget(scroll, 1);

    // ── Bottom bar ────────────────────────────────────────────────────────
    auto *bottomBar = new QFrame(this);
    bottomBar->setObjectName("settingsBottomBar");
    bottomBar->setFixedHeight(64);

    auto *bottomLayout = new QHBoxLayout(bottomBar);
    bottomLayout->setContentsMargins(40, 0, 40, 0);
    bottomLayout->setSpacing(12);
    bottomLayout->addStretch();

    m_cancelBtn = new QPushButton("Cancel", bottomBar);
    m_cancelBtn->setObjectName("cancelBtn");
    m_cancelBtn->setFixedSize(100, 38);
    m_cancelBtn->setCursor(Qt::PointingHandCursor);

    m_saveBtn = new QPushButton("Save", bottomBar);
    m_saveBtn->setObjectName("saveBtn");
    m_saveBtn->setFixedSize(100, 38);
    m_saveBtn->setCursor(Qt::PointingHandCursor);

    bottomLayout->addWidget(m_cancelBtn);
    bottomLayout->addWidget(m_saveBtn);

    root->addWidget(bottomBar);

    connect(m_saveBtn,   &QPushButton::clicked, this, &SettingsScreen::onSaveClicked);
    connect(m_cancelBtn, &QPushButton::clicked, this, &SettingsScreen::cancelled);
}

void SettingsScreen::applyStyles()
{
    setStyleSheet(R"(
        SettingsScreen { background: #1a1a2e; }

        #settingsTopBar {
            background: #0f3460;
            border-bottom: 1px solid rgba(255,255,255,0.08);
        }
        #settingsTitle {
            font-size: 16px;
            font-weight: 700;
            color: #ffffff;
        }

        #settingsScroll { background: #1a1a2e; }

        #settingsGroup {
            color: rgba(255,255,255,0.5);
            font-size: 11px;
            font-weight: 700;
            letter-spacing: 1px;
            border: 1px solid rgba(255,255,255,0.08);
            border-radius: 10px;
            margin-top: 8px;
            padding: 12px;
        }
        #settingsGroup::title {
            subcontrol-origin: margin;
            left: 12px;
            padding: 0 6px;
        }

        QFormLayout QLabel, #prefLabel {
            color: rgba(255,255,255,0.7);
            font-size: 13px;
        }

        #settingsInput {
            background: rgba(255,255,255,0.06);
            border: 1px solid rgba(255,255,255,0.12);
            border-radius: 7px;
            color: #ffffff;
            padding: 7px 12px;
            font-size: 13px;
        }
        #settingsInput:focus {
            border: 1px solid #e94560;
        }

        #settingsCheck {
            color: rgba(255,255,255,0.8);
            font-size: 13px;
        }
        #settingsCheck::indicator {
            width: 16px; height: 16px;
            border: 1px solid rgba(255,255,255,0.25);
            border-radius: 4px;
            background: rgba(255,255,255,0.06);
        }
        #settingsCheck::indicator:checked {
            background: #e94560;
            border-color: #e94560;
        }

        #settingsCombo {
            background: rgba(255,255,255,0.06);
            border: 1px solid rgba(255,255,255,0.12);
            border-radius: 7px;
            color: #ffffff;
            padding: 5px 10px;
            font-size: 13px;
            min-width: 160px;
        }

        #settingsBottomBar {
            background: #0f3460;
            border-top: 1px solid rgba(255,255,255,0.08);
        }

        #cancelBtn {
            background: rgba(255,255,255,0.07);
            color: rgba(255,255,255,0.7);
            border: 1px solid rgba(255,255,255,0.12);
            border-radius: 7px;
            font-size: 13px;
        }
        #cancelBtn:hover { background: rgba(255,255,255,0.12); }

        #saveBtn {
            background: #e94560;
            color: white;
            border: none;
            border-radius: 7px;
            font-size: 13px;
            font-weight: 600;
        }
        #saveBtn:hover   { background: #c73652; }
        #saveBtn:pressed { background: #a52d44; }

        #settingsError {
            color: #ff6b6b;
            font-size: 12px;
            background: rgba(255,107,107,0.1);
            border: 1px solid rgba(255,107,107,0.3);
            border-radius: 6px;
            padding: 8px 12px;
        }

        QScrollBar:vertical { background: transparent; width: 6px; }
        QScrollBar::handle:vertical {
            background: rgba(255,255,255,0.15); border-radius: 3px;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
    )");
}

// ── Public interface ──────────────────────────────────────────────────────────

void SettingsScreen::populate(const QString &username, const QString &host, int port)
{
    m_usernameEdit->setText(username);
    m_hostEdit->setText(host);
    m_portEdit->setText(QString::number(port));
    m_errorLabel->hide();
}

// ── Private slots ─────────────────────────────────────────────────────────────

void SettingsScreen::onSaveClicked()
{
    if (!validateInputs()) return;

    emit settingsSaved(
        m_usernameEdit->text().trimmed(),
        m_hostEdit->text().trimmed(),
        m_portEdit->text().toInt()
    );
}

bool SettingsScreen::validateInputs()
{
    if (m_usernameEdit->text().trimmed().isEmpty()) {
        m_errorLabel->setText("⚠  Username cannot be empty.");
        m_errorLabel->show();
        m_usernameEdit->setFocus();
        return false;
    }
    if (m_hostEdit->text().trimmed().isEmpty()) {
        m_errorLabel->setText("⚠  Host cannot be empty.");
        m_errorLabel->show();
        m_hostEdit->setFocus();
        return false;
    }
    if (m_portEdit->text().trimmed().isEmpty()) {
        m_errorLabel->setText("⚠  Port cannot be empty.");
        m_errorLabel->show();
        m_portEdit->setFocus();
        return false;
    }
    m_errorLabel->hide();
    return true;
}
