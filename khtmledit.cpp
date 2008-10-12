/***************************************************************************
 *   Copyright (C) 2008 by Thorsten Staerk                                 *
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
*/

#include <KApplication>
#include <KFileDialog>
#include <KMessageBox>
#include <QDBusConnection>
#include <QFile>
#include <QRegExp>
#include <QTextStream>
#include "kdebug.h"
#include "khtmledit.h"
#include "khtmledit.moc"
#include <kio/netaccess.h>
#include "ktemporaryfile.h"
#include <KUrl>
#include "ui_khtmledit.h"
#include <iostream>
#include <vector>

using namespace std;

QString khtmledit::load(QString filename)
{
  kDebug() << "Entering function";
  QString err=QString();
  content=QString();
  if (filename!="")
  {
    KUrl url(filename);
    kDebug() << url.url();
    if ((url.isLocalFile()) || (!url.url().contains("/")))
    { // load locally
      kDebug() << "Using file " << filename;
      static QFile file(filename);
      static QTextStream stream(&file);
      if (file.open(QIODevice::ReadOnly))
        while (!stream.atEnd()) content.append(stream.readLine()).append("\n");
      file.close();
    }
    else // load remotely
    {
      KTemporaryFile tmpFile;
      if ( !tmpFile.open() ) err = QString::fromLatin1( "Unable to get temporary file" );
      else
      {
        QString test2;
        QString& test=test2;
        if (!KIO::NetAccess::download( url, test, 0 )) err=QString::fromLatin1("Could not download");
        filename=test;
        kDebug() << "Using file " << filename;
        static QFile file(filename);
        static QTextStream stream(&file);
        if (file.open(QIODevice::ReadOnly))
          while (!stream.atEnd()) content.append(stream.readLine()).append("\n");
        file.close();
      }
    }
  }
  kDebug() << "Leaving function";
  return err;
}

khtmledit::khtmledit(char *filename):QMainWindow(NULL)
{
  kDebug() << "Entering function";
  mfilename=filename;
  slotplaintextmodifiedhtml=false;
  khtmleditui = new Ui_khtmledit();
  khtmleditui->setupUi(this);
  load(filename);
  connect(khtmleditui->pushButton_save, SIGNAL(clicked()), SLOT(slotbuttonsaveclicked()));
  connect(khtmleditui->pushButton_bold, SIGNAL(clicked()), SLOT(slotbuttonfatclicked()));
  connect(khtmleditui->pushButton_h1, SIGNAL(clicked()), SLOT(slotbuttonh1clicked()));
  connect(khtmleditui->pushButton_h2, SIGNAL(clicked()), SLOT(slotbuttonh2clicked()));
  connect(khtmleditui->pushButton_h3, SIGNAL(clicked()), SLOT(slotbuttonh3clicked()));
  connect(khtmleditui->pushButton_h4, SIGNAL(clicked()), SLOT(slotbuttonh4clicked()));
  connect(khtmleditui->pushButton_h5, SIGNAL(clicked()), SLOT(slotbuttonh5clicked()));
  connect(khtmleditui->pushButton_h6, SIGNAL(clicked()), SLOT(slotbuttonh6clicked()));
  connect(khtmleditui->pushButton_underlined, SIGNAL(clicked()), SLOT(slotbuttonunderlinedclicked()));
  connect(khtmleditui->pushButton_italic, SIGNAL(clicked()), SLOT(slotbuttonitalicclicked()));
  connect(khtmleditui->ktextedit_html, SIGNAL(textChanged()), SLOT(slothtmltextchanged()));
  connect(khtmleditui->ktextedit_plain, SIGNAL(textChanged()), SLOT(slotplaintextchanged()));
  connect(khtmleditui->actionSave, SIGNAL(triggered()), SLOT(slotbuttonsaveclicked()));
  connect(khtmleditui->actionSave_as, SIGNAL(triggered()), SLOT(slotbuttonsaveasclicked()));
  connect(khtmleditui->actionLoad, SIGNAL(triggered()), SLOT(slotactionload()));
  connect(khtmleditui->actionQuit, SIGNAL(triggered()), SLOT(slotquit()));
  khtmleditui->ktextedit_html->setText(content);
  khtmleditui->ktextedit_plain->setPlainText(content);
  dirty=false;
  kDebug() << "Leaving function";
}

