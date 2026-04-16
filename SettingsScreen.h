#pragma once

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QPushButton;
class QLabel;
class QComboBox;
class QCheckBox;
QT_END_NAMESPACE

/**
 * SettingsScreen – lets the user adjust connection parameters and preferences.
 * NO business/network logic lives here.
 */
class SettingsScreen : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsScreen(QWidget *parent = nullptr);

    /** Pre-fill fields before showing the screen */
    void populate(const QString &username, const QString &host, int port);

signals:
    /** Emitted when user clicks Save; caller reconnects if needed */
    void settingsSaved(const QString &username, const QString &host, int port);
    void cancelled();

private slots:
    void onSaveClicked();

private:
    void buildUi();
    void applyStyles();
    bool validateInputs();

    QLineEdit   *m_usernameEdit;
    QLineEdit   *m_hostEdit;
    QLineEdit   *m_portEdit;
    QCheckBox   *m_timestampsCheck;
    QCheckBox   *m_soundCheck;
    QComboBox   *m_themeCombo;
    QLabel      *m_errorLabel;
    QPushButton *m_saveBtn;
    QPushButton *m_cancelBtn;
};
