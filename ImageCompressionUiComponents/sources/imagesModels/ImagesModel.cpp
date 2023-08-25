#include "ImagesModel.h"

#include "ImagesModelItem.h"

namespace ImagesModels
{

int ImagesModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;

	return m_data.size();
}

QVariant ImagesModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant{};

    if (role == Roles::ItemObject)
        return QVariant::fromValue(m_data.at(index.row()));
    else
        return QVariant();
}

const ImagesModelItem *ImagesModel::at(qsizetype i) const
{
    return m_data.at(i);
}

ImagesModelItem *ImagesModel::item(qsizetype i)
{
    return m_data.value(i);
}

void ImagesModel::append(ImagesModelItem &newItem)
{
    auto index = rowCount();

    newItem.setParent(this);

    beginInsertRows({}, index, index);
    m_data.append(&newItem);
    endInsertRows();
}

QHash<int, QByteArray> ImagesModel::roleNames() const
{
	QHash<int, QByteArray> roles;
    roles[Roles::ItemObject] = "item";

	return roles;
}

bool ImagesModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent)

    this->beginRemoveRows({}, row, row + count - 1);
    m_data.remove(row, count);
    this->endRemoveRows();

    return true;
}

QModelIndex ImagesModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return createIndex(row, column, m_data.at(row));
}

} // namespace ImagesModels