void khtmledit::slothtmltextchanged()
{
  kDebug() << "Entering function";
  dirty=true;
  if (slotplaintextmodifiedhtml) slotplaintextmodifiedhtml=false;
  else 
    if (khtmleditui->ktextedit_plain->toPlainText() != khtmleditui->ktextedit_html->toHtml())
      khtmleditui->ktextedit_plain->setPlainText(khtmleditui->ktextedit_html->toHtml());
}

void khtmledit::slotplaintextchanged()
{
  kDebug() << "Entering function";
  dirty=true;
  // maybe slothtmltextchanged triggered slotplaintextchanged -> do not loop infinitely
  if (khtmleditui->ktextedit_plain->toPlainText() != khtmleditui->ktextedit_html->toHtml())
  {
    // if the content is valid html, set ktextedit_html
    slotplaintextmodifiedhtml=true;
    khtmleditui->ktextedit_html->setText(khtmleditui->ktextedit_plain->toPlainText());
  }
}

void khtmledit::slotbuttonsaveasclicked()
{
  kDebug() << "Entering function";
  mfilename = KFileDialog::getSaveFileName( QString(), QString(), this );
  QString err;
  static QFile file(mfilename);
  KUrl url(mfilename);
  kDebug() << url.url();
  if ((url.isLocalFile()) || (!url.url().contains("/")))
  { // save locally
    static QTextStream stream(&file);
    if (file.open(QIODevice::WriteOnly))
    {
      file.write(khtmleditui->ktextedit_html->toHtml().toUtf8());
      file.close();
      dirty=false;
    }
  }
  else // use remote file
  {
    KTemporaryFile tmpFile;
    if ( !tmpFile.open() ) err = QString::fromLatin1( "Unable to get temporary file" );
    else
    {
      QTextStream stream ( &tmpFile );
      stream << khtmleditui->ktextedit_html->toHtml().toUtf8();
      stream.flush();
      if (!KIO::NetAccess::upload( tmpFile.fileName(), url, 0 )) err=QString::fromLatin1("Could not upload");
      else dirty=false;
    }
  }
  kDebug() << "Leaving function";
}

void khtmledit::slotbuttonsaveclicked()
{
  kDebug() << "Entering function";
  QString err;
  static QFile file(mfilename);
  KUrl url(mfilename);
  kDebug() << url.url();
  if ((url.isLocalFile()) || (!url.url().contains("/")))
  { // save locally
    static QTextStream stream(&file);
    if (file.open(QIODevice::WriteOnly))
    {
      file.write(khtmleditui->ktextedit_html->toHtml().toUtf8());
      file.close();
      dirty=false;
    }
  }
  else // use remote file
  {
    KTemporaryFile tmpFile;
    if ( !tmpFile.open() ) err = QString::fromLatin1( "Unable to get temporary file" );
    else
    {
      QTextStream stream ( &tmpFile );
      stream << khtmleditui->ktextedit_html->toHtml().toUtf8();
      stream.flush();
      if (!KIO::NetAccess::upload( tmpFile.fileName(), url, 0 )) err=QString::fromLatin1("Could not upload");
      else dirty=false;
    }
  }
  kDebug() << "Leaving function";
}

void khtmledit::slotquit()
{
  kDebug() << "dirty= "<< dirty;
  if ((!dirty) || (KMessageBox::warningYesNo(0, "Content has changed, really quit ?") == KMessageBox::Yes) ) kapp->exit();
}

void khtmledit::slotbuttonfatclicked()
{
  kDebug() << "Entering khtmledit::slotbuttonfatclicked";
  QString text=khtmleditui->ktextedit_html->textCursor().selectedText();
  kDebug() << "text =" << text;
  kDebug() << "fontweight=" << khtmleditui->ktextedit_html->fontWeight();
  if (khtmleditui->ktextedit_html->fontWeight() == 50) khtmleditui->ktextedit_html->setFontWeight(75);
  else khtmleditui->ktextedit_html->setFontWeight(50);
  kDebug() << "Leaving khtmledit::slotbuttonfatclicked";
}

void khtmledit::slotbuttonunderlinedclicked()
{
  kDebug() << "Entering function";
  if ( khtmleditui->ktextedit_html->fontUnderline() ) khtmleditui->ktextedit_html->setFontUnderline( false );
  else khtmleditui->ktextedit_html->setFontUnderline( true );
  kDebug() << "Leaving function";
}

void khtmledit::slotbuttonitalicclicked()
{
  kDebug() << "Entering function";
  if ( khtmleditui->ktextedit_html->fontItalic() ) khtmleditui->ktextedit_html->setFontItalic( false );
  else khtmleditui->ktextedit_html->setFontItalic( true );
  kDebug() << "Leaving function";
}

