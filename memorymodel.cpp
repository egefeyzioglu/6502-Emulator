#include "memorymodel.h"

MemoryModel::MemoryModel(size_t kMemorySize, Emulator *emulator, QObject *parent) : QAbstractTableModel{parent}, kMemorySize{kMemorySize}, emulator{emulator} {}

MemoryModel::~MemoryModel(){}

int MemoryModel::rowCount(const QModelIndex &parent) const{
    return kMemorySize/0x10;
}

int MemoryModel::columnCount(const QModelIndex &parent) const{
    return 0x10;
}

QVariant MemoryModel::headerData(int section, Qt::Orientation orientation, int role) const{
    // We're only interested in the display role
    if(role != Qt::DisplayRole) return QVariant();
    // Pad to 4 digits if it's the vertical header, don't if horizontal. Format in base 16
    if(orientation == Qt::Orientation::Horizontal)
        return QString("%1").arg(section, 1, 16);
    else
        return QString("%1").arg(section * this -> columnCount(), 4, 16, QLatin1Char('0'));
}

QVariant MemoryModel::data(const QModelIndex &index, int role) const{
    // We're only interested in the display and tooltip roles
    if(role != Qt::DisplayRole && role != Qt::ToolTipRole){
        return QVariant();
    }
    // If we have a good index, return
    if(index.isValid() && index.row() < rowCount() && index.row() >= 0 && index.column() < columnCount() && index.column() >= 0){
        auto ret = QVariant(QString("%1").arg((emulator -> getMemoryValue((index.row()) * columnCount() + index.column())), 2, 16, QLatin1Char('0')));
        return ret;
    } else {
        return QVariant();
    }
}

bool MemoryModel::setData(const QModelIndex &index, const QVariant &value, int role){
    if(index.isValid() && index.row() < kMemorySize && index.row() >= 0){
        bool ok;
        int val = value.toString().toUInt(&ok, 16);
        if(ok && val < 0x100) {
            emulator -> setMemoryValue((index.row()) * columnCount() + index.column(), val);
            return true;
        }
    }
    return false;
}

Qt::ItemFlags MemoryModel::flags(const QModelIndex &index) const{
    return Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

void MemoryModel::updateData(QModelIndex topLeft, QModelIndex bottomRight){
    if(!topLeft.isValid()){
        topLeft = index(0,0);
    }
    if(!bottomRight.isValid()){
        bottomRight = index(this -> rowCount(), this -> columnCount());
    }
    emit dataChanged(topLeft, bottomRight);
}
