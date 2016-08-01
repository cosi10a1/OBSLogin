#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QByteArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include "obs-app.hpp"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->webView->load(QUrl("https://auth.garena.com/oauth/login?client_id=10035&redirect_uri=http://beta.vetv.vn/api/glogin&response_type=token&locale=vi-VN"));
	qApp->addLibraryPath(QDir::currentPath()+"/plugins");
}

MainWindow::~MainWindow()
{
    delete ui;
}
 QJsonObject MainWindow::makePostRequest(QString str)
{
        QByteArray jsonString = "{\"trl\":{\"c\":\"gd\",\"i\":256}}";
        QByteArray postDataSize = QByteArray::number(jsonString.size());
        QUrl url=QUrl(str);
        //QUrl serviceURL("http://192.168.1.108/ask.cgi/");
        QNetworkRequest request(url);
        request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
        request.setRawHeader("Content-Length", postDataSize);

        QNetworkAccessManager test;
        QEventLoop loop;
        connect(&test, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
        QNetworkReply * reply = test.post(request, jsonString);
        loop.exec();

        QByteArray response = reply->readAll();
        QJsonDocument document =QJsonDocument::fromJson(response);
        QJsonObject obj=document.object();
        qDebug()<< " slot" << obj;
        return obj;
}
void MainWindow::handleAuthError()
{
    QMessageBox::critical(this,"Error","Login fail");
   ui->webView->load(QUrl("https://auth.garena.com/oauth/login?client_id=10035&redirect_uri=http://beta.vetv.vn/api/glogin&response_type=token&locale=vi-VN"));
}
void MainWindow::handleFileError()
{
    QMessageBox::critical(this,"Error","Canot write config file");
   ui->webView->load(QUrl("https://auth.garena.com/oauth/login?client_id=10035&redirect_uri=http://beta.vetv.vn/api/glogin&response_type=token&locale=vi-VN"));
}
void MainWindow::writeJsonFile(QString filepath,QString data)
{
QFile File(filepath);
QJsonDocument document = QJsonDocument::fromJson(data.toUtf8());
if (!File.open(QFile::WriteOnly | QFile::Text | QFile::Truncate))
{
	handleFileError();
	return;
}

File.write(document.toJson());
File.close();

}
void MainWindow::on_webView_urlChanged(const QUrl &arg1)
{
    if(ui->webView->url().toString()=="https://auth.garena.com/oauth/login?client_id=10035&redirect_uri=http://beta.vetv.vn/api/glogin&response_type=token&locale=vi-VN")
        return;
    QString str=ui->webView->url().toString();
    QJsonObject obj =makePostRequest(str);
    if(obj["error"].toString()!="0"&&obj["error"].toString()!=""){
        handleAuthError();
        return;
    }
    str="http://beta.vetv.vn/api/get?api_token="+obj["value"].toString();
    obj=makePostRequest(str);
	if (obj["error"].toString() != "0"&&obj["error"].toString() != ""){
		handleAuthError();
		return;
	}
	char path[512];

	if (GetConfigPath(path, sizeof(path), "obs-studio") <= 0)
		return ;
	QString obspath= QString::fromUtf8(path);
	QString currentkeypath = obspath + "/basic/profiles/Untitled/service.json";
	QString serverpath = obspath + "/plugin_config/rtmp-services/services.json";
	QDir dir(obspath+"/plugin_config/rtmp-services/");
	if (!dir.exists()) {
		dir.mkpath(".");
	}
	QString servers = "{\"format_version\":1,\n\"services\":[\n{\n\"common\":true,\n\"name\":\"VST\",\n\"recommended\":{\n\"keyint\":2,\n\"max audio bitrate\":160,\n\"max video bitrate\":3500,\n\"profile\":\"main\",\n\"x264opts\":\"scenecut=0\"},\n\"servers\":[\n{\"name\":\"Encoder 1\",\n\"url\":\"rtmp:\/\/113.171.255.130\/stream\"\n}\n]\n}\n],\n\"version\":51\n}";
	QString keys = "{\"settings\":{\n\"key\":\"" + obj["value"].toObject()["stream_key"].toString() + "\",\n\"server\":\"rtmp:\/\/"+obj["value"].toObject()["stream_server"].toString() + "\/stream\",\n\"service\":\"VST\"\n},\n\"type\":\"rtmp_common\"\n}";
	writeJsonFile(currentkeypath, keys);
	writeJsonFile(serverpath, servers);
	emit mySignal();
	this->close();
}