void khtmledit::slotbuttonh1clicked()
{
  kDebug() << "Entering khtmledit::slotbuttonfatclicked";
  QString text=khtmleditui->ktextedit_html->textCursor().selectedText();
  kDebug() << "text =" << text;
  text=QString("%1%2%3").arg("<h1>").arg(text).arg("</h1>");
  kDebug() << "text =" << text;
  khtmleditui->ktextedit_html->textCursor().removeSelectedText();
  khtmleditui->ktextedit_html->textCursor().insertHtml(text.replace(" ","&nbsp;"));
  kDebug() << "Leaving khtmledit::slotbuttonfatclicked";
}

void khtmledit::slotbuttonh2clicked()
{
  kDebug() << "Entering khtmledit::slotbuttonfatclicked";
  QString text=khtmleditui->ktextedit_html->textCursor().selectedText();
  kDebug() << "text =" << text;
  text=QString("%1%2%3").arg("<h2>").arg(text).arg("</h2>");
  kDebug() << "text =" << text;
  khtmleditui->ktextedit_html->textCursor().removeSelectedText();
  khtmleditui->ktextedit_html->textCursor().insertHtml(text.replace(" ","&nbsp;"));
  kDebug() << "Leaving khtmledit::slotbuttonfatclicked";
}

void khtmledit::slotbuttonh3clicked()
{
  kDebug() << "Entering khtmledit::slotbuttonfatclicked";
  QString text=khtmleditui->ktextedit_html->textCursor().selectedText();
  kDebug() << "text =" << text;
  text=QString("%1%2%3").arg("<h3>").arg(text).arg("</h3>");
  kDebug() << "text =" << text;
  khtmleditui->ktextedit_html->textCursor().removeSelectedText();
  khtmleditui->ktextedit_html->textCursor().insertHtml(text.replace(" ","&nbsp;"));
  kDebug() << "Leaving khtmledit::slotbuttonfatclicked";
}

void khtmledit::slotbuttonh4clicked()
{
  kDebug() << "Entering khtmledit::slotbuttonfatclicked";
  QString text=khtmleditui->ktextedit_html->textCursor().selectedText();
  kDebug() << "text =" << text;
  text=QString("%1%2%3").arg("<h4>").arg(text).arg("</h4>");
  kDebug() << "text =" << text;
  khtmleditui->ktextedit_html->textCursor().removeSelectedText();
  khtmleditui->ktextedit_html->textCursor().insertHtml(text.replace(" ","&nbsp;"));
  kDebug() << "Leaving khtmledit::slotbuttonfatclicked";
}

void khtmledit::slotbuttonh5clicked()
{
  kDebug() << "Entering khtmledit::slotbuttonfatclicked";
  QString text=khtmleditui->ktextedit_html->textCursor().selectedText();
  kDebug() << "text =" << text;
  text=QString("%1%2%3").arg("<h5>").arg(text).arg("</h5>");
  kDebug() << "text =" << text;
  khtmleditui->ktextedit_html->textCursor().removeSelectedText();
  khtmleditui->ktextedit_html->textCursor().insertHtml(text.replace(" ","&nbsp;"));
  kDebug() << "Leaving khtmledit::slotbuttonfatclicked";
}

void khtmledit::slotbuttonh6clicked()
{
  kDebug() << "Entering khtmledit::slotbuttonfatclicked";
  QString text=khtmleditui->ktextedit_html->textCursor().selectedText();
  kDebug() << "text =" << text;
  text=QString("%1%2%3").arg("<h6>").arg(text).arg("</h6>");
  kDebug() << "text =" << text;
  khtmleditui->ktextedit_html->textCursor().removeSelectedText();
  khtmleditui->ktextedit_html->textCursor().insertHtml(text.replace(" ","&nbsp;"));
  kDebug() << "Leaving khtmledit::slotbuttonfatclicked";
}

void khtmledit::slotfilechanged()
{
  kDebug() << "Entering slotfilechanged" << endl;
  load();
  khtmleditui->ktextedit_plain->setText(content);
  kDebug() << "Leaving slotfilechanged" << endl;
}

void khtmledit::slotactionload()
{
  KFileDialog::getOpenFileName( QString(), QString(), this, 0 );
  QString err=load(mfilename);
  if ( err != QString() ) KMessageBox::error(0,err);
}
