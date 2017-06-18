#include "widget.h"
#include "ui_widget.h"

using namespace std;

Widget::Widget(QWidget *parent):
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    initItem();
}

void Widget::initItem(){
    qPlayer = new QMediaPlayer();
    ff = ffmpeg::getInstance();
    ffmpegDir = QString::fromStdString(ff->getFFMPEGDir());
    tmpImageFileName = QString::fromStdString(ff->getTmpImageFileName());
    emotionUrl = "https://westus.api.cognitive.microsoft.com/emotion/v1.0/recognize";
    emotionKey = "9192c66a098c4c80be682e1cba0ae42e";

    tmpAudioInputFileName = "tmpAudioInput.wav";
    speechTokenUrl = "https://openapi.baidu.com/oauth/2.0/token";
    speechAudioUrl = "http://vop.baidu.com/server_api";
    ttsUrl = "http://tsn.baidu.com/text2audio";
    speechApiKey = "tlFdL37GfccnAh2eQTrqwTFd";
    speechSecretKey = "uKaG6K7ecAHG8fD8oDU9xF6gaYw1Uyn4";
    speechToken = "";

    luisUrl = "https://westus.api.cognitive.microsoft.com/luis/v2.0/apps/d8c0a952-b9b5-4142-87bf-34d1da58cd0d?subscription-key=6a4d601e42074b708ffe59ae1b563fd9&timezoneOffset=480&verbose=false&q=";

    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::TlsV1SslV3);

    request.setSslConfiguration(config);
    speechTokenRequest.setSslConfiguration(config);

    request.setUrl(QUrl(emotionUrl));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
    request.setRawHeader("Ocp-Apim-Subscription-Key", emotionKey);
    speechTokenRequest.setUrl(QUrl(speechTokenUrl));
    speechTokenRequest.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-www-form-urlencoded"));
    token_post_data = "grant_type=client_credentials";
    token_post_data.append("&client_id=").append(speechApiKey);
    token_post_data.append("&client_secret=").append(speechSecretKey);

    speechAudioRequest.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

    ttsRequest.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-www-form-urlencoded"));

    connect(ui->getEmotionButton, SIGNAL(clicked()), this, SLOT(accessEmotionApi()));
    connect(ui->speechButton, SIGNAL(pressed()), this, SLOT(saveAudioInupt()));
    connect(ui->speechButton, SIGNAL(released()), this, SLOT(accessSpeechApi()));
    connect(ui->clearButton, SIGNAL(clicked()), this, SLOT(clearTextBrowser()));

    manager = new QNetworkAccessManager(this);
    speechTokenManager = new QNetworkAccessManager(this);
    speechAudioManager = new QNetworkAccessManager(this);
    luisManager = new QNetworkAccessManager(this);
    ttsManager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(emotionReplyFinished(QNetworkReply*)));
    connect(speechTokenManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(speechTokenReplyFinished(QNetworkReply*)));
    connect(speechAudioManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(speechReplyFinished(QNetworkReply*)));
    connect(luisManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(luisReplyFinished(QNetworkReply*)));
    connect(ttsManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(ttsReplyFinished(QNetworkReply *)));

    ui->linerchart->setRenderHint(QPainter::Antialiasing);
    ui->piechart->setRenderHint(QPainter::Antialiasing);

    emotion *initE = new emotion(0,0,0,0,0,0,0,0);
    updatePieChart(initE);

    eLineArrayP = 0;
    initLineChart();
    getSpeechToken();
}

