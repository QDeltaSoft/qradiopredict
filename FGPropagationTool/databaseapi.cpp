#include "databaseapi.h"


DatabaseApi::DatabaseApi()
{
    _db =  QSqlDatabase::addDatabase("QSQLITE");
    _db.setDatabaseName("propagation.sqlite");
    if(!_db.open())
    {
        qDebug() << "Could not connect to database!";
    }
}

DatabaseApi::~DatabaseApi()
{
    _db.close();
}

bool
DatabaseApi::select_commands(const unsigned &id_session)
{
    QSqlQuery query(_db);
    query.prepare("SELECT * FROM commands WHERE id_session=:id_session LIMIT 1");
    query.bindValue(":id_session", id_session);
    query.exec();
    int update_signals_idx = query.record().indexOf("update_signals");
    query.next();
    QString command = query.value(update_signals_idx).toString();
    if (command=="false")
        return false;
    else
        return true;
}

QVector<Signal *>
DatabaseApi::select_signals(const unsigned &id_session)
{
    QVector<Signal *> station_signals;
    QSqlQuery query(_db);
    query.prepare("SELECT * FROM signals LEFT JOIN ground_stations ON signals.id_station=ground_stations.id"
                   "WHERE ground_stations.id!='' AND signals.id_session=:id_session");
    query.bindValue(":id_session", id_session);
    query.exec();

    int name_idx = query.record().indexOf("name");
    int frequency_idx = query.record().indexOf("frequency");
    int signal_dbm_idx = query.record().indexOf("signal_dbm");
    int signal_idx = query.record().indexOf("signal");
    int field_strength_uv_idx = query.record().indexOf("field_strength_uv");
    int link_budget_idx = query.record().indexOf("link_budget");
    int terrain_attenuation_idx = query.record().indexOf("terrain_attenuation");
    int clutter_attenuation_idx = query.record().indexOf("clutter_attenuation");
    int prop_mode_idx = query.record().indexOf("prop_mode");
    while(query.next())
    {
        Signal *s = new Signal;
        s->station_name = query.value(name_idx).toString();
        s->frequency = query.value(frequency_idx).toDouble();
        s->signal_dbm = query.value(signal_dbm_idx).toDouble();
        s->signal = query.value(signal_idx).toDouble();
        s->field_strength_uv = query.value(field_strength_uv_idx).toDouble();
        s->link_budget = query.value(link_budget_idx).toDouble();
        s->terrain_attenuation = query.value(terrain_attenuation_idx).toDouble();
        s->clutter_attenuation = query.value(clutter_attenuation_idx).toDouble();
        s->prop_mode = query.value(prop_mode_idx).toString();
        station_signals.push_back(s);
    }
    return station_signals;
}


void
DatabaseApi::update_signals(const unsigned &id_station, const unsigned &id_session, const Signal * s)
{
    QSqlQuery query(_db);
    QSqlQuery query2(_db);
    query2.prepare("SELECT * FROM signals WHERE id_station=:id_station");
    query2.bindValue(":id_station", id_station);
    query2.exec();
    if(query2.size()>0)
    {
        query.prepare("UPDATE signals SET signal_dbm=:signal_dbm, signal=:signal,"
                       "field_strength_uv=:field_strength_uv, link_budget=:link_budget, "
                       "terrain_attenuation=:terrain_attenuation, "
                       "clutter_attenuation=:clutter_attenuation, prop_mode=:prop_mode"
                       "WHERE id_session=:id_session AND id_station=:id_station");
        query.bindValue(":signal_dbm", s->signal_dbm);
        query.bindValue(":signal", s->signal);
        query.bindValue(":field_strength_uv", s->field_strength_uv);
        query.bindValue(":link_budget", s->link_budget);
        query.bindValue(":terrain_attenuation", s->terrain_attenuation);
        query.bindValue(":clutter_attenuation", s->clutter_attenuation);
        query.bindValue(":prop_mode", s->prop_mode);
        query.bindValue(":id_session", id_session);
        query.bindValue(":id_station", id_station);
        query.exec();

    }
    else
    {
        query.prepare("INSERT INTO signals (signal_dbm, signal,"
                       "field_strength_uv, link_budget, "
                       "terrain_attenuation, "
                       "clutter_attenuation, prop_mode,"
                       "id_session, id_station) VALUES (:signal_dbm,"
                      ":signal,:field_strength_uv,:link_budget,"
                      ":terrain_attenuation,:clutter_attenuation,"
                      ":prop_mode,:id_session,:id_station)");
        query.bindValue(":signal_dbm", s->signal_dbm);
        query.bindValue(":signal", s->signal);
        query.bindValue(":field_strength_uv", s->field_strength_uv);
        query.bindValue(":link_budget", s->link_budget);
        query.bindValue(":terrain_attenuation", s->terrain_attenuation);
        query.bindValue(":clutter_attenuation", s->clutter_attenuation);
        query.bindValue(":prop_mode", s->prop_mode);
        query.bindValue(":id_session", id_session);
        query.bindValue(":id_station", id_station);
        query.exec();

    }

}

