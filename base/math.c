#include <stdio.h>
#include "math.h"

double deg2rad(double deg) {
    return deg * (M_PI / 180);
}

double rad2deg(double rad) {
    return rad * (180 / M_PI );
}

vector getVector(vector pos1, vector pos2) {
	vector vec;
	vec.x = (double) (pos2.x - pos1.x);
	vec.y = (double) (pos2.y - pos1.y);

	return vec;
}

double getDistance(double x, double y) {
	return y - x;
}

double vectorDistance(vector* vec) {
	return getDistance(vec->x, vec->y);
}

vector sumVector(vector* v1, vector* v2) {
    vector res;

    res.x = v1->x + v2->x;
    res.y = v1->y + v2->y;

    return res;
}

vector diffVector(vector* v1, vector* v2) {
    vector res;

    res.x = v1->x - v2->x;
    res.y = v1->y - v2->y;

    return res;
}

vector rotateVector(vector* v, double angle) {
    vector res;

    fprintf(stdout, "-- ROTATING: %lf\n", angle);
    angle = deg2rad(angle);
    fprintf(stdout, "-- ROTATING (Rad): %lf\n", angle);


    fprintf(stdout, "-- cos(%.0lf) = %.2lf\n", angle, cos(angle));
    fprintf(stdout, "-- sin(%.0lf) = %.2lf\n", angle, sin(angle));

    fprintf(stdout, "-- X * cos(%.0lf) = %.2lf\n", angle, v->x * cos(angle));
    fprintf(stdout, "-- Y * sin(%.0lf) = %.2lf\n", angle, v->y * sin(angle));

    res.x = (v->x * cos(angle)) - (v->y * sin(angle));
    res.y = (v->x * sin(angle)) + (v->y * cos(angle));

    return res;
}

bool operator==(vector v, vector v2) {
    return (v.x == v2.x) && (v.y == v2.y);
}

bool operator!=(vector v, vector v2) {
    return !(v == v2);
}