void Widget::initLineChart(){
    QGraphicsScene *scence = new QGraphicsScene();
    ui->linerchart->setScene(scence);
    QChart *lineChart = new QChart();
    lineChart->setTitle("emotion line chart");
    lineChart->legend()->setAlignment(Qt::AlignRight);
    lineChart->setAnimationOptions(QChart::AllAnimations);

    QLineSeries *angerLineSeries = new QLineSeries();
    angerLineSeries->setName("anger");
    QLineSeries *contemptLineSeries = new QLineSeries();
    contemptLineSeries->setName("contempt");
    QLineSeries *disgustLineSeries = new QLineSeries();
    disgustLineSeries->setName("disgust");
    QLineSeries *fearLineSeries = new QLineSeries();
    fearLineSeries->setName("fear");
    QLineSeries *happinessLineSeries = new QLineSeries();
    happinessLineSeries->setName("happiness");
    QLineSeries *neutralLineSeries = new QLineSeries();
    neutralLineSeries->setName("neutral");
    QLineSeries *sadnessLineSeries = new QLineSeries();
    sadnessLineSeries->setName("sadness");
    QLineSeries *surpriseLineSeries = new QLineSeries();
    surpriseLineSeries->setName("surprise");

    for(int i=0; i<10; i++){
        eLineArray[i] = new emotion(0,0,0,0,0,0,0,0);
        angerLineSeries->append(i,0);
        contemptLineSeries->append(i,0);
        disgustLineSeries->append(i,0);
        fearLineSeries->append(i,0);
        happinessLineSeries->append(i,0);
        neutralLineSeries->append(i,0);
        sadnessLineSeries->append(i,0);
        surpriseLineSeries->append(i,0);
    }

    lineChart->addSeries(angerLineSeries);
    lineChart->addSeries(contemptLineSeries);
    lineChart->addSeries(disgustLineSeries);
    lineChart->addSeries(fearLineSeries);
    lineChart->addSeries(happinessLineSeries);
    lineChart->addSeries(sadnessLineSeries);
    lineChart->addSeries(neutralLineSeries);
    lineChart->addSeries(surpriseLineSeries);

    lineChart->setGeometry(0,0,650,270);
    lineChart->createDefaultAxes();
    lineChart->setMargins(QMargins(0,0,0,0));
    scence->addItem(lineChart);
    ui->linerchart->show();
}

void Widget::updateLineChart(emotion *e){
    for(int i=0; i<9; i++){
        eLineArray[i] = eLineArray[i+1];
    }
    eLineArray[9] = e;

    QGraphicsScene *scence = new QGraphicsScene();
    ui->linerchart->setScene(scence);
    QChart *lineChart = new QChart();
    lineChart->setTitle("emotion line chart");
    lineChart->legend()->setAlignment(Qt::AlignRight);
    lineChart->setAnimationOptions(QChart::AllAnimations);

    QLineSeries *angerLineSeries = new QLineSeries();
    angerLineSeries->setName("anger");
    QLineSeries *contemptLineSeries = new QLineSeries();
    contemptLineSeries->setName("contempt");
    QLineSeries *disgustLineSeries = new QLineSeries();
    disgustLineSeries->setName("disgust");
    QLineSeries *fearLineSeries = new QLineSeries();
    fearLineSeries->setName("fear");
    QLineSeries *happinessLineSeries = new QLineSeries();
    happinessLineSeries->setName("happiness");
    QLineSeries *neutralLineSeries = new QLineSeries();
    neutralLineSeries->setName("neutral");
    QLineSeries *sadnessLineSeries = new QLineSeries();
    sadnessLineSeries->setName("sadness");
    QLineSeries *surpriseLineSeries = new QLineSeries();
    surpriseLineSeries->setName("surprise");

    for(int i=0; i<10; i++){
        angerLineSeries->append(i,eLineArray[i]->getAnger());
        contemptLineSeries->append(i,eLineArray[i]->getContempt());
        disgustLineSeries->append(i,eLineArray[i]->getDisgust());
        fearLineSeries->append(i,eLineArray[i]->getFear());
        happinessLineSeries->append(i,eLineArray[i]->getHappiness());
        neutralLineSeries->append(i,eLineArray[i]->getNeutral());
        sadnessLineSeries->append(i,eLineArray[i]->getSadness());
        surpriseLineSeries->append(i,eLineArray[i]->getSurprise());
    }

    lineChart->addSeries(angerLineSeries);
    lineChart->addSeries(contemptLineSeries);
    lineChart->addSeries(disgustLineSeries);
    lineChart->addSeries(fearLineSeries);
    lineChart->addSeries(happinessLineSeries);
    lineChart->addSeries(neutralLineSeries);
    lineChart->addSeries(sadnessLineSeries);
    lineChart->addSeries(surpriseLineSeries);

    lineChart->setGeometry(0,0,650,270);
    lineChart->createDefaultAxes();
    lineChart->setMargins(QMargins(0,0,0,0));
    scence->addItem(lineChart);
    ui->linerchart->show();
}

