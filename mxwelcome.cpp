/**********************************************************************
 *  mxwelcome.cpp
 **********************************************************************
 * Copyright (C) 2015 MX Authors
 *
 * Authors: Adrian
 *          MX & MEPIS Community <http://forum.mepiscommunity.org>
 *
 * This file is part of mx-welcome.
 *
 * mx-welcome is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * mx-welcome is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with mx-welcome.  If not, see <http://www.gnu.org/licenses/>.
 **********************************************************************/


#include "mxwelcome.h"
#include "ui_mxwelcome.h"

#include <QFileDialog>
#include <QDebug>

mxwelcome::mxwelcome(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::mxwelcome)
{
    ui->setupUi(this);
    setup();
}

mxwelcome::~mxwelcome()
{
    delete ui;
}

// setup versious items first time program runs
void mxwelcome::setup()
{
    version = getVersion("mx-welcome");
    this->setWindowTitle(tr("MX Welcome"));    
    ui->stackedWidget->setCurrentIndex(0);
    ui->buttonCancel->setEnabled(true);
}

// Util function for getting bash command output and error code
Result mxwelcome::runCmd(QString cmd)
{
    QEventLoop loop;
    proc = new QProcess(this);
    proc->setReadChannelMode(QProcess::MergedChannels);
    connect(proc, SIGNAL(finished(int)), &loop, SLOT(quit()));
    proc->start("/bin/bash", QStringList() << "-c" << cmd);
    loop.exec();
    disconnectAll();
    Result result = {proc->exitCode(), proc->readAll().trimmed()};
    delete proc;
    return result;
}

// disconnect all connections
void mxwelcome::disconnectAll()
{
    disconnect(proc, SIGNAL(started()), 0, 0);
    disconnect(proc, SIGNAL(finished(int)), 0, 0);
}

// Get version of the program
QString mxwelcome::getVersion(QString name)
{
    QString cmd = QString("dpkg -l %1 | awk 'NR==6 {print $3}'").arg(name);
    return runCmd(cmd).output;
}

// set proc and timer connections
void mxwelcome::setConnections()
{
    connect(proc, SIGNAL(started()), SLOT(procStart()));
    connect(proc, SIGNAL(finished(int)), SLOT(procDone(int)));
}

//// slots ////

// About button clicked
void mxwelcome::on_buttonAbout_clicked()
{
    QMessageBox msgBox(QMessageBox::NoIcon,
                       tr("About MX Welcome"), "<p align=\"center\"><b><h2>" +
                       tr("MX Welcome") + "</h2></b></p><p align=\"center\">" + tr("Version: ") + version + "</p><p align=\"center\"><h3>" +
                       tr("Program for displaying a welcome screen in MX Linux") +
                       "</h3></p><p align=\"center\"><a href=\"http://www.mepiscommunity.org/mx\">http://www.mepiscommunity.org/mx</a><br /></p><p align=\"center\">" +
                       tr("Copyright (c) MX Linux") + "<br /><br /></p>", 0, this);
    msgBox.addButton(tr("Cancel"), QMessageBox::AcceptRole); // because we want to display the buttons in reverse order we use counter-intuitive roles.
    msgBox.addButton(tr("License"), QMessageBox::RejectRole);
    if (msgBox.exec() == QMessageBox::RejectRole) {
        QString cmd = QString("mx-viewer http://www.mepiscommunity.org/doc_mx/mx-welcome-license.html '%1'").arg(tr("MX Welcome"));
        system(cmd.toUtf8());
    }
}