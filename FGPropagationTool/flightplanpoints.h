#ifndef FLIGHTPLANPOINTS_H
#define FLIGHTPLANPOINTS_H

/**
 * @brief Struct holding the properties of a Waypoint
 */
class FlightPlanPoints
{
public:
    unsigned id;
    unsigned id_session;

    double longitude;
    double latitude;
    double altitude;
    unsigned created_on;
    FlightPlanPoints();
};

#endif // FLIGHTPLANPOINTS_H
