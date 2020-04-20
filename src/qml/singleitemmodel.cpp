#include "singleitemmodel.h"

SingleItem::SingleItem(const QString &modelData)
    :m_modelData(modelData)
{

}

QString SingleItem::modelData() const
{
    return m_modelData;
}

SingleItemModel::SingleItemModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

void SingleItemModel::addSingleItem(const SingleItem &singleitem)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_singleitems << singleitem;
    endInsertRows();
}

void SingleItemModel::RemoveSingleItem(int index)
{
    beginRemoveRows(QModelIndex(), index, index);
    m_singleitems.removeAt(index);
    endRemoveRows();
}

void SingleItemModel::ChangeSingleItem(int index,const QString &new_data)
{
    m_singleitems.replace(index,new_data);
}

void SingleItemModel::ClearSingleItem()
{
    if(rowCount()>0){

        beginRemoveRows(QModelIndex(), 0, rowCount()-1);
        m_singleitems.clear();
        endRemoveRows();
    }
}

int SingleItemModel::rowCount(const QModelIndex & parent) const {
    Q_UNUSED(parent);
    return m_singleitems.count();
}

QVariant SingleItemModel::data(const QModelIndex & index, int role) const {
    if (index.row() < 0 || index.row() >= m_singleitems.count())
        return QVariant();

    const SingleItem &singleitem = m_singleitems[index.row()];
    if (role == ItemDataRole)
        return singleitem.modelData();

    return QVariant();

}





QHash<int, QByteArray> SingleItemModel::roleNames() const {
    QHash<int, QByteArray> roles;

    roles[ItemDataRole] = "modelData";



    return roles;
}


Qt::ItemFlags SingleItemModel::flags(const QModelIndex &index) const
{

    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}


bool SingleItemModel::setData(const QModelIndex &index,const QVariant &value, int role)
{
    if (role == ItemDataRole){
        emit dataChanged(index, index);
        return true;
    }

    return false;
}
