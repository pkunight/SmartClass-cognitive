#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSslConfiguration>
#include <QFile>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QtCharts>
#include <QAudioRecorder>
#include <QMediaPlayer>
#include "emotion.h"
#include "ffmpeg.h"

QT_CHARTS_USE_NAMESPACE

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void accessEmotionApi();
    void emotionReplyFinished(QNetworkReply *);
    void saveAudioInupt();
    void getSpeechToken();
    void speechTokenReplyFinished(QNetworkReply *);
    void accessSpeechApi();
    void speechReplyFinished(QNetworkReply *);
    void luisReplyFinished(QNetworkReply *);
    void ttsReplyFinished(QNetworkReply *);
    void clearTextBrowser();

private:
    Ui::Widget *ui;

    ffmpeg* ff;

    QNetworkAccessManager *manager;
    QNetworkAccessManager *speechTokenManager;
    QNetworkAccessManager *speechAudioManager;
    QNetworkAccessManager *luisManager;
    QNetworkAccessManager *ttsManager;
    QNetworkRequest request;
    QNetworkRequest speechTokenRequest;
    QNetworkRequest speechAudioRequest;
    QNetworkRequest luisRequest;
    QNetworkRequest ttsRequest;
    QNetworkReply *reply;
    QNetworkReply *speechTokenReply;
    QNetworkReply *speechAudioReply;
    QNetworkReply *luisReply;
    QNetworkReply *ttsReply;
    QFile *file;
    QFile *speechAudioInputFile;
    QFile *ttsFile;
    QImage *image;

    QMediaPlayer *qPlayer;

    QAudioRecorder *audioRecorder;

    QSslConfiguration config;

    QString ffmpegDir;
    QString tmpImageFileName;
    QString tmpAudioInputFileName;
    QString emotionUrl;
    QString speechTokenUrl;
    QString speechAudioUrl;
    QString luisUrl;
    QString ttsUrl;
    QString speechToken;
    QByteArray emotionKey;
    QByteArray speechApiKey;
    QByteArray speechSecretKey;
    QByteArray token_post_data;

    emotion *eLineArray[10];
    int eLineArrayP;

    void initItem();
    void updatePieChart(emotion *e);
    void initLineChart();
    void updateLineChart(emotion *e);

    void accessLUISApi(QString q);
    void accessTTSApi(QString text);

};

#endif // WIDGET_H
