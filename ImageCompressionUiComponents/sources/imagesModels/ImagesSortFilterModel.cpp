#include "ImagesSortFilterModel.h"

#include "ImagesModel.h"
#include "ImagesModelItem.h"

namespace ImagesModels
{

static QRegularExpression maskToRegularExpression(const QString &filterMask);

ImagesSortFilterModel::ImagesSortFilterModel(ImagesModel &sourceModel, QObject *parent)
    : QSortFilterProxyModel{parent}
    , m_sourceImagesModel{sourceModel}
{
    this->setSourceModel(&sourceModel);

    connect(this, &ImagesSortFilterModel::regularExpressionChanged, [this]
    (
        const QRegularExpression &regularExpression
    )
    {
        Q_UNUSED(regularExpression);

        this->invalidateFilter();
    });
}

QRegularExpression ImagesSortFilterModel::regularExpression() const
{
    return m_regularExpression;
}

void ImagesSortFilterModel::setFilterMask(const QString &newFilterMask)
{
    setRegularExpression(maskToRegularExpression(newFilterMask));
}

bool ImagesSortFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    Q_UNUSED(source_parent);

    if (!m_regularExpression.isValid())
        return true;

    auto item = m_sourceImagesModel.at(source_row);
    auto name = item->fileName();

    return m_regularExpression.match(name).hasMatch();
}

bool ImagesSortFilterModel::lessThan
(
    const QModelIndex &source_left,
    const QModelIndex &source_right
) const
{
    auto left = source_left.data(ImagesModel::ItemObject).value<ImagesModelItem *>();
    auto right = source_right.data(ImagesModel::ItemObject).value<ImagesModelItem *>();

    return left->fileName() < right->fileName();
}

void ImagesSortFilterModel::setRegularExpression(const QRegularExpression &newRegularExpression)
{
    if (m_regularExpression == newRegularExpression)
        return;

    m_regularExpression = newRegularExpression;
    emit regularExpressionChanged(m_regularExpression);
}

static QRegularExpression maskToRegularExpression(const QString &filterMask)
{
    QString regExp = filterMask;
    regExp.replace(".", "\\.")
    .replace("?", ".")
    .replace("*", ".*");
    auto regularExpression = QRegularExpression("^" + regExp + "$");

    return regularExpression;
};

} // namespace ImagesModels