void
DatabaseApi::update_replays(const unsigned &id_station, const unsigned &id_session,
                            const double &mobile_longitude, const double &mobile_latitude,
                            QString &id_replay, const Signal *s)
{
    QSqlQuery query(_db);
    query.prepare("INSERT INTO replays (mobile_longitude, mobile_latitude, "
                  "signal_dbm, signal,"
                   "field_strength_uv, link_budget, "
                   "terrain_attenuation, "
                   "clutter_attenuation, prop_mode,"
                   "id_session, id_station, id_replay) VALUES ("
                  ":mobile_longitude, :mobile_latitude, :signal_dbm,"
                  ":signal,:field_strength_uv,:link_budget,"
                  ":terrain_attenuation,:clutter_attenuation,"
                  ":prop_mode,:id_session,:id_station, :id_replay)");
    query.bindValue(":mobile_longitude", mobile_longitude);
    query.bindValue(":mobile_latitude", mobile_latitude);
    query.bindValue(":signal_dbm", s->signal_dbm);
    query.bindValue(":signal", s->signal);
    query.bindValue(":field_strength_uv", s->field_strength_uv);
    query.bindValue(":link_budget", s->link_budget);
    query.bindValue(":terrain_attenuation", s->terrain_attenuation);
    query.bindValue(":clutter_attenuation", s->clutter_attenuation);
    query.bindValue(":prop_mode", s->prop_mode);
    query.bindValue(":id_session", id_session);
    query.bindValue(":id_station", id_station);
    query.bindValue(":id_replay", id_replay);
    query.exec();
}


QVector<MobileStation *>
DatabaseApi::select_mobile_station(const unsigned &id_session)
{
    QVector<MobileStation *> mobile_stations;
    QSqlQuery query(_db);
    query.prepare("SELECT * FROM mobile_stations WHERE id_session=:id_session ORDER BY id ASC");
    query.bindValue(":id_session", id_session);
    query.exec();

    int id_idx = query.record().indexOf("id");
    int name_idx = query.record().indexOf("name");
    int id_session_idx = query.record().indexOf("id_session");
    int longitude_idx = query.record().indexOf("longitude");
    int latitude_idx = query.record().indexOf("latitude");
    int current_longitude_idx = query.record().indexOf("current_longitude");
    int current_latitude_idx = query.record().indexOf("current_latitude");
    int heading_deg_idx = query.record().indexOf("heading_deg");
    int elevation_feet_idx = query.record().indexOf("elevation_feet");
    int speed_idx = query.record().indexOf("speed");
    int frequency_idx = query.record().indexOf("frequency");
    int tx_power_watt_idx = query.record().indexOf("tx_power_watt");
    int terrain_following_idx = query.record().indexOf("terrain_following");
    int created_on_idx = query.record().indexOf("created_on");
    while(query.next())
    {
        MobileStation *mobile =new MobileStation;
        mobile->id = query.value(id_idx).toInt();
        mobile->id_session = query.value(id_session_idx).toInt();
        mobile->name = query.value(name_idx).toString();
        mobile->longitude = query.value(longitude_idx).toDouble();
        mobile->latitude = query.value(latitude_idx).toDouble();
        mobile->current_longitude = query.value(current_longitude_idx).toDouble();
        mobile->current_latitude = query.value(current_latitude_idx).toDouble();
        mobile->heading_deg = query.value(heading_deg_idx).toDouble();
        mobile->elevation_feet = query.value(elevation_feet_idx).toDouble();
        mobile->speed = query.value(speed_idx).toDouble();
        mobile->frequency = query.value(frequency_idx).toDouble();
        mobile->tx_power_watt = query.value(tx_power_watt_idx).toDouble();
        mobile->terrain_following = query.value(terrain_following_idx).toInt();
        mobile->created_on = query.value(created_on_idx).toInt();
        mobile_stations.push_back(mobile);
    }
    return mobile_stations;

}


