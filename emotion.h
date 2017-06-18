#ifndef EMOTION_H
#define EMOTION_H

class emotion{
public :
    emotion();
    emotion(int _anger, int _contempt, int _disgust, int _fear, int _happiness, int _neutral, int _sadness, int _surprise);

    int getAnger() const;
    void setAnger(int value);

    int getContempt() const;
    void setContempt(int value);

    int getDisgust() const;
    void setDisgust(int value);

    int getFear() const;
    void setFear(int value);

    int getHappiness() const;
    void setHappiness(int value);

    int getNeutral() const;
    void setNeutral(int value);

    int getSadness() const;
    void setSadness(int value);

    int getSurprise() const;
    void setSurprise(int value);

private:
    int anger;
    int contempt;
    int disgust;
    int fear;
    int happiness;
    int neutral;
    int sadness;
    int surprise;

};

#endif // EMOTION_H
