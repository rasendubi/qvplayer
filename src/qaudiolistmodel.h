/*
    QVPlayer. VKontakte player
    Copyright (C) 2012  Alexey Shmalko <dubi.rasen@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef QAUDIOLISTMODEL_H
#define QAUDIOLISTMODEL_H

#include <qt4/QtCore/QModelIndex>
#include <qtvk/audiofile.h>
#include <QList>

Q_DECLARE_METATYPE(Vk::AudioFile)

class QAudioListModel : public QAbstractListModel
{
public:
  virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
  virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
  
private:
  QList<Vk::AudioFile> audios;
};

#endif // QAUDIOLISTMODEL_H