QVector<GroundStation *>
DatabaseApi::select_ground_stations(const unsigned &id_session)
{
    QVector<GroundStation *> ground_stations;
    QSqlQuery query(_db);
    query.prepare("SELECT * FROM ground_stations WHERE id_session=:id_session ORDER BY id ASC");
    query.bindValue(":id_session", id_session);
    query.exec();
    int id_idx = query.record().indexOf("id");
    int name_idx = query.record().indexOf("name");
    int id_session_idx = query.record().indexOf("id_session");
    int longitude_idx = query.record().indexOf("longitude");
    int latitude_idx = query.record().indexOf("latitude");
    int heading_deg_idx = query.record().indexOf("heading_deg");
    int transmission_type_idx = query.record().indexOf("transmission_type");
    int elevation_feet_idx = query.record().indexOf("elevation_feet");
    int beacon_delay_idx = query.record().indexOf("beacon_delay");
    int frequency_idx = query.record().indexOf("frequency");
    int pitch_deg_idx = query.record().indexOf("pitch_deg");
    int polarization_idx = query.record().indexOf("polarization");
    int rx_antenna_height_idx = query.record().indexOf("rx_antenna_height");
    int rx_antenna_type_idx = query.record().indexOf("rx_antenna_type");
    int rx_antenna_gain_idx = query.record().indexOf("rx_antenna_gain");
    int rx_line_losses_idx = query.record().indexOf("rx_line_losses");
    int rx_sensitivity_idx = query.record().indexOf("rx_sensitivity");
    int tx_power_watt_idx = query.record().indexOf("tx_power_watt");
    int tx_antenna_height_idx = query.record().indexOf("tx_antenna_height");
    int tx_antenna_type_idx = query.record().indexOf("tx_antenna_type");
    int tx_antenna_gain_idx = query.record().indexOf("tx_antenna_gain");
    int tx_line_losses_idx = query.record().indexOf("tx_line_losses");
    int enabled_idx = query.record().indexOf("enabled");
    int created_on_idx = query.record().indexOf("created_on");
    while(query.next())
    {
        GroundStation *ground =new GroundStation;
        ground->id = query.value(id_idx).toInt();
        ground->id_session = query.value(id_session_idx).toInt();
        ground->name = query.value(name_idx).toString();
        ground->longitude = query.value(longitude_idx).toDouble();
        ground->latitude = query.value(latitude_idx).toDouble();
        ground->pitch_deg = query.value(pitch_deg_idx).toDouble();
        ground->heading_deg = query.value(heading_deg_idx).toDouble();
        ground->elevation_feet = query.value(elevation_feet_idx).toDouble();
        ground->transmission_type = query.value(transmission_type_idx).toInt();
        ground->beacon_delay = query.value(beacon_delay_idx).toInt();
        ground->frequency = query.value(frequency_idx).toDouble();
        ground->tx_power_watt = query.value(tx_power_watt_idx).toDouble();
        ground->polarization = query.value(polarization_idx).toInt();
        ground->rx_antenna_height = query.value(rx_antenna_height_idx).toDouble();
        ground->rx_antenna_gain = query.value(rx_antenna_gain_idx).toDouble();
        ground->rx_antenna_type = query.value(rx_antenna_type_idx).toString();
        ground->rx_line_losses = query.value(rx_line_losses_idx).toDouble();
        ground->rx_sensitivity = query.value(rx_sensitivity_idx).toDouble();
        ground->tx_antenna_height = query.value(tx_antenna_height_idx).toDouble();
        ground->tx_antenna_gain = query.value(tx_antenna_gain_idx).toDouble();
        ground->tx_antenna_type = query.value(tx_antenna_type_idx).toString();
        ground->tx_line_losses = query.value(tx_line_losses_idx).toDouble();
        ground->enabled = query.value(enabled_idx).toInt();

        ground->created_on = query.value(created_on_idx).toInt();
        ground_stations.push_back(ground);
    }

    return ground_stations;

}