void Widget::updatePieChart(emotion *e){
    QGraphicsScene *scence = new QGraphicsScene();
    ui->piechart->setScene(scence);
    QPieSeries *series = new QPieSeries();
    if(e->getAnger() != 0){
        QPieSlice *sliceAnger = new QPieSlice("anger", e->getAnger());
        sliceAnger->setLabel("anger  "+QString::number(e->getAnger()));
        series->append(sliceAnger);
    }
    if(e->getContempt() != 0){
        QPieSlice *sliceContempt = new QPieSlice("contempt", e->getContempt());
        sliceContempt->setLabel("contempt  "+QString::number(e->getContempt()));
        series->append(sliceContempt);
    }
    if(e->getDisgust() != 0){
        QPieSlice *sliceDisgust = new QPieSlice("disgust", e->getDisgust());
        sliceDisgust->setLabel("disgust  "+QString::number(e->getDisgust()));
        series->append(sliceDisgust);
    }
    if(e->getFear() != 0){
        QPieSlice *sliceFear = new QPieSlice("fear", e->getFear());
        sliceFear->setLabel("fear  "+QString::number(e->getFear()));
        series->append(sliceFear);
    }
    if(e->getHappiness() != 0){
        QPieSlice *sliceHappiness = new QPieSlice("happiness", e->getHappiness());
        sliceHappiness->setLabel("happiness  "+QString::number(e->getHappiness()));
        series->append(sliceHappiness);
    }
    if(e->getNeutral() != 0){
        QPieSlice *sliceNeutral = new QPieSlice("neutral", e->getNeutral());
        sliceNeutral->setLabel("neutral  "+QString::number(e->getNeutral()));
        series->append(sliceNeutral);
    }
    if(e->getSadness() != 0){
        QPieSlice *sliceSadness = new QPieSlice("sadness", e->getSadness());
        sliceSadness->setLabel("sadness  "+QString::number(e->getSadness()));
        series->append(sliceSadness);
    }
    if(e->getSurprise() != 0){
        QPieSlice *sliceSurprise = new QPieSlice("surprise", e->getSurprise());
        sliceSurprise->setLabel("surprise  "+QString::number(e->getSurprise()));
        series->append(sliceSurprise);
    }

    series->setLabelsVisible(true);

    QChart *pieChart = new QChart();
    pieChart->addSeries(series);
    pieChart->setTitle("emotion pie chart");
    pieChart->legend()->setAlignment(Qt::AlignRight);
    pieChart->setAnimationOptions(QChart::AllAnimations);
    pieChart->setGeometry(0,0,430,320);
    pieChart->setMargins(QMargins(0,0,0,0));
    scence->addItem(pieChart);
    ui->piechart->show();
}

