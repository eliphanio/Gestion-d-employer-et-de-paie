#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QSqlDatabase"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
class PointageWidget;
}
QT_END_NAMESPACE

class PointageWidget;   // <-- ligne ajoutée (déclaration anticipée)

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void affichePoste();
    void afficheEmploye();

private slots:
    void on_pushButton_clicked();

    void quitClicked();

private:
    Ui::MainWindow *ui;
    QSqlDatabase db;
    PointageWidget *pagePointage = nullptr;   // <-- ligne ajoutée


};
#endif // MAINWINDOW_H
