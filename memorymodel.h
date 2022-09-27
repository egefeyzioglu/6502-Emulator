#ifndef MEMORYMODEL_H
#define MEMORYMODEL_H

#include <QAbstractListModel>

#include "emulator.h"

class MemoryModel : public QAbstractTableModel
{
public:
    explicit MemoryModel(size_t kMemorySize, Emulator *emulator, QObject *parent = nullptr);
    ~MemoryModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    void updateData(QModelIndex topLeft = QModelIndex(), QModelIndex bottomRight = QModelIndex());
    Qt::ItemFlags flags(const QModelIndex &index) const override;
private:
    const size_t kMemorySize;
    Emulator *emulator;
};

#endif // MEMORYMODEL_H
