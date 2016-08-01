#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJsonObject>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QJsonObject makePostRequest(QString str);
    void handleAuthError();
    void handleFileError();
    void writeJsonFile(QString filepath,QString data);
private slots:
    void on_webView_urlChanged(const QUrl &arg1);
	 signals:
	void mySignal();
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
