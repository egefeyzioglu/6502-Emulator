#include "memorymodel.h"

#include "log.h"

#include "QColor"

MemoryModel::MemoryModel(size_t kMemorySize, Emulator *emulator, QObject *parent) : QAbstractTableModel{parent}, kMemorySize{kMemorySize}, emulator{emulator} {
    // When a new instruction is run, clear all previous highlights
    connect(emulator, &Emulator::instructionRan, this, &MemoryModel::clearHighlight);
    // We need to update the model whenever the memory changes
    connect(emulator, &Emulator::memoryChanged, this, &MemoryModel::handleMemoryChanged);
}

MemoryModel::~MemoryModel(){}

int MemoryModel::rowCount(const QModelIndex &parent) const{
    return kMemorySize/0x10;
}

int MemoryModel::columnCount(const QModelIndex &parent) const{
    return 0x11;
}

QVariant MemoryModel::headerData(int section, Qt::Orientation orientation, int role) const{
    // We're only interested in the display role
    if(role != Qt::DisplayRole) return QVariant();

    // Pad to 4 digits if it's the vertical header, don't if horizontal. Format in base 16
    if(orientation == Qt::Orientation::Horizontal){
        // If we're at the final (dump) column, print title
        if(section == this -> columnCount() - 1)
            return QString("Dump");
        else
            return QString("%1").arg(section, 1, 16);
    }else{
        return QString("%1").arg(section * (this -> columnCount() - 1), 4, 16, QLatin1Char('0'));
    }
}

QVariant MemoryModel::data(const QModelIndex &index, int role) const{
    // Display or tooltip roles
    if(role == Qt::DisplayRole || role == Qt::ToolTipRole){
        // If we have a good index, return the memory value or dump
        if(index.isValid() && index.row() < rowCount() && index.row() >= 0 && index.column() < columnCount() && index.column() >= 0){
            if(index.column() == this -> columnCount() - 1){
                // If we're on the dump column,
                // Grab this row's raw data
                uint8_t line[this -> columnCount() - 1];
                for(int col = 0; col < this -> columnCount() - 1; col++) {
                    line[col] = emulator -> getMemoryValue((index.row()) * columnCount() + col);
                    // Replace unprintable characters with '.' because QString::fromLatin1 won't
                    if(!isprint(line[col])) line[col] = '.';
                }
                // Convert to string and return
                return QVariant(QString::fromLatin1((char*) line, (qsizetype) (this -> columnCount() - 1)));
            }else{
                // Grab memory value and return
                auto ret = QVariant(QString("%1").arg((emulator -> getMemoryValue((index.row()) * (columnCount() - 1) + index.column())), 2, 16, QLatin1Char('0')));
                return ret;
            }
        } else {
            return QVariant();
        }
    }

    // Alignment role
    if(role == Qt::TextAlignmentRole){
        return Qt::AlignCenter;
    }

    // Background brush role
    if(role == Qt::BackgroundRole){
        // If the cell was newly changed, highlight the cell by painting the background
        if(std::find(newlyChangedCells.begin(), newlyChangedCells.end(), index) != newlyChangedCells.end()){
            return QColor(Qt::yellow);
        }
    }

    // All other roles
    return QVariant();
}

bool MemoryModel::setData(const QModelIndex &index, const QVariant &value, int role){
    // If the user is setting the ASCII dump
    if(index.isValid() && index.column() == this -> columnCount() - 1){
        // Grab the string data
        QString input = value.toString();
        std::string inputString = input.toStdString();
        uint8_t *data = (uint8_t*) inputString.data();
        // Set bytes from this row
        for(int i = 0; i < inputString.size(); i++){
            emulator -> setMemoryValue(index.row() * (this -> columnCount() - 1) + i, data[i]);
        }
        return true;
    }
    // Else, so if the user is setting an individual byte
    if(index.isValid()){
        // Convert the input to a hex integer
        bool ok;
        int val = value.toString().toUInt(&ok, 16);
        if(ok && val < 0x100) {
            // And if the parsed integer fits in a byte, set the byte
            emulator -> setMemoryValue((index.row()) * (columnCount() - 1) + index.column(), val);
            return true;
        }
    }
    // Otherwise report the failure
    return false;
}

Qt::ItemFlags MemoryModel::flags(const QModelIndex &index) const{
    // Items are enabled and editable
    return Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

/**
 * Updates the views associated for the area specified
 *
 * @param topLeft Top left of the area to be updated. Defaults to (0,0)
 * @param bottomRight Bottom right of the area to be updated. Defaults to the bottom left of the table
 */
void MemoryModel::updateData(QModelIndex topLeft, QModelIndex bottomRight){
    if(!topLeft.isValid()){
        topLeft = index(0,0);
    }
    if(!bottomRight.isValid()){
        bottomRight = index(this -> rowCount(), this -> columnCount());
    }
    emit dataChanged(topLeft, bottomRight);
}

void MemoryModel::handleMemoryChanged(uint16_t address){
    // Get an index to the cell and ASCII dump that was changed
    int row = address / 0x10;
    int column = address % 0x10;
    QModelIndex cellToUpdate = this -> index(row, column);
    QModelIndex dumpToUpdate = this -> index(row, this -> columnCount() - 1);
    // Add cell and ASCII dump to the list of newly changed cells
    this -> newlyChangedCells.push_back(cellToUpdate);
    this -> newlyChangedCells.push_back(dumpToUpdate);
    // Update the cell and dump
    this -> updateData(cellToUpdate, cellToUpdate);
    this -> updateData(dumpToUpdate, dumpToUpdate);
}

void MemoryModel::clearHighlight(){
    newlyChangedCells.clear();
    this -> updateData();
}
