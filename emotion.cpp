#include "emotion.h"

emotion::emotion(){

}

emotion::emotion(int _anger, int _contempt, int _disgust, int _fear, int _happiness, int _neutral, int _sadness, int _surprise)
{
anger = _anger;
contempt = _contempt;
disgust = _disgust;
fear = _fear;
happiness = _happiness;
neutral = _neutral;
sadness = _sadness;
surprise = _surprise;
}

int emotion::getAnger() const
{
return anger;
}

void emotion::setAnger(int value)
{
anger = value;
}

int emotion::getContempt() const
{
return contempt;
}

void emotion::setContempt(int value)
{
contempt = value;
}

int emotion::getDisgust() const
{
return disgust;
}

void emotion::setDisgust(int value)
{
disgust = value;
}

int emotion::getFear() const
{
return fear;
}

void emotion::setFear(int value)
{
fear = value;
}

int emotion::getHappiness() const
{
return happiness;
}

void emotion::setHappiness(int value)
{
happiness = value;
}

int emotion::getNeutral() const
{
return neutral;
}

void emotion::setNeutral(int value)
{
neutral = value;
}

int emotion::getSadness() const
{
return sadness;
}

void emotion::setSadness(int value)
{
sadness = value;
}

int emotion::getSurprise() const
{
return surprise;
}

void emotion::setSurprise(int value)
{
surprise = value;
}
