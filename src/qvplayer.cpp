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

#include "qvplayer.h"
#include "ui_qvplayer.h"

#include <QApplication>
#include <QDir>
#include <QCloseEvent>
#include <QMenu>
#include <QShortcut>
#include <QtWebKit/QWebView>

#include <phonon/AudioOutput>

#include <qtvk/audio/search.h>
#include <qtvk/audio/get.h>
#include <qtvk/friends/get.h>
#include <qtvk/vkauth.h>

#include <utility>

QVPlayer::QVPlayer(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::QVPlayer),
  curSourceId(0),
  audioOutput(new Phonon::AudioOutput(Phonon::MusicCategory, this)),
  mediaObject(new Phonon::MediaObject(this))
{
  ui->setupUi(this);
  
  repeatTrack = false;
  
  stringmodel = new QStringListModel(this);
  ui->listView->setModel(stringmodel);
  connect(ui->listView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(audioClicked(QModelIndex)));
  
  userModel = new QStringListModel(this);
  ui->userView->setModel(userModel);
  connect(ui->userView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(userClicked(QModelIndex)));
  
#ifndef Q_WS_WIN
  QString appdir = ".qvplayer";
#else
  QString appdir = "qvplayer";
#endif
  if( !QDir::home().exists(appdir) )
    QDir::home().mkdir(appdir);
  cookiesPath = QDir::homePath() + "/" + appdir + "/cookies.ck";
  
  Vk::VkAuth *auth = new Vk::VkAuth(cookiesPath, this);
  connect(auth, SIGNAL(authAccepted(QString)), this, SLOT(accepted(QString)));
  connect(auth, SIGNAL(authCanceled()), this, SLOT(close()));
  auth->auth("2921193", "audio,friends")->show();
  
  mediaObject->setTickInterval(1000);
  Phonon::createPath(mediaObject, audioOutput);
  ui->volumeSlider->setAudioOutput(audioOutput);
  ui->seekSlider->setMediaObject(mediaObject);
  
  connect(mediaObject, SIGNAL(stateChanged(Phonon::State,Phonon::State)),
          this, SLOT(mediaStateChanged(Phonon::State,Phonon::State)));
  
  connect(mediaObject, SIGNAL(finished()), this, SLOT(audioEnd()));
  
  connect(ui->searchEdit, SIGNAL(returnPressed()), this, SLOT(searchClicked()));
  
  setupActions();
  setupTray();
  
  ui->playButton   ->setDefaultAction(toggleAction);
  ui->preButton    ->setDefaultAction(preAction);
  ui->nextButton   ->setDefaultAction(nextAction);
  ui->searchButton ->setDefaultAction(searchAction);
  ui->homeButton   ->setDefaultAction(homeAction);
  ui->muteButton   ->setDefaultAction(muteAction);
  ui->shuffleButton->setDefaultAction(shuffleAction);
  ui->repeatButton ->setDefaultAction(repeatTrackAction);
  
  setWindowIcon(QIcon(":/img/music.png"));  
}

QVPlayer::~QVPlayer()
{
  delete ui;
}

