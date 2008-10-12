/***************************************************************************
 *   Copyright (C) 2007-2008 by Thorsten Staerk                            *
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

khtmledit

*/

#include <QString>
#include <kapplication.h>
#include <kaboutdata.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kcmdlineargs.h>
#include <KMainWindow>
#include <khtmledit.h>

int main(int argc, char *argv[])
{
  kDebug() << "Entering main";
  KAboutData aboutData( "khtmledit", 0, ki18n("khtmledit"),
      VERSION, ki18n("khtmledit"), KAboutData::License_GPL,
      ki18n("(c) 2008") );
  KCmdLineArgs::init( argc, argv, &aboutData );

  KCmdLineOptions options;
  options.add("+[URL]", ki18n(( "Document to open" )));
  KCmdLineArgs::addCmdLineOptions( options );

  KApplication khello;

  khtmledit *mw = new khtmledit(argv[1]);
  mw->setWindowFlags(Qt::WindowContextHelpButtonHint);
  mw->show();
  khello.exec();
}
