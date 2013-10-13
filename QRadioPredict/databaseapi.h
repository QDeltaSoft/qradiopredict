// Written by Adrian Musceac YO8RZZ at gmail dot com, started August 2013.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.


#ifndef DATABASEAPI_H
#define DATABASEAPI_H

#include <QtSql>
#include <QVector>
#include <QString>
#include <math.h>
#include "mobilestation.h"
#include "groundstation.h"
#include "flightplanpoints.h"
#include "aprsstation.h"
#include "signal.h"
#include "flightgearprefs.h"

/**
 * @brief Sqlite database access class
 *      All database storage is processed here
 */
class DatabaseApi
{
public:
    DatabaseApi();
    ~DatabaseApi();

    QVector<AprsStation *>
    similar_stations(QString &callsign, int time);

    QVector<AprsStation *>
    filter_aprs_stations(int time);

    QVector<AprsStation *>
    select_aprs_stations();

    void
    update_aprs_stations(AprsStation * s);

    bool
    select_commands(const unsigned &id_session);

    QVector<FlightgearPrefs *>
    select_prefs();

    void
    savePrefs(FlightgearPrefs *p);

    QVector<Signal *>
    select_signals(const unsigned &id_session);

    void
    update_signals(const unsigned &id_station, const unsigned &id_session, const Signal *s);

    void
    update_replays(const unsigned &id_station, const unsigned &id_session,
                                const double &mobile_longitude, const double &mobile_latitude,
                                QString &id_replay, const Signal *s);



    QVector<MobileStation *>
    select_mobile_station(const unsigned &id_session);



    QVector<GroundStation *>
    select_ground_stations(const unsigned &id_session);


    QVector<FlightPlanPoints *>
    select_flightplan_positions(const unsigned &id_session);



    void
    add_ground_station(const unsigned &id_session,
                                    const double &lon, const double &lat,
                                    const unsigned &time);


    void
    add_mobile_station(const unsigned &id_session,
                                    const double &lon, const double &lat,
                                    const unsigned &time);


    void
    add_flightplan_position(const unsigned &id_session,
                                    const double &lon, const double &lat,
                                    const unsigned &time);


    void
    delete_ground_station(const unsigned &id_session, const unsigned &id);


    void
    delete_flightplan_position(const unsigned &id_session, const unsigned &id);



    void
    update_ground_station(const unsigned &id_session, const unsigned &id,
                                       const QString &name, const double &frequency, const short &beacon_delay,
                                       const short &transmission_type, const double &elevation_feet,
                                       const double &heading_deg, const double &pitch_deg,
                                       const short &polarization, const double &tx_antenna_height,
                                       const QString &tx_antenna_type, const double &tx_antenna_gain,
                                       const double &tx_line_losses, const double &tx_power_watt,
                                       const double &rx_antenna_height, const QString &rx_antenna_type,
                                       const double &rx_antenna_gain, const double &rx_line_losses,
                                       const double &rx_sensitivity, const unsigned &time);



    void
    update_mobile_station(const unsigned &id_session, const unsigned &id,
                                       const QString &name, const double &frequency, const double &elevation_feet,
                                       const double &heading_deg, const double &tx_power_watt,
                                       const unsigned &terrain_following,
                                       const double &speed, const unsigned &time);



    void
    update_mobile_position(const unsigned &id_session, const double &longitude, const double &latitude);



    void
    update_flightplan_position(const double &altitude, const unsigned &id_session, const unsigned &id);



private:
    QSqlDatabase _db;

};

#endif // DATABASEAPI_H
