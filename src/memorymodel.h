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
    void updateData(QModelIndex top_left = QModelIndex(), QModelIndex bottom_right = QModelIndex());
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    /**
     * Emits dataChanged() for addresses modified by an instruction, marks those cells for highlighting
     *
     * @param address
     */
    void handleMemoryChanged(uint16_t address);

    /**
     * Clears the `newlyChangedCells vector`
     */
    void clearHighlight();

    /**
     * Vector of cells affected by the last instruction
     *
     * @brief newlyChangedCells
     */
    std::vector<QModelIndex> newly_changed_cells;
private:
    const size_t kMemorySize;
    Emulator *emulator;
};

#endif // MEMORYMODEL_H
