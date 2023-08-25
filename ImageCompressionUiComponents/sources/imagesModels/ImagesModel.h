#ifndef IMAGESMODEL_H
#define IMAGESMODEL_H

#include <QAbstractItemModel>

namespace ImagesModels
{

class ImagesModelItem;

class ImagesModel: public QAbstractListModel
{
	Q_OBJECT

public:
    using QAbstractListModel::QAbstractListModel;

    enum Roles
    {
        ItemObject = Qt::UserRole + 1
    };

public:
    virtual int rowCount(const QModelIndex &parent = {}) const override;
	virtual QVariant data(const QModelIndex &index, int role) const override;

    const ImagesModelItem *at(qsizetype i) const;
    ImagesModelItem *item(qsizetype i);

    void append(ImagesModelItem &newItem);

    virtual QHash<int, QByteArray> roleNames() const override;

private:
    QList<ImagesModelItem *> m_data;
    QMap<QString, ImagesModelItem *> m_itemsByName;

public:
    virtual bool removeRows(int row, int count, const QModelIndex &parent) override;

public:
    virtual QModelIndex index(int row, int column, const QModelIndex &parent) const override;
};

} // namespace ImagesModels

#endif // IMAGESMODEL_H
