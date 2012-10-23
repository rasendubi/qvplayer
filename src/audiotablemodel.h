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


#ifndef AUDIOTABLEMODEL_H
#define AUDIOTABLEMODEL_H

#include <QModelIndex>
#include <QVector>

#include <qtvk/audiofile.h>


class AudioTableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  explicit AudioTableModel(QObject* parent = 0) ;
  
  void setAudioList(const QVector<Vk::AudioFile>& audioList);
  const QVector<Vk::AudioFile>& getAudioList() const;
  
  virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
  virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
  
private:
  QVector<Vk::AudioFile> audioList;
};

#endif // AUDIOTABLEMODEL_H
