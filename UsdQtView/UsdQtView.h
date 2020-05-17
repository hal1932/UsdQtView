#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QOpenGLWidget>
#include <QtGui/QOpenGLFunctions>
#include "ui_UsdQtView.h"

class UsdQtView : public QMainWindow
{
    Q_OBJECT

public:
    UsdQtView(QWidget *parent = Q_NULLPTR);

private:
    Ui::UsdQtViewClass ui;
};