void QVPlayer::setupActions()
{
  quitAction     = new QAction(tr("&Quit"),  this);
  muteAction     = new QAction(tr("&Mute"),  this);
  muteAction->setCheckable(true);
  stopAction     = new QAction(tr("&Stop"),  this);
  toggleAction   = new QAction(tr("P&lay"),  this);
  nextAction     = new QAction(tr("&Next"),  this);
  preAction      = new QAction(tr("&Pre"),   this);
  searchAction   = new QAction(tr("&Search"),this);
  homeAction     = new QAction(tr("Home page"), this);
  showHideAction = new QAction(tr("Show/&Hide"), this);
  shuffleAction  = new QAction(tr("Shuffle"),this);
  clearCookiesAction = new QAction(tr("Clear cookies"), this);
  repeatTrackAction  = new QAction(tr("Repeat"), this);
  repeatTrackAction->setCheckable(true);

  //Setup shortcuts
  muteAction->setShortcuts(QList<QKeySequence>() 
    << QKeySequence("Ctrl+m") 
    << QKeySequence("m")
  );
  toggleAction->setShortcuts(QList<QKeySequence>()
    << QKeySequence(Qt::CTRL + Qt::Key_Space)
    << QKeySequence(Qt::Key_Space)
  );
  nextAction->setShortcuts(QList<QKeySequence>()
    << QKeySequence(Qt::CTRL + Qt::Key_Right)
    << QKeySequence(Qt::Key_Right)
  );
  preAction->setShortcuts(QList<QKeySequence>()
    << QKeySequence(Qt::CTRL + Qt::Key_Left)
    << QKeySequence(Qt::Key_Left)
  );
  homeAction->setShortcuts(QList<QKeySequence>()
    << QKeySequence("Ctrl+h")
    << QKeySequence("h")
  );
  shuffleAction->setShortcuts(QList<QKeySequence>()
    << QKeySequence("Ctrl+s")
    << QKeySequence("s")
  );
  repeatTrackAction->setShortcuts(QList<QKeySequence>()
    << QKeySequence("Ctrl+r")
    << QKeySequence("r")
  );  
  
  //Setup icons
  muteAction        -> setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
  stopAction        -> setIcon(style()->standardIcon(QStyle::SP_MediaStop));
  toggleAction      -> setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
  nextAction        -> setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
  preAction         -> setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
  homeAction        -> setIcon(QIcon(":/img/home.png"));
  shuffleAction     -> setIcon(QIcon(":/img/shuffle.png"));
  repeatTrackAction -> setIcon(QIcon(":/img/repeat.png"));

  //Setup tooltips
  quitAction     -> setToolTip(tr("Quit the application"));
  muteAction     -> setToolTip(tr("Mute sound"));
  stopAction     -> setToolTip(tr("Stop music"));
  toggleAction   -> setToolTip(tr("Play track"));
  nextAction     -> setToolTip(tr("Play next audio"));
  preAction      -> setToolTip(tr("Play previous audio"));
  searchAction   -> setToolTip(tr("Search audios"));
  homeAction     -> setToolTip(tr("Switch to home playlist"));
  showHideAction -> setToolTip(tr("Show/hide player"));
  shuffleAction  -> setToolTip(tr("Shuffle playlist"));
  clearCookiesAction->setToolTip(tr("Clear authorization cookies"));
  repeatTrackAction ->setToolTip(tr("Repeat current track"));
  
  //Connect signals
  connect(quitAction, SIGNAL(triggered(bool)), 
          qApp, SLOT(quit()));
  connect(muteAction, SIGNAL(triggered(bool)),
          audioOutput, SLOT(setMuted(bool)));
  connect(muteAction, SIGNAL(triggered(bool)),
          this, SLOT(muteClicked(bool)));
  connect(stopAction, SIGNAL(triggered(bool)),
          mediaObject, SLOT(stop()));
  connect(toggleAction, SIGNAL(triggered(bool)),
          this, SLOT(audioToggle()));
  connect(nextAction, SIGNAL(triggered(bool)),
          this, SLOT(audioNext()));
  connect(preAction, SIGNAL(triggered(bool)),
          this, SLOT(audioPre()));
  connect(searchAction, SIGNAL(triggered(bool)),
          this, SLOT(searchClicked()));
  connect(homeAction, SIGNAL(triggered(bool)),
          this, SLOT(audioHome()));
  connect(showHideAction, SIGNAL(triggered(bool)),
          this, SLOT(showHide()));
  connect(shuffleAction, SIGNAL(triggered(bool)),
          this, SLOT(listShuffle()));
  connect(clearCookiesAction, SIGNAL(triggered(bool)),
          this, SLOT(clearCookies()));
  connect(repeatTrackAction, SIGNAL(triggered(bool)),
          this, SLOT(repeatTrackClicked(bool)));
}

