#ifndef IMAGESSORTFILTERMODEL_H
#define IMAGESSORTFILTERMODEL_H

#include <QSortFilterProxyModel>

namespace ImagesModels
{

class ImagesModel;

class ImagesSortFilterModel: public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit ImagesSortFilterModel(ImagesModel &sourceModel, QObject *parent = nullptr);

    QRegularExpression regularExpression() const;

public slots:
    void setFilterMask(const QString &newFilterMask);

signals:
    void filterMaskChanged(QString filterMask);

    void regularExpressionChanged(const QRegularExpression &regularExpression);

protected:
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
    virtual bool lessThan
    (
        const QModelIndex &source_left,
        const QModelIndex &source_right
    ) const override;

private:
    void setRegularExpression(const QRegularExpression &newRegularExpression);

    ImagesModel &m_sourceImagesModel;
    QRegularExpression m_regularExpression{};
};

} // namespace ImagesModels

#endif // IMAGESSORTFILTERMODEL_H
