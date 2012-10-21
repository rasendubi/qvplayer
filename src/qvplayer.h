/*
 *  QVPlayer. VKontakte player
 *  Copyright (C) 2012  Alexey Shmalko <dubi.rasen@gmail.com> and Sochka Oleksandr <sasha.sochka@gmail.com>
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef QVPLAYER_H
#define QVPLAYER_H

#include <QList>
#include <QStringList>
#include <QStringListModel>
#include <QSystemTrayIcon>
#include <QWidget>

#include <phonon/AudioOutput>
#include <phonon/MediaObject>
#include <phonon/MediaSource>
#include <phonon/Path>

#include <qtvk/audiofile.h>
#include <qtvk/user.h>

class QAction;
namespace Ui {
  class QVPlayer;
}

class QVPlayer : public QWidget
{
  Q_OBJECT
  
public:
  explicit QVPlayer(QWidget *parent = 0);
  ~QVPlayer();
  
private:
  Ui::QVPlayer *ui;
  QStringListModel *stringmodel;
  QStringListModel *userModel;
  QVector<Phonon::MediaSource> sources;
  QVector<int> userIds;
  QString token;
  Phonon::AudioOutput *audioOutput;
  Phonon::MediaObject *mediaObject;
  int curSourceId;
  QSystemTrayIcon *tray;
  QString cookiesPath;
  bool repeatTrack;
  
  QAction *quitAction,
          *muteAction,
          *stopAction,
          *playAction,
          *pauseAction,
          *nextAction,
          *preAction,
          *searchAction,
          *homeAction,
          *showHideAction,
          *shuffleAction,
          *clearCookiesAction,
          *repeatTrackAction;
  
private:
  virtual void closeEvent(QCloseEvent* );
  void setupActions();
  void setupTray();
  
private slots:
  void accepted(QString token);
  void audioRequestFinished(QList<Vk::AudioFile> list);
  void userRequestFinished(QList<Vk::User> list);
  void audioClicked(const QModelIndex& index);
  void userClicked(const QModelIndex& index);
  void audioPre();
  void audioNext();
  void audioEnd();
  void audioHome();
  void listShuffle();
  void clearCookies();
  void trayActivated(QSystemTrayIcon::ActivationReason reason);
  void searchClicked();
  void getFriends();
  void muteClicked(bool state);
  void mediaStateChanged(Phonon::State state,Phonon::State oldstate);
  void showHide();
  void repeatTrackClicked(bool state);
};

#endif // QVPLAYER_H
