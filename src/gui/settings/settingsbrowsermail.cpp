// This file is part of RSS Guard.
//
// Copyright (C) 2011-2016 by Martin Rotter <rotter.martinos@gmail.com>
//
// RSS Guard is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// RSS Guard is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with RSS Guard. If not, see <http://www.gnu.org/licenses/>.

#include "gui/settings/settingsbrowsermail.h"

#include "network-web/silentnetworkaccessmanager.h"
#include "miscellaneous/application.h"
#include "miscellaneous/textfactory.h"

#include <QNetworkProxy>
#include <QFileDialog>


SettingsBrowserMail::SettingsBrowserMail(Settings *settings, QWidget *parent)
  : SettingsPanel(settings, parent), m_ui(new Ui::SettingsBrowserMail) {
  m_ui->setupUi(this);

  connect(m_ui->m_cmbProxyType, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SettingsBrowserMail::onProxyTypeChanged);
  connect(m_ui->m_checkShowPassword, &QCheckBox::stateChanged, this, &SettingsBrowserMail::displayProxyPassword);
  connect(m_ui->m_cmbExternalBrowserPreset, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SettingsBrowserMail::changeDefaultBrowserArguments);
  connect(m_ui->m_btnExternalBrowserExecutable, &QPushButton::clicked, this, &SettingsBrowserMail::selectBrowserExecutable);
  connect(m_ui->m_cmbExternalEmailPreset, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SettingsBrowserMail::changeDefaultEmailArguments);
  connect(m_ui->m_btnExternalEmailExecutable, &QPushButton::clicked, this, &SettingsBrowserMail::selectEmailExecutable);
}

SettingsBrowserMail::~SettingsBrowserMail() {
  delete m_ui;
}

void SettingsBrowserMail::changeDefaultBrowserArguments(int index) {
  if (index != 0) {
    m_ui->m_txtExternalBrowserArguments->setText(m_ui->m_cmbExternalBrowserPreset->itemData(index).toString());
  }
}

void SettingsBrowserMail::selectBrowserExecutable() {
  const QString executable_file = QFileDialog::getOpenFileName(this,
                                                               tr("Select web browser executable"),
                                                               qApp->homeFolderPath(),
                                                               //: File filter for external browser selection dialog.
                                                             #if defined(Q_OS_LINUX)
                                                               tr("Executables (*)")
                                                             #else
                                                               tr("Executables (*.*)")
                                                             #endif
                                                               );

  if (!executable_file.isEmpty()) {
    m_ui->m_txtExternalBrowserExecutable->setText(QDir::toNativeSeparators(executable_file));
  }
}

void SettingsBrowserMail::displayProxyPassword(int state) {
  if (state == Qt::Checked) {
    m_ui->m_txtProxyPassword->setEchoMode(QLineEdit::Normal);
  }
  else {
    m_ui->m_txtProxyPassword->setEchoMode(QLineEdit::PasswordEchoOnEdit);
  }
}

void SettingsBrowserMail::onProxyTypeChanged(int index) {
  const QNetworkProxy::ProxyType selected_type = static_cast<QNetworkProxy::ProxyType>(m_ui->m_cmbProxyType->itemData(index).toInt());
  const bool is_proxy_selected = selected_type != QNetworkProxy::NoProxy && selected_type != QNetworkProxy::DefaultProxy;

  m_ui->m_txtProxyHost->setEnabled(is_proxy_selected);
  m_ui->m_txtProxyPassword->setEnabled(is_proxy_selected);
  m_ui->m_txtProxyUsername->setEnabled(is_proxy_selected);
  m_ui->m_spinProxyPort->setEnabled(is_proxy_selected);
  m_ui->m_checkShowPassword->setEnabled(is_proxy_selected);
  m_ui->m_lblProxyHost->setEnabled(is_proxy_selected);
  m_ui->m_lblProxyInfo->setEnabled(is_proxy_selected);
  m_ui->m_lblProxyPassword->setEnabled(is_proxy_selected);
  m_ui->m_lblProxyPort->setEnabled(is_proxy_selected);
  m_ui->m_lblProxyUsername->setEnabled(is_proxy_selected);
}

void SettingsBrowserMail::changeDefaultEmailArguments(int index) {
  if (index != 0) {
    m_ui->m_txtExternalEmailArguments->setText(m_ui->m_cmbExternalEmailPreset->itemData(index).toString());
  }
}

void SettingsBrowserMail::selectEmailExecutable() {
  QString executable_file = QFileDialog::getOpenFileName(this,
                                                         tr("Select e-mail executable"),
                                                         qApp->homeFolderPath(),
                                                         //: File filter for external e-mail selection dialog.
                                                       #if defined(Q_OS_LINUX)
                                                         tr("Executables (*)")
                                                       #else
                                                         tr("Executables (*.*)")
                                                       #endif
                                                         );

  if (!executable_file.isEmpty()) {
    m_ui->m_txtExternalEmailExecutable->setText(QDir::toNativeSeparators(executable_file));
  }
}

