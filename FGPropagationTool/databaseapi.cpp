#include "databaseapi.h"

DatabaseApi::DatabaseApi()
{
    _db =  QSqlDatabase::addDatabase("QSQLITE");
    _db.setDatabaseName("propagation.sqlite");
    _db.open();
}

DatabaseApi::~DatabaseApi()
{
    _db.close();
}



void
DatabaseApi::select_mobile_station(const unsigned &id_session)
{
    QSqlQuery query(_db);
    query.prepare("SELECT * FROM mobile_stations WHERE id_session=:id_sesssion");
    query.bindValue(":id_session", id_session);
    query.exec();
    int id_idx = query.record().indexOf("id");
    int name_idx = query.record().indexOf("name");
    while(query.next())
    {
        // oops
        QString name = query.value(name_idx).toString();
    }

}


void
DatabaseApi::select_ground_stations(const unsigned &id_session)
{
    QSqlQuery query(_db);
    query.prepare("SELECT * FROM ground_stations WHERE id_session=:id_sesssion");
    query.bindValue(":id_session", id_session);
    query.exec();
    while(query.next())
    {
        // oops
    }

}


void
DatabaseApi::select_flightplan_positions(const unsigned &id_session)
{
    QSqlQuery query(_db);
    query.prepare("SELECT * FROM flightplan_positions WHERE id_session=:id_sesssion");
    query.bindValue(":id_session", id_session);
    query.exec();
    while(query.next())
    {
        // oops
    }

}


void
DatabaseApi::add_ground_station(const unsigned &id_session,
                                const double &lon, const double &lat,
                                const unsigned &time)
{
    QSqlQuery query(_db);
    query.prepare("INSERT INTO ground_stations SET "
                  " id_session=:id_session, longitude=:lon,"
                  "latitude = :lat, created_on=:time");
    query.bindValue(":id_session", id_session);
    query.bindValue(":longitude", lon);
    query.bindValue(":latitude", lat);
    query.bindValue(":time", time);
    query.exec();

}

void
DatabaseApi::add_mobile_station(const unsigned &id_session,
                                const double &lon, const double &lat,
                                const unsigned &time)
{
    QSqlQuery query(_db);
    query.prepare("INSERT INTO mobile_stations SET "
                  " id_session=:id_session, longitude=:lon,"
                  "latitude = :lat, created_on=:time");
    query.bindValue(":id_session", id_session);
    query.bindValue(":longitude", lon);
    query.bindValue(":latitude", lat);
    query.bindValue(":time", time);
    query.exec();

}

void
DatabaseApi::add_flightplan_position(const unsigned &id_session,
                                const double &lon, const double &lat,
                                const unsigned &time)
{
    QSqlQuery query(_db);
    query.prepare("INSERT INTO flightplan_positions SET "
                  " id_session=:id_session, longitude=:lon,"
                  "latitude = :lat, created_on=:time");
    query.bindValue(":id_session", id_session);
    query.bindValue(":longitude", lon);
    query.bindValue(":latitude", lat);
    query.bindValue(":time", time);
    query.exec();

}

void
DatabaseApi::delete_ground_station(const unsigned &id_session, const unsigned &id)
{
    QSqlQuery query(_db);
    query.prepare("DELETE FROM ground_stations WHERE "
                  " id_session=:id_session AND id=:id");
    query.bindValue(":id_session", id_session);
    query.bindValue(":id", id);
    query.exec();
}

void
DatabaseApi::delete_flightplan_position(const unsigned &id_session, const unsigned &id)
{
    QSqlQuery query(_db);
    query.prepare("DELETE FROM flightplan_positions WHERE "
                  " id_session=:id_session AND id=:id");
    query.bindValue(":id_session", id_session);
    query.bindValue(":id", id);
    query.exec();
}