void Widget::accessEmotionApi(){
    ui->textBrowser->setText(ui->textBrowser->toPlainText() + "Get image from RTMP stream...\n");
    bool res = true;
    //res = ff->saveRTMPtoImageFile("rtmp://live.hkstv.hk.lxdns.com/live/hks");
    //res = ff->saveRTMPtoImageFile("rtmp://play.bcelive.com/live/lss-gm4k64ts8y7kevfi");
    if(res){
        QString img_file_location = ffmpegDir+tmpImageFileName;
        file = new QFile(img_file_location);
        file->open(QIODevice::ReadOnly);

        QByteArray post_data = file->readAll();
        file->close();
        delete file;
        file = NULL;

        QGraphicsScene *scencePic = new QGraphicsScene();
        image = new QImage(img_file_location);
        *image = image->scaled(ui->pic->size().width(), ui->pic->size().height(), Qt::KeepAspectRatio);
        scencePic->addPixmap(QPixmap::fromImage(*image));
        ui->pic->setScene(scencePic);
        ui->pic->show();
        delete image;
        image = NULL;

        ui->textBrowser->setText(ui->textBrowser->toPlainText() + "Send and receive message from Azure emotion api...\n");
        request.setHeader(QNetworkRequest::ContentLengthHeader, post_data.length());

        reply = manager->post(request, post_data);
    } else{
        ui->textBrowser->setText("save rtmp to image fail!");
    }
}

void Widget::emotionReplyFinished(QNetworkReply *){
    // QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    // std::cout<<"status code:"<<status_code.toString().toStdString()<<endl;
    if(reply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = reply->readAll();

        // std::cout<<"raw json: "<<bytes.toStdString()<<endl;

        QJsonParseError jsonError;
        QJsonArray jArray = QJsonDocument::fromJson(bytes, &jsonError).array();

        //std::cout<<"json array size: "<<jArray.size()<<endl;
        ui->textBrowser->setText(ui->textBrowser->toPlainText() + "Faces recognized: " + QString::number(jArray.size()) + "\n");

        if(jsonError.error == QJsonParseError::NoError){
            //0-anger,1-contempt,2-disgust,3-fear
            //4-happiness,5-neutral,6-sadness,7-surprise
            int emotionArray[8] = {0};
            int max;
            double maxValue, tmpValue;
            QJsonObject jsonPerson;
            QJsonObject scores;
            for(int i=0; i<jArray.size(); i++){
                ui->textBrowser->setText(ui->textBrowser->toPlainText() + "person " + QString::number(i) + ": ");
                jsonPerson = jArray.at(i).toObject();
                scores = jsonPerson.value(QString("scores")).toObject();

                max = 0;
                maxValue = scores.value(QString("anger")).toDouble();
                ui->textBrowser->setText(ui->textBrowser->toPlainText() + "anger: " + QString::number(maxValue*100, 'f', 0) + "% ");

                tmpValue = scores.value(QString("contempt")).toDouble();
                ui->textBrowser->setText(ui->textBrowser->toPlainText() + "contempt: " + QString::number(tmpValue*100, 'f', 0) + "% ");
                if(maxValue < tmpValue){
                    max = 1;
                    maxValue = tmpValue;
                }

                tmpValue = scores.value(QString("disgust")).toDouble();
                ui->textBrowser->setText(ui->textBrowser->toPlainText() + "disgust: " + QString::number(tmpValue*100, 'f', 0) + "% ");
                if(maxValue < tmpValue){
                    max = 2;
                    maxValue = tmpValue;
                }

                tmpValue = scores.value(QString("fear")).toDouble();
                ui->textBrowser->setText(ui->textBrowser->toPlainText() + "fear: " + QString::number(tmpValue*100, 'f', 0) + "%\n");
                if(maxValue < tmpValue){
                    max = 3;
                    maxValue = tmpValue;
                }

                tmpValue = scores.value(QString("happiness")).toDouble();
                ui->textBrowser->setText(ui->textBrowser->toPlainText() + "          happiness: " + QString::number(tmpValue*100, 'f', 0) + "% ");
                if(maxValue < tmpValue){
                    max = 4;
                    maxValue = tmpValue;
                }

                tmpValue = scores.value(QString("neutral")).toDouble();
                ui->textBrowser->setText(ui->textBrowser->toPlainText() + "neutral: " + QString::number(tmpValue*100, 'f', 0) + "% ");
                if(maxValue < tmpValue){
                    max = 5;
                    maxValue = tmpValue;
                }

                tmpValue = scores.value(QString("sadness")).toDouble();
                ui->textBrowser->setText(ui->textBrowser->toPlainText() + "sadness: " + QString::number(tmpValue*100, 'f', 0) + "% ");
                if(maxValue < tmpValue){
                    max = 6;
                    maxValue = tmpValue;
                }

                tmpValue = scores.value(QString("surprise")).toDouble();
                ui->textBrowser->setText(ui->textBrowser->toPlainText() + "surprise: " + QString::number(tmpValue*100, 'f', 0) + "% \n");
                if(maxValue < tmpValue){
                    max = 7;
                    maxValue = tmpValue;
                }

                emotionArray[max] += 1;
            } //end for

            emotion *e = new emotion();
            e->setAnger(emotionArray[0]);
            e->setContempt(emotionArray[1]);
            e->setDisgust(emotionArray[2]);
            e->setFear(emotionArray[3]);
            e->setHappiness(emotionArray[4]);
            e->setNeutral(emotionArray[5]);
            e->setSadness(emotionArray[6]);
            e->setSurprise(emotionArray[7]);

            ui->textBrowser->setText(ui->textBrowser->toPlainText() + "Refresh charts...\n");
            updateLineChart(e);
            updatePieChart(e);
        } else{
            ui->textBrowser->setText("parse json fail!");
        }
     } else{
        ui->textBrowser->setText("get http reply fail!");
     }
     //reply->deleteLater();
}

