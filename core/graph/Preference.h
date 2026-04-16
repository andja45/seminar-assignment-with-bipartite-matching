#ifndef SEMINARALLOCATIONENGINE_PREFERENCE_H
#define SEMINARALLOCATIONENGINE_PREFERENCE_H

struct Preference {
    int student;
    int topic;
    int rank;
}; // student gives a list of preferences for topics, ranked from 1 (most preferred) to N (least preferred)

#endif
