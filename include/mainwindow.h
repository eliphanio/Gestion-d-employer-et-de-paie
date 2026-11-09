#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QSqlDatabase"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void affichePoste();
    void afficheEmploye();
    void affichePosteByEmploye();
    void nEmploye();
    void nPoste();
    void rechercherEmploye();

private slots:
    void quitClicked();

private:
    Ui::MainWindow *ui;
    QSqlDatabase db;
    void afficherPagePaie(QWidget *page);
    QWidget *pagePaieCourante = nullptr;

};
#endif // MAINWINDOW_H
