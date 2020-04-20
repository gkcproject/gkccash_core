#ifndef SINGLEITEMMODEL_H
#define SINGLEITEMMODEL_H

#include <QObject>
#include <QAbstractListModel>

class SingleItem
{
public:
    SingleItem(const QString &modelData);
    QString modelData() const;

private:
    QString m_modelData;
};

class SingleItemModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum SingleItemRoles {
        ItemDataRole = Qt::UserRole + 1,
    };

    SingleItemModel(QObject *parent = nullptr);


    void addSingleItem(const SingleItem &singleitem);
    void write_newdata(const SingleItem &singleitem);

    void RemoveSingleItem(int index);
    void ClearSingleItem();

    void ChangeSingleItem(int index,const QString &new_data);


    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index,const QVariant &value, int role);

protected:
    QHash<int, QByteArray> roleNames() const;
private:
    QList<SingleItem> m_singleitems;
};

#endif // SINGLEITEMMODEL_H

