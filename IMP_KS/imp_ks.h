#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_impqt.h"

class ImpQt : public QMainWindow
{
    Q_OBJECT

public:
    ImpQt(QWidget *parent = Q_NULLPTR);
    
private slots:
    void onStart();

private:
    void showTip(const QString& tip);

private:
    Ui::impqtClass ui;
};
