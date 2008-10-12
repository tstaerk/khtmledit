/***************************************************************************
 *   Copyright (C) 2007 by Thorsten Staerk                                 *
 *   kde staerk de                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef KHTMLEDIT_H
#define KHTMLEDIT_H
#define VERSION "0.1"

#include <kdirwatch.h>
#include <kmainwindow.h>
#include <QThread>

class Ui_khtmledit;

class khtmledit:public QMainWindow
{
  Q_OBJECT
  public:
    khtmledit(char *argv=0);
    QString load(QString filename="");
    QString filterresult(QString);
    QString content; // what has to be filtered
    QString mfilename;
    Ui_khtmledit *khtmleditui; // holds the UI, e.g. ktextedit and ktextedit_2
    KDirWatch dw;
    bool slotplaintextmodifiedhtml;

  public slots:
    void slotbuttonsaveclicked();
    void slotbuttonsaveasclicked();
    void slotbuttonfatclicked();
    void slotbuttonh1clicked();
    void slotbuttonh2clicked();
    void slotbuttonh3clicked();
    void slotbuttonh4clicked();
    void slotbuttonh5clicked();
    void slotbuttonh6clicked();
    void slotbuttonitalicclicked();
    void slotbuttonunderlinedclicked();
    void slotfilechanged();
    void slothtmltextchanged();
    void slotplaintextchanged();
    void slotactionload();
    void slotquit();

  private:
    bool dirty; // has any text changed since last save ?
};

#endif