void Widget::saveAudioInupt(){
    audioRecorder = new QAudioRecorder(this);
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("audio/wav");
    audioSettings.setBitRate(16);
    audioSettings.setChannelCount(1);
    audioSettings.setSampleRate(16000);
    audioSettings.setEncodingMode(QMultimedia::ConstantBitRateEncoding);

    audioRecorder->setAudioSettings(audioSettings);

    audioRecorder->setOutputLocation(QUrl::fromLocalFile(ffmpegDir+tmpAudioInputFileName));
    ui->textBrowser->setText(ui->textBrowser->toPlainText() + "Recording...\n");
    audioRecorder->record();
}

void Widget::getSpeechToken(){
    ui->textBrowser->setText(ui->textBrowser->toPlainText() + "Get baidu speech access token...\n");
    speechTokenReply = speechTokenManager->post(speechTokenRequest, token_post_data);
}

void Widget::speechTokenReplyFinished(QNetworkReply *){
    if(speechTokenReply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = speechTokenReply->readAll();
        QJsonParseError jsonError;
        QJsonObject jObject = QJsonDocument::fromJson(bytes, &jsonError).object();
        speechToken = jObject.value(QString("access_token")).toString();
    } else{
        ui->textBrowser->setText(ui->textBrowser->toPlainText() + "Get baidu speech access token fail!\n");
    }
}

void Widget::accessSpeechApi(){
    audioRecorder->stop();
    speechAudioInputFile = new QFile(ffmpegDir+tmpAudioInputFileName);
    speechAudioInputFile->open(QIODevice::ReadOnly);
    ui->textBrowser->setText(ui->textBrowser->toPlainText() + "Get baidu speech recognization result...\n");
    QByteArray post_data = speechAudioInputFile->readAll();
    speechAudioInputFile->close();
    //speechAudioRequest.setUrl(QUrl(speechAudioUrl+"?cuid=pkuwangyidingbuhuichongfu&token="+speechToken+"&lan=zh"));
    //speechAudioRequest.setHeader(QNetworkRequest::ContentLengthHeader, post_data.length());

    QJsonObject obj;
    obj.insert("format","wav");
    obj.insert("rate",16000);
    obj.insert("channel",1);
    obj.insert("token",speechToken);
    obj.insert("cuid","pkuwangyidingbuhuichongfu");
    obj.insert("speech",QString(post_data.toBase64()));
    obj.insert("len",post_data.length());
    obj.insert("lan","zh");

    QByteArray post_json_byte = QByteArray(QJsonDocument(obj).toJson(QJsonDocument::Compact));
    speechAudioRequest.setHeader(QNetworkRequest::ContentLengthHeader, post_json_byte.length());
    speechAudioRequest.setUrl(QUrl(speechAudioUrl));

    speechAudioReply = speechAudioManager->post(speechAudioRequest, post_json_byte);
}

