#include "stdafx.h"
#include "NodeListComponents.h"

VariantSetModel::VariantSetModel(QObject* parent)
    : QAbstractTableModel(parent)
{ }

///////////////////////////////////////////////////////////////////////////////

QVariant VariantSetModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole) {
        return QVariant();
    }
    switch (section) {
        case 0: return "name";
        case 1: return "variants";
    }
    return QVariant();
}

QVariant VariantSetModel::data(const QModelIndex& index, int role) const {
    if (role != Qt::DisplayRole) {
        return QVariant();
    }
    switch (index.column()) {
        case 0: return items_[index.row()].name.c_str();
        case 1: return items_[index.row()].variantNames[0].c_str();
    }
    return QVariant();
}

void VariantSetModel::add(const UsdVariantSet& variantSet) {
    beginInsertRows(QModelIndex(), items_.size(), items_.size());
    items_.emplace_back(variantSet.GetName(), variantSet.GetVariantNames(), variantSet.GetVariantSelection());
    endInsertRows();
}

void VariantSetModel::clear() {
    beginResetModel();
    items_.clear();
    endResetModel();
}

///////////////////////////////////////////////////////////////////////////////

VariantSetSelectorDelegate::VariantSetSelectorDelegate(QObject* parent) : QStyledItemDelegate(parent) {}

QWidget* VariantSetSelectorDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    auto pModel = reinterpret_cast<const VariantSetModel*>(index.model());
    const auto& item = pModel->item(index);

    auto pComboBox = new QComboBox(parent);
    for (const auto& variant : item.variantNames) {
        pComboBox->addItem(variant.c_str());
    }
    pComboBox->setCurrentText(item.selectedVariantName.c_str());

    connect(pComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [](int index) { qDebug() << index; });
    connect(pComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &VariantSetSelectorDelegate::selectionChanged);

    return pComboBox;
}

///////////////////////////////////////////////////////////////////////////////

VariantSetListWidget::VariantSetListWidget(QWidget* parent)
    : QWidget(parent) {
    view_.setModel(&model_);
}

void VariantSetListWidget::setupUi() {
    layout_.setContentsMargins(0, 0, 0, 0);
    layout_.addWidget(&view_);
    setLayout(&layout_);

    view_.verticalHeader()->hide();
    view_.setItemDelegateForColumn(1, new VariantSetSelectorDelegate(&view_));
}

void VariantSetListWidget::set(UsdPrim prim) {
    model_.clear();
    const auto variantSetNames = prim.GetVariantSets().GetNames();
    for (auto i = 0; i < variantSetNames.size(); ++i) {
        model_.add(prim.GetVariantSet(variantSetNames[i]));
        view_.openPersistentEditor(model_.index(i, 1));
    }
}