void QVPlayer::setupTray()
{
  tray = new QSystemTrayIcon(QIcon(":/img/music.png"));
  connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), 
          this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));
  tray->setContextMenu(new QMenu(tr("Main menu")));
  tray->contextMenu()->addAction(showHideAction);
  tray->contextMenu()->addSeparator();
  tray->contextMenu()->addAction(clearCookiesAction);
  tray->contextMenu()->addSeparator();
  tray->contextMenu()->addAction(toggleAction);
  tray->contextMenu()->addAction(stopAction);
  tray->contextMenu()->addSeparator();
  tray->contextMenu()->addAction(preAction);
  tray->contextMenu()->addAction(nextAction);
  tray->contextMenu()->addAction(muteAction);
  tray->contextMenu()->addSeparator();
  tray->contextMenu()->addAction(quitAction);
}

void QVPlayer::accepted(QString token)
{
  this->token = token;
  show();
  tray->show();
  audioHome();
  getFriends();
}

void QVPlayer::audioClicked(const QModelIndex& index)
{
  curSourceId = index.row();
  mediaObject->setCurrentSource(sources.at(curSourceId));
  mediaObject->play();
}

void QVPlayer::userClicked(const QModelIndex& index)
{
  Vk::Audio::Get *getAudioRequest = new Vk::Audio::Get(token, userIds[index.row()]);
  connect(getAudioRequest, SIGNAL(finished(QList<Vk::AudioFile>)), this, SLOT(audioRequestFinished(QList<Vk::AudioFile>)));
  getAudioRequest->exec();
  ui->listView->setEnabled(false);
  ui->status->setText(tr("Friend: ") + userModel->stringList().value(index.row()));
}

void QVPlayer::audioToggle()
{
  if( mediaObject->state() == Phonon::PlayingState )
    mediaObject->pause();
  else if( mediaObject->state() == Phonon::LoadingState || mediaObject->state() == Phonon::StoppedState)
  {
    if( sources.isEmpty() )
      return;
    QModelIndex index = ui->listView->selectionModel()->currentIndex();
    curSourceId = (index.isValid() ? index.row() : 0);
    ui->listView->setCurrentIndex(ui->listView->model()->index(curSourceId, 0));
    audioClicked(ui->listView->model()->index(curSourceId, 0));
  }
  else
    mediaObject->play();
}

void QVPlayer::audioPre()
{
  if(sources.isEmpty())
    return;

  if( --curSourceId < 0 )
    curSourceId = sources.count()-1;
  
  ui->listView->setCurrentIndex(ui->listView->model()->index(curSourceId, 0));
  audioClicked(ui->listView->model()->index(curSourceId, 0));
}

void QVPlayer::audioNext()
{
  if(sources.isEmpty())
    return;

  if( ++curSourceId >= sources.count() )
    curSourceId = 0;
  
  ui->listView->setCurrentIndex(ui->listView->model()->index(curSourceId, 0));
  audioClicked(ui->listView->model()->index(curSourceId, 0));
  
}

void QVPlayer::audioEnd()
{
  if( repeatTrack )
    mediaObject->play();
  else
    audioNext();
}

void QVPlayer::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
  switch( reason )
  {
    case QSystemTrayIcon::Trigger:
      showHide();
      break;
    case QSystemTrayIcon::MiddleClick:
      qApp->quit();
    default:
      ;
  }
}

void QVPlayer::showHide()
{
  if( isVisible() )
    hide();
  else
  {
    setWindowState(Qt::WindowActive);
    show();
  }
}

void QVPlayer::closeEvent(QCloseEvent* event)
{
  if( isVisible() )
  {
    hide();
#ifndef CLOSE_INSTEAD_OF_HIDE
    event->ignore();
#endif
  }
}

