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


#include "audiotablemodel.h"

AudioTableModel::AudioTableModel(QObject* parent): QAbstractTableModel(parent)
{
}

void AudioTableModel::setAudioList(const QVector< Vk::AudioFile >& audioList)
{
  emit beginResetModel();
  this->audioList = audioList;
  emit endResetModel();  
}

const QVector< Vk::AudioFile >& AudioTableModel::getAudioList() const
{
  return audioList;
}

QVariant AudioTableModel::data(const QModelIndex& index, int role) const
{
  if( !index.isValid() )
    return QVariant();
  
  if( index.row() >= audioList.size() )
    return QVariant();
  
  switch( role )
  {
    case Qt::DisplayRole:
      if( index.column() == 0 )
        return audioList.at(index.row()).artist;
      else if( index.column() == 1 )
        return audioList.at(index.row()).title;
      else if( index.column() == 2 )
      {
        int dur = audioList.at(index.row()).duration;
        return QString::number(dur/60) + ":" + QString::number(dur%60+100).right(2);
      }
      else 
        return QVariant();
      
    default:
      return QVariant();
  }
}

QVariant AudioTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if( orientation != Qt::Horizontal || role != Qt::DisplayRole )
    return QAbstractItemModel::headerData(section, orientation, role);
  else if( section == 0 )
    return "Artist";
  else if( section == 1 )
    return "Title";
  else if( section == 2 )
    return "Duration";
  else
    return QVariant();
}

int AudioTableModel::columnCount(const QModelIndex& parent) const
{
  return 3;
}

int AudioTableModel::rowCount(const QModelIndex& parent) const
{
  return audioList.count();
}