void Widget::speechReplyFinished(QNetworkReply *){
    //QVariant status_code = speechAudioReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    //std::cout<<"speechaudioreply_status code:"<<status_code.toString().toStdString()<<endl;
    if(speechAudioReply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = speechAudioReply->readAll();
        QJsonParseError jsonError;
        QJsonObject jObject = QJsonDocument::fromJson(bytes, &jsonError).object();
        QJsonArray jArray = jObject.value(QString("result")).toArray();
        ui->textBrowser->setText(ui->textBrowser->toPlainText() + "Speech recognization result: \"" + jArray.at(0).toString() + "\"\n");
        accessLUISApi(jArray.at(0).toString());

        //ui->textBrowser->setText(ui->textBrowser->toPlainText() + "Raw json: " + QString(bytes)  + "\n");

    } else{
        ui->textBrowser->setText(ui->textBrowser->toPlainText() + "Get baidu speech recognization result fail!\n");
    }
}


void Widget::accessLUISApi(QString query){
    luisRequest.setUrl(QUrl(luisUrl+query));
    luisReply = luisManager->get(luisRequest);
}

void Widget::luisReplyFinished(QNetworkReply *){
    if(luisReply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = luisReply->readAll();
        QJsonParseError jsonError;
        QJsonObject jObject = QJsonDocument::fromJson(bytes, &jsonError).object();
        QJsonObject intent = jObject.value(QString("topScoringIntent")).toObject();
        QJsonArray entityArray = jObject.value(QString("entities")).toArray();
        QString item = "";
        QString intentStr = intent.value("intent").toString();
        if(intentStr == "None"){
            intentStr = "其他命令";
        }
        for(int i=0; i<entityArray.size(); i++){
            QJsonObject obj = entityArray.at(i).toObject();
            if(obj.value(QString("type")).toString() == QString("item")){
                item = obj.value(QString("entity")).toString();
            }
        }
        ui->textBrowser->setText(ui->textBrowser->toPlainText() + "LUIS intent:" + intentStr + " item: " + item + "\n");
        accessTTSApi("收到"+intentStr+item);
    } else{
        ui->textBrowser->setText(ui->textBrowser->toPlainText() + "Get LUIS result fail!\n");
    }
}

void Widget::accessTTSApi(QString text){
    ttsRequest.setUrl(QUrl(ttsUrl+"?tex="+text+"&tok="+speechToken+"&lan=zh&ctp=1&cuid=pkuwangyidingbuhuichongfu"));
    ttsReply = ttsManager->get(ttsRequest);
}

void Widget::ttsReplyFinished(QNetworkReply *){
    if(ttsReply->error() == QNetworkReply::NoError)
    {
        qPlayer->setMedia(NULL);
        QByteArray bytes = ttsReply->readAll();
        ttsFile = new QFile(ffmpegDir+"ttsFile.mp3");
        ttsFile->open(QIODevice::WriteOnly);
        ttsFile->write(bytes);
        ttsFile->close();

        qPlayer->setMedia(QUrl::fromLocalFile(ffmpegDir+"ttsFile.mp3"));
        qPlayer->play();
    } else{
        ui->textBrowser->setText(ui->textBrowser->toPlainText() + "Get TTS result fail!\n");
    }
}

void Widget::clearTextBrowser(){
    ui->textBrowser->setText("");
}


Widget::~Widget()
{
    delete ui;
}
