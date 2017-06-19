# Data for qRadioPredict

For the calculation of the propagation model, it is important to have a altitude model of the surroundings.
As such altitude data is not provided as such, we need a source for such a digital elevation model (DEM).
Such data is provided in so called tiles, which cover only a certain area.
Another factor that plays into radio propagation is the land cover.
Eg. wether we have grassland, forest or urban area. 

## Script for getting DEM tiles 
The provided script will fetch a whole continent from the shuttle SRTM data.
```
./get-datafiles.sh -c "Eurasia"
```
will fetch all tiles for Europe and Asia.

Valid continents are `North_America`, `South_America`, `Africa`,
        `Eurasia`, `Australia` and  `Islands`
### Data size
Take care, a whole continent can take up quite a lot of space.
`Eurasia` will take approximately 16GB of diskspace.

### DEM Sources
Today there are many different provider of such data:
- Shuttle SRTM: http://dds.cr.usgs.gov/srtm/version2_1/SRTM3/
- Higher Res: http://e4ftl01.cr.usgs.gov/SRTM/SRTMGL1.003/2000.02.11/
- European DEM: https://www.eea.europa.eu/data-and-maps/data/eu-dem

## Land Cover Shapefile
The land cover is provided as shapefile by the CIGAR project.
https://www.eea.europa.eu/data-and-maps/data/clc-2006-vector-data-version-2
 