void QVPlayer::audioHome()
{
  Vk::Audio::Get *getAudioRequest = new Vk::Audio::Get(token);
  connect(getAudioRequest, SIGNAL(finished(QList<Vk::AudioFile>)), this, SLOT(audioRequestFinished(QList<Vk::AudioFile>)));
  getAudioRequest->exec();
  ui->listView->setEnabled(false);
  ui->status->setText(tr("My page"));
}

void QVPlayer::listShuffle()
{
  QVector<QString> stringVec  = stringmodel->stringList().toVector();
  for(int i = 1; i < sources.count(); i++)
  {
    int r = qrand() % (i+1);
    std::swap(sources[r], sources[i]);
    std::swap(stringVec [r], stringVec [i]);
    if(curSourceId == r || curSourceId == i)
      curSourceId = curSourceId == r ? i : r;
  }
  stringmodel->setStringList(stringVec.toList());
  ui->listView->setCurrentIndex(ui->listView->model()->index(curSourceId, 0));
}

void QVPlayer::clearCookies()
{
  if( QFile::remove(cookiesPath) )
    clearCookiesAction->setEnabled(false);  
}

void QVPlayer::searchClicked()
{
  if( !ui->searchEdit->text().isEmpty() )
  {
    Vk::Audio::Search *searchRequest = new Vk::Audio::Search(token, ui->searchEdit->text());
    connect(searchRequest, SIGNAL(finished(QList<Vk::AudioFile>)), this, SLOT(audioRequestFinished(QList<Vk::AudioFile>)));
    //connect(searchRequest, SIGNAL(finished(QList<Vk::AudioFile>)), searchRequest, SLOT(deleteLater()));
    searchRequest->exec();
    ui->listView->setEnabled(false);
    ui->status->setText(tr("Search: ") + ui->searchEdit->text());
  }
  else
    audioHome();
}

void QVPlayer::getFriends()
{
  Vk::Friends::Get *get = new Vk::Friends::Get(token, 0, "first_name,last_name,hints");
  connect(get, SIGNAL(finished(QList<Vk::User>)), this, SLOT(userRequestFinished(QList<Vk::User>)));
  get->exec();
  ui->userView->setEnabled(false);
}

void QVPlayer::userRequestFinished(QList<Vk::User> list)
{
  userIds.clear();
  QStringList ids;
  foreach( Vk::User user, list )
  {
    userIds.append(user.uid);
    ids.append(user.firstName + " " + user.lastName);
  }
  userModel->setStringList(ids);
  ui->userView->setEnabled(true);
}

void QVPlayer::audioRequestFinished(QList<Vk::AudioFile> list)
{
  sources.clear();
  sources.reserve(list.size());
  QStringList strs;
  foreach( Vk::AudioFile file, list )
  {
    sources.append(Phonon::MediaSource(file.url));
    strs.append(file.artist + " - " + file.title + " (" + QString::number(file.duration/60) 
                + ":" + QString::number(file.duration%60+100).right(2) +")");
  }
  ui->listView->setEnabled(true);
  stringmodel->setStringList(strs);

}

void QVPlayer::muteClicked(bool state)
{
  muteAction->setIcon(state?style()->standardIcon(QStyle::SP_MediaVolumeMuted)
                           :style()->standardIcon(QStyle::SP_MediaVolume));
}

void QVPlayer::mediaStateChanged(Phonon::State state, Phonon::State oldstate )
{
  if( state == Phonon::PlayingState )
  {
    toggleAction->setText(tr("&Pause"));
    toggleAction->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    toggleAction->setToolTip(tr("Pause track"));
  }
  else if( state == Phonon::ErrorState )
    audioNext();
  else
  {
    toggleAction->setText(tr("&Play"));
    toggleAction->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    toggleAction->setToolTip(tr("Play track"));
  }
}

void QVPlayer::repeatTrackClicked(bool state)
{
  repeatTrack = state;
}
