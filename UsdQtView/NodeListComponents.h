#pragma once

class NodeListWidget : public QListWidget {
public:
    NodeListWidget(QWidget* parent = nullptr)
        : QListWidget(parent) {
    }

    void setupUi() {}
};

///////////////////////////////////////////////////////////////////////////////

struct VariantSetItem {
    std::string name;
    std::vector<std::string> variantNames;
    std::string selectedVariantName;

    VariantSetItem(const std::string& name, const std::vector<std::string> variantNames, const std::string& selection)
        : name(name), variantNames(variantNames), selectedVariantName(selection) {
    }
};

///////////////////////////////////////////////////////////////////////////////

class VariantSetModel : public QAbstractTableModel {
public:
    VariantSetModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override { return items_.size(); }
    int columnCount(const QModelIndex& parent = QModelIndex()) const override { return 2; }
    const VariantSetItem& item(QModelIndex index) const { return items_[index.row()]; }

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    void add(const UsdVariantSet& variantSet);
    void clear();

private:
    std::vector<VariantSetItem> items_;
};

///////////////////////////////////////////////////////////////////////////////

class VariantSetSelectorDelegate : public QStyledItemDelegate {
    Q_OBJECT
signals:
    void selectionChanged(int index);

public:
    VariantSetSelectorDelegate(QObject* parent = nullptr);
    ~VariantSetSelectorDelegate() = default;

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

///////////////////////////////////////////////////////////////////////////////

class VariantSetListWidget : public QWidget {
public:
    VariantSetListWidget(QWidget* parent = nullptr);
    void setupUi();
    void set(UsdPrim prim);

private:
    QVBoxLayout layout_;
    QTableView view_;
    VariantSetModel model_;
};
