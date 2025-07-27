﻿/*
 * Copyright (C) 2020 ~ 2022 LiuMingHang.
 *
 * Author:     LiuMingHang <liuminghang0821@gmail.com>
 *
 * Maintainer: LiuMingHang <liuminghang0821@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "moresetting.h"
#include "ui_moresetting.h"
#include "application.h"

#include <QTimer>
#include <QDBusInterface>
#include <QDBusPendingCall>
#include <QMessageBox>
#include "ini/inimanager.h"
MoreSetting::MoreSetting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MoreSetting)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Advanced Settings"));

    m_languageMap["en"] = u8"English";
    m_languageMap["zh_CN"] = u8"中文";
    m_languageMap["zh_TW"] = u8"繁體中文(中国台湾)";
    m_languageMap["zh_HK"] = u8"繁體中文(中国香港)";
    m_languageMap["es"] = u8"Español";
    m_languageMap["pl"] = u8"Polski";
    m_languageMap["ja"] = u8"日本語";
    m_languageMap["de"] = u8"Deutsch";
    m_languageMap["ko"] = u8"한국어";
    m_languageMap["it"] = u8"Italiano";
    m_languageMap["fr"] = u8"Français";
    m_languageMap["ru"] = u8"Русский";
    m_languageMap["pt"] = u8"Português";
    m_languageMap["pt_BR"] = u8"Português (Brasil)";
    m_languageMap["fi"] = u8"Suomi";
    m_languageMap["vi"] = u8"Tiếng Việt";
    m_languageMap["tr"] = u8"Türkçe";
    m_languageMap["th"] = u8"ภาษาไทย";
    m_languageMap["hu"] = u8"Magyar";
    m_languageMap["sv"] = u8"Svenska";
    m_languageMap["sk"] = u8"Slovenčina";
    m_languageMap["ro"] = u8"Română";
    m_languageMap["ms"] = u8"Bahasa Melayu";
    m_languageMap["nl"] = u8"Nederlands";
    m_languageMap["et"] = u8"Eesti keel";
    m_languageMap["bg"] = u8"Български";
    m_languageMap["cs"] = u8"Čeština";
    m_languageMap["da"] = u8"Dansk sprog";
    m_languageMap["el"] = u8"Ελληνική γλώσσα";

    setLanguageCombox();
}

MoreSetting::~MoreSetting()
{
    delete ui;
}

void MoreSetting::setData(const MoreSetData &data)
{
    if (data.isAuto == 0) {
        ui->autoBox->setCurrentText(tr("continue playing"));
    } else {
        ui->autoBox->setCurrentText(tr("pause"));
    }
    if (data.fps != 0) {
        ui->fpsbox->setCurrentText(QString::number(data.fps));
    } else {
        ui->fpsbox->setCurrentText(tr("default"));
    }
    if (data.hwdec.contains("gpu")  ||
            data.hwdec.contains("vaapi")  ||
            data.hwdec.contains("vdpau")  ||
            data.hwdec.contains("none")  ||
            data.hwdec.contains("auto")
       ) {
        ui->hwdecEdit->hide();
        ui->hwdecBox->setCurrentText(data.hwdec);
    } else {
        ui->hwdecBox->setCurrentText("other");
        ui->hwdecEdit->setText(data.hwdec);
    }

    if (data.vo == "libmpv"  ||
        data.vo == ""  ||
        data.vo == "opengl"  ||
        data.vo == "opengl-cb"
        ) 
	{
        ui->voEdit->hide();
        ui->voBox->setCurrentText(data.vo);
    } else {
        ui->voBox->setCurrentText("other");
        ui->voEdit->setText(data.vo);
    }


    if (data.isShowDesktopIcon) {
        ui->desktopShowCombox->setCurrentText(tr("true"));
    } else {
        ui->desktopShowCombox->setCurrentText(tr("false"));
    }

    if (data.isTop) {
        ui->topBox->setCurrentText(tr("true"));
    } else {
        ui->topBox->setCurrentText(tr("false"));
    }
    if(data.isEventPenetration)
    {
        ui->eventBox->setCurrentText(tr("true"));
    }
    else
    {
        ui->eventBox->setCurrentText(tr("false"));
    }
    ui->label_name->setText(dApp->m_currentDesktopName);
    ui->desktop_transparency->setValue(dApp->m_moreData.m_DesktopTransparency *100.0);
    ui->back_transparency->setValue(dApp->m_moreData.m_WallpaperTransparency *100.0);

    ui->fontBox->setCurrentText(dApp->m_moreData.fontColor);
    ui->langbox->setCurrentText(m_languageMap.value(dApp->m_moreData.language));

    ui->check_deepin->setChecked(dApp->m_isDDE23);

    int videoPlugin = IniManager::instance()->value("Media/CurrentPlugin",0).toInt();
    ui->comboxVideoPlugin->setCurrentIndex(videoPlugin);

    if (data.theme == "white") {
        ui->themeBox->setCurrentText(tr("white"));
    } else {
        ui->themeBox->setCurrentText(tr("dark"));
    }

    ui->DefaultPath1Edit->setText(data.defaultPath1);
    ui->DefaultPath2Edit->setText(data.defaultPath2);
}

void MoreSetting::setLanguageCombox()
{

    for (const QString &file : m_languageMap) {
        ui->langbox->addItem(file);
    }
}

void MoreSetting::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(event);
}

void MoreSetting::on_okBtn_clicked()
{
    dApp->m_moreData.fps = ui->fpsbox->currentText().toInt();
    if (ui->autoBox->currentText() == tr("continue playing")) {
        dApp->m_moreData.isAuto = 0;
    } else {
        dApp->m_moreData.isAuto = 1;
    }
    //    dApp->setMpvValue("correct-pts", "no");
    dApp->m_moreData.fps = ui->fpsbox->currentText().toInt();
    QString str = ui->fpsbox->currentText();
    if (!str.contains(tr("default"))) {
        dApp->setMpvValue("fps", ui->fpsbox->currentText());
    } else {
        dApp->setMpvValue("fps", "0");
    }

    QString hwdecBoxStr = ui->hwdecBox->currentText();
    if (hwdecBoxStr.contains("other")) {
        dApp->m_moreData.hwdec = ui->hwdecEdit->text();
    } else {
        dApp->m_moreData.hwdec = ui->hwdecBox->currentText();
    }

    QString voBoxStr = ui->voBox->currentText();
    if (voBoxStr.contains("other")) {
        dApp->m_moreData.vo = ui->voEdit->text();
    } else {
        dApp->m_moreData.vo = ui->voBox->currentText();
    }

    QString isDesktopIcon = ui->desktopShowCombox->currentText();
    if (isDesktopIcon.contains(tr("true"))) {
        dApp->m_moreData.isShowDesktopIcon = true;
    } else {
        dApp->m_moreData.isShowDesktopIcon = false;
    }

    QString isTop = ui->topBox->currentText();
    if (isTop.contains(tr("true"))) {
        if(dApp->m_moreData.isTop!= true)
        {
            Q_EMIT dApp->sigWallpaperTopChanged(true);
        }
        dApp->m_moreData.isTop = true;
    } else {
        if(dApp->m_moreData.isTop!= false)
        {
            Q_EMIT dApp->sigWallpaperTopChanged(false);
        }
        dApp->m_moreData.isTop = false;
    }
    QString isEvent = ui->eventBox->currentText();
    if (isEvent.contains(tr("true"))) {
        if(dApp->m_moreData.isEventPenetration!= true)
        {
            Q_EMIT dApp->sigWallpaperEventChanged(true);
        }
        dApp->m_moreData.isEventPenetration = true;
    } else {
        if(dApp->m_moreData.isEventPenetration!= false)
        {
            Q_EMIT dApp->sigWallpaperEventChanged(false);
        }
        dApp->m_moreData.isEventPenetration = false;
    }

    if (dApp->m_moreData.fontColor != ui->fontBox->currentText()) {
        QMessageBox::information(this, tr("Tips"), tr("The text color change will take effect after restarting this software!"));
    }
    dApp->m_moreData.fontColor = ui->fontBox->currentText();
    dApp->m_moreData.language  = m_languageMap.key(ui->langbox->currentText());

    if (dApp->m_moreData.theme != ui->themeBox->currentText()) {
        QMessageBox::information(this, tr("Tips"), tr("The theme change will take effect after restarting this software!"));
    }
    dApp->m_moreData.theme = ui->themeBox->currentText();
    dApp->m_moreData.defaultPath1 = ui->DefaultPath1Edit->text();
    dApp->m_moreData.defaultPath2 = ui->DefaultPath2Edit->text();

    int index = ui->themeBox->currentIndex();
    if (index == 0)  // 第一个选项, dark
        IniManager::instance()->setValue("WallPaper/theme", "dark");
    else if (index == 1)  // 第二个选项, white
        IniManager::instance()->setValue("WallPaper/theme", "white");
    IniManager::instance()->setValue("WallPaper/defaultPath1",ui->DefaultPath1Edit->text());
    IniManager::instance()->setValue("WallPaper/defaultPath2",ui->DefaultPath2Edit->text());

    Q_EMIT dApp->moreSettingSave();

    dApp->setSpecialDesktop();

    IniManager::instance()->setValue("Media/CurrentPlugin",ui->comboxVideoPlugin->currentIndex());
    VideoWidgetType type = VideoWidgetType(ui->comboxVideoPlugin->currentIndex());
    dApp->sigSetMeidaType(type);

    close();
}

void MoreSetting::on_cancelBtn_clicked()
{
    close();
}

void MoreSetting::on_hwdecBox_activated(const QString &arg1)
{
    if (arg1.contains("other")) {
        ui->hwdecEdit->show();
    } else {
        ui->hwdecEdit->hide();
    }
}


void MoreSetting::on_desktop_transparency_valueChanged(int value)
{
    double dop = (double)value/100.0;
    dApp->changePidOpacity(dop);
    dApp->m_moreData.m_DesktopTransparency = dop;
    IniManager::instance()->setValue("WallPaper/DesktopTransparency",QString::number(dop));

}

void MoreSetting::on_back_transparency_valueChanged(int value)
{
    double dop = (double)value/100.0;
    dApp->changeMeOpacity(dop);
    dApp->m_moreData.m_WallpaperTransparency = dop;
    IniManager::instance()->setValue("WallPaper/WallpaperTransparency",QString::number(dop));
}

void MoreSetting::on_voBox_currentTextChanged(const QString &arg1)
{
    if (arg1.contains("other")) {
        ui->voEdit->show();
    } else {
        ui->voEdit->hide();
    }
}


void MoreSetting::on_check_deepin_stateChanged(int arg1)
{
    dApp->m_isDDE23 = ui->check_deepin->isChecked();
}


void MoreSetting::on_setWorkPaper_clicked()
{
    QDBusInterface iface("com.deepin.daemon.Appearance",
                         "/com/deepin/daemon/Appearance",
                         "com.deepin.daemon.Appearance",
                         QDBusConnection::sessionBus());
    if(QFile::exists("/usr/share/fantascene-dynamic-wallpaper/normal/touming.png"))
    {
        iface.asyncCall("SetCurrentWorkspaceBackground", "/usr/share/fantascene-dynamic-wallpaper/normal/touming.png");
    }
    else if(QFile::exists("/usr/share/fantascene-dynamic-wallpaper/normal/deepin/touming.png"))
    {
        iface.asyncCall("SetCurrentWorkspaceBackground", "/usr/share/fantascene-dynamic-wallpaper/normal/deepin/touming.png");
    }

}


void MoreSetting::on_setWorkPaper_2_clicked()
{
    QDBusInterface iface("com.deepin.daemon.Appearance",
                         "/com/deepin/daemon/Appearance",
                         "com.deepin.daemon.Appearance",
                         QDBusConnection::sessionBus());
    iface.asyncCall("SetCurrentWorkspaceBackground", "");
}


void MoreSetting::on_comboxVideoPlugin_currentIndexChanged(int index)
{

    switch (index) {
    case 0:
        ui->label_FPS->setVisible(true);
        ui->label_Decoder->setVisible(true);
        ui->label_vo->setVisible(true);

        ui->hwdecBox->setVisible(true);
        ui->fpsbox->setVisible(true);
        ui->voBox->setVisible(true);
        if(ui->hwdecBox->currentText() == "other")
        {
            ui->hwdecEdit->setVisible(true);
        }
        if(ui->voBox->currentText() == "other")
        {
            ui->voEdit->setVisible(true);
        }

        break;
    case 1:
        ui->label_FPS->setVisible(false);
        ui->label_Decoder->setVisible(false);
        ui->label_vo->setVisible(false);

        ui->hwdecBox->setVisible(false);
        ui->fpsbox->setVisible(false);
        ui->voBox->setVisible(false);

        ui->hwdecEdit->setVisible(false);
        ui->voEdit->setVisible(false);
        break;
    default:
        break;
    }
}