void SettingsBrowserMail::loadSettings() {
  // Load settings of web browser GUI.
  m_ui->m_cmbExternalBrowserPreset->addItem(tr("Opera 12 or older"), QSL("-nosession %1"));
  m_ui->m_txtExternalBrowserExecutable->setText(settings()->value(GROUP(Browser), SETTING(Browser::CustomExternalBrowserExecutable)).toString());
  m_ui->m_txtExternalBrowserArguments->setText(settings()->value(GROUP(Browser), SETTING(Browser::CustomExternalBrowserArguments)).toString());
  m_ui->m_grpCustomExternalBrowser->setChecked(settings()->value(GROUP(Browser), SETTING(Browser::CustomExternalBrowserEnabled)).toBool());

  // Load settings of e-mail.
  m_ui->m_cmbExternalEmailPreset->addItem(tr("Mozilla Thunderbird"), QSL("-compose \"subject='%1',body='%2'\""));
  m_ui->m_txtExternalEmailExecutable->setText(settings()->value(GROUP(Browser), SETTING(Browser::CustomExternalEmailExecutable)).toString());
  m_ui->m_txtExternalEmailArguments->setText(settings()->value(GROUP(Browser), SETTING(Browser::CustomExternalEmailArguments)).toString());
  m_ui->m_grpCustomExternalEmail->setChecked(settings()->value(GROUP(Browser), SETTING(Browser::CustomExternalEmailEnabled)).toBool());

  m_ui->m_cmbProxyType->addItem(tr("No proxy"), QNetworkProxy::NoProxy);
  m_ui->m_cmbProxyType->addItem(tr("System proxy"), QNetworkProxy::DefaultProxy);
  m_ui->m_cmbProxyType->addItem(tr("Socks5"), QNetworkProxy::Socks5Proxy);
  m_ui->m_cmbProxyType->addItem(tr("Http"), QNetworkProxy::HttpProxy);

  // Load the settings.
  QNetworkProxy::ProxyType selected_proxy_type = static_cast<QNetworkProxy::ProxyType>(settings()->value(GROUP(Proxy), SETTING(Proxy::Type)).toInt());

  m_ui->m_cmbProxyType->setCurrentIndex(m_ui->m_cmbProxyType->findData(selected_proxy_type));
  m_ui->m_txtProxyHost->setText(settings()->value(GROUP(Proxy), SETTING(Proxy::Host)).toString());
  m_ui->m_txtProxyUsername->setText(settings()->value(GROUP(Proxy), SETTING(Proxy::Username)).toString());
  m_ui->m_txtProxyPassword->setText(TextFactory::decrypt(settings()->value(GROUP(Proxy), SETTING(Proxy::Password)).toString()));
  m_ui->m_spinProxyPort->setValue(settings()->value(GROUP(Proxy), SETTING(Proxy::Port)).toInt());


}

void SettingsBrowserMail::saveSettings() {
  // Save settings of GUI of web browser.
  settings()->setValue(GROUP(Browser), Browser::CustomExternalBrowserEnabled, m_ui->m_grpCustomExternalBrowser->isChecked());
  settings()->setValue(GROUP(Browser), Browser::CustomExternalBrowserExecutable, m_ui->m_txtExternalBrowserExecutable->text());
  settings()->setValue(GROUP(Browser), Browser::CustomExternalBrowserArguments, m_ui->m_txtExternalBrowserArguments->text());

  // Save settings of e-mail.
  settings()->setValue(GROUP(Browser), Browser::CustomExternalEmailExecutable, m_ui->m_txtExternalEmailExecutable->text());
  settings()->setValue(GROUP(Browser), Browser::CustomExternalEmailArguments, m_ui->m_txtExternalEmailArguments->text());
  settings()->setValue(GROUP(Browser), Browser::CustomExternalEmailEnabled, m_ui->m_grpCustomExternalEmail->isChecked());

  settings()->setValue(GROUP(Proxy), Proxy::Type, m_ui->m_cmbProxyType->itemData(m_ui->m_cmbProxyType->currentIndex()));
  settings()->setValue(GROUP(Proxy), Proxy::Host, m_ui->m_txtProxyHost->text());
  settings()->setValue(GROUP(Proxy), Proxy::Username,  m_ui->m_txtProxyUsername->text());
  settings()->setValue(GROUP(Proxy), Proxy::Password, TextFactory::encrypt(m_ui->m_txtProxyPassword->text()));
  settings()->setValue(GROUP(Proxy), Proxy::Port, m_ui->m_spinProxyPort->value());

  // Reload settings for all network access managers.
  SilentNetworkAccessManager::instance()->loadSettings();
}