QVector<FlightPlanPoints *>
DatabaseApi::select_flightplan_positions(const unsigned &id_session)
{
    QVector<FlightPlanPoints *> fp_points;
    QSqlQuery query(_db);
    query.prepare("SELECT * FROM flightplan_positions WHERE id_session=:id_session ORDER BY id ASC");
    query.bindValue(":id_session", id_session);
    query.exec();
    int id_idx = query.record().indexOf("id");
    int id_session_idx = query.record().indexOf("id_session");
    int longitude_idx = query.record().indexOf("longitude");
    int latitude_idx = query.record().indexOf("latitude");
    int altitude_idx = query.record().indexOf("altitude");
    int created_on_idx = query.record().indexOf("created_on");
    while(query.next())
    {
        FlightPlanPoints *fp = new FlightPlanPoints;
        fp->id = query.value(id_idx).toInt();
        fp->id_session = query.value(id_session_idx).toInt();
        fp->longitude = query.value(longitude_idx).toDouble();
        fp->latitude = query.value(latitude_idx).toDouble();
        fp->altitude = query.value(altitude_idx).toDouble();
        fp->created_on = query.value(created_on_idx).toInt();
        fp_points.push_back(fp);
    }

    return fp_points;

}


void
DatabaseApi::add_ground_station(const unsigned &id_session,
                                const double &lon, const double &lat,
                                const unsigned &time)
{
    QSqlQuery query(_db);
    query.prepare("INSERT INTO ground_stations ("
                  " id_session, longitude, "
                  "latitude, frequency, beacon_delay, transmission_type, elevation_feet, "
                  "polarization, rx_sensitivity, tx_power_watt, enabled, created_on) VALUES (:id_session, "
                  ":lon, :lat, :frequency, :beacon_delay, :transmission_type, :elevation_feet, "
                  ":polarization, :rx_sensitivity, :tx_power_watt, :enabled, :time)");
    query.bindValue(":id_session", id_session);
    query.bindValue(":lon", lon);
    query.bindValue(":lat", lat);
    query.bindValue(":frequency", 144.5);
    query.bindValue(":beacon_delay", 2);
    query.bindValue(":transmission_type", 1);
    query.bindValue(":elevation_feet", 0);
    query.bindValue(":polarization", 1);
    query.bindValue(":rx_sensitivity", -120.0);
    query.bindValue(":tx_power_watt", 1.0);
    query.bindValue(":enabled", 1);
    query.bindValue(":time", time);
    query.exec();

}

void
DatabaseApi::add_mobile_station(const unsigned &id_session,
                                const double &lon, const double &lat,
                                const unsigned &time)
{
    QSqlQuery query(_db);
    QSqlQuery query2(_db);
    query2.prepare("SELECT id FROM mobile_stations WHERE id_session=:id_session");
    query2.bindValue(":id_session", id_session);
    query2.exec();
    if(!query2.next())
    {
        query.prepare("INSERT INTO mobile_stations ("
                      " id_session, longitude, "
                      "latitude, created_on) VALUES (:id_session, "
                      ":lon, :lat, :time)");
        query.bindValue(":id_session", id_session);
        query.bindValue(":lon", lon);
        query.bindValue(":lat", lat);
        query.bindValue(":time", time);
        query.exec();
    }
    else
    {

        query.prepare("UPDATE mobile_stations SET "
                      " id_session=:id_session, longitude=:lon, "
                      "latitude=:lat, created_on=:time WHERE id=:id");
        query.bindValue(":id_session", id_session);
        query.bindValue(":lon", lon);
        query.bindValue(":lat", lat);
        query.bindValue(":time", time);
        query.bindValue(":id", query2.value(0));
        query.exec();
        qDebug() << query.lastError().text();
    }


}

void
DatabaseApi::add_flightplan_position(const unsigned &id_session,
                                const double &lon, const double &lat,
                                const unsigned &time)
{
    QSqlQuery query(_db);
    query.prepare("INSERT INTO flightplan_positions ("
                  " id_session, longitude, "
                  "latitude, altitude, created_on) VALUES (:id_session, "
                  ":lon, :lat, :alt, :time)");
    query.bindValue(":id_session", id_session);
    query.bindValue(":lon", lon);
    query.bindValue(":lat", lat);
    query.bindValue(":alt", 0);
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
    QSqlQuery query2(_db);
    query2.prepare("SELECT id FROM mobile_stations WHERE id_session=:id_session");
    query2.bindValue(":id_session", id_session);
    query2.exec();
    if(query2.next())
    {
        query.prepare("UPDATE mobile_stations SET id_session=:id_session,"
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

}


void
DatabaseApi::update_mobile_position(const unsigned &id_session, const double &longitude, const double &latitude)
{
    QSqlQuery query(_db);
    query.prepare("UPDATE mobile_stations SET "
                  "current_longitude =:current_longitude, current_latitude=:current_latitude "
                  " WHERE id_session=:id_session");
    query.bindValue(":id_session", id_session);
    query.bindValue(":current_longitude", longitude);
    query.bindValue(":current_latitude", latitude);
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