void
DatabaseApi::update_ground_station(const unsigned &id_session, const unsigned &id,
                                   const QString &name, const double &frequency, const short &beacon_delay,
                                   const short &transmission_type, const double &elevation_feet,
                                   const double &heading_deg, const double &pitch_deg,
                                   const short &polarization, const double &tx_antenna_height,
                                   const QString &tx_antenna_type, const double &tx_antenna_gain,
                                   const double &tx_line_losses, const double &tx_power_watt,
                                   const double &rx_antenna_height, const QString &rx_antenna_type,
                                   const double &rx_antenna_gain, const double &rx_line_losses,
                                   const double &rx_sensitivity, const unsigned &time)
{
    QSqlQuery query(_db);
    query.prepare("UPDATE ground_stations SET id_session=:id_session,"
                  "name=:name, frequency=:frequency,"
                  "beacon_delay =:beacon_delay, transmission_type=:transmission_type,"
                  "elevation_feet =:elevation_feet, heading_deg=:heading_deg,"
                  "pitch_deg =:pitch_deg, polarization=:polarization,"
                  "tx_antenna_height =:tx_antenna_height, tx_antenna_type=:tx_antenna_type,"
                  "tx_antenna_gain =:tx_antenna_gain, tx_line_losses=:tx_line_losses,"
                  "tx_power_watt = :tx_power_watt, rx_antenna_height=:rx_antenna_height,"
                  "rx_antenna_type=:rx_antenna_type, rx_antenna_gain=:rx_antenna_gain,"
                  "rx_sensitivity=:rx_sensitivity, rx_line_losses=:rx_line_losses,"
                  "created_on=:time WHERE id=:id");
    query.bindValue(":id_session", id_session);
    query.bindValue(":name", name);
    query.bindValue(":frequency", frequency);
    query.bindValue(":beacon_delay", beacon_delay);
    query.bindValue(":transmission_type", transmission_type);
    query.bindValue(":elevation_feet", elevation_feet);
    query.bindValue(":heading_deg", heading_deg);
    query.bindValue(":pitch_deg", pitch_deg);
    query.bindValue(":polarization", polarization);
    query.bindValue(":tx_antenna_height", tx_antenna_height);
    query.bindValue(":tx_antenna_type", tx_antenna_type);
    query.bindValue(":tx_antenna_gain", tx_antenna_gain);
    query.bindValue(":tx_line_losses", tx_line_losses);
    query.bindValue(":tx_power_watt", tx_power_watt);
    query.bindValue(":rx_antenna_height", rx_antenna_height);
    query.bindValue(":rx_antenna_type", rx_antenna_type);
    query.bindValue(":rx_antenna_gain", rx_antenna_gain);
    query.bindValue(":rx_sensitivity", rx_sensitivity);
    query.bindValue(":rx_line_losses", rx_line_losses);
    query.bindValue(":time", time);
    query.bindValue(":id", id);
    query.exec();

}


void
DatabaseApi::update_mobile_station(const unsigned &id_session, const unsigned &id,
                                   const QString &name, const double &frequency, const double &elevation_feet,
                                   const double &heading_deg, const double &tx_power_watt,
                                   const unsigned &terrain_following,
                                   const double &speed, const unsigned &time)
{
    QSqlQuery query(_db);
    query.prepare("UPDATE ground_stations SET id_session=:id_session,"
                  "name=:name, frequency=:frequency,"
                  "elevation_feet =:elevation_feet, heading_deg=:heading_deg,"
                  "tx_power_watt = :tx_power_watt,"
                  "terrain_following=:terrain_following, speed=:speed,"
                  "created_on=:time WHERE id=:id");
    query.bindValue(":id_session", id_session);
    query.bindValue(":name", name);
    query.bindValue(":frequency", frequency);
    query.bindValue(":elevation_feet", elevation_feet);
    query.bindValue(":heading_deg", heading_deg);
    query.bindValue(":tx_power_watt", tx_power_watt);
    query.bindValue(":terrain_following", terrain_following);
    query.bindValue(":speed", speed);
    query.bindValue(":time", time);
    query.bindValue(":id", id);
    query.exec();

}


void
DatabaseApi::update_flightplan_position(const double &altitude, const unsigned &id_session, const unsigned &id)
{
    QSqlQuery query(_db);
    query.prepare("UPDATE flightplan_positions SET altitude=:altitude WHERE "
                  " id_session=:id_session AND id=:id");
    query.bindValue(":altitude", altitude);
    query.bindValue(":id_session", id_session);
    query.bindValue(":id", id);
    query.exec();
}